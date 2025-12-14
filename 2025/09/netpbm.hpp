#include <fstream>
#include <vector>
#include <stdexcept>
using namespace std;

namespace ash {

struct Pixel {
    unsigned char r, g, b;

    unsigned char& operator[](int i) {
        unsigned char* arr = &r;
        return arr[i];
    }

    unsigned char operator[](int i) const {
        const unsigned char* arr = &r;
        return arr[i];
    }
};

Pixel kBlack = {0, 0, 0};
Pixel kWhite = {255, 255, 255};

std::ostream& operator<<(std::ostream& out, const Pixel& pixel) {
    return out << '(' << static_cast<int>(pixel.r) << ", "
                      << static_cast<int>(pixel.g) << ", "
                      << static_cast<int>(pixel.b) << ')';
}

namespace detail {

std::string NextToken(std::istream& in) {
    std::string token;
    // get first non-space out-of-comment character
    in >> std::ws;
    while (in && in.peek() == '#') {
        in.ignore(numeric_limits<streamsize>::max(), '\n');
        in >> std::ws;
    }
    // keep reading characters, ignoring comments, until a whitespace
    // is encountered
    char c;
    while (in.get(c) && !std::isspace(c)) {
        if (c == '#')
            in.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        else
            token += c;
    }
    return token;
}

int NextInt(std::istream& in) {
    std::string token = NextToken(in);
    size_t processed;
    int value = std::stoi(token, &processed);
    if (processed != token.size())
        throw std::invalid_argument(
                "Cannot convert '" + token + "' to integer");
    return value;
}

}

class Image {
    public:
        static Image Load(const std::string& filename) {
            std::ifstream in(filename);
            if (!in)
                throw std::invalid_argument("Couldn't open " + filename);
            std::string magic(2,' ');
            if (!in.get(magic[0]) || !in.get(magic[1]) || !(magic=="P3" || magic=="P6"))
                throw std::invalid_argument(
                        "File " + filename + " is not a proper netpbm file");
            size_t width = detail::NextInt(in);
            size_t height = detail::NextInt(in);
            size_t depth = detail::NextInt(in);
            if (depth != 255)
                throw std::runtime_error(
                        "Depth of " + std::to_string(depth) + " not allowed");
            Image image(width, height);
            if (magic == "P3")
                image.LoadAscii(in);
            else // if (magic == "P6")
                image.LoadBinary(in);
            return image;
        }

        void Save(const std::string& filename, bool binary = true) {
            auto mode = ios::out;
            if (binary)
                mode = mode | std::ios::binary;
            std::ofstream out(filename, mode);
            if (!out)
                throw std::invalid_argument("Couldn't open " + filename);
            out << 'P' << (binary? '6' : '3') << ' '
                    << _width << ' ' << _height << " 255\n";
            if (binary) {
                for (const auto& pixel : _data)
                    out << pixel.r << pixel.g << pixel.b;
            }
            else
                for (const auto& pixel : _data)
                    out << ((int)pixel.r) << ' '
                        << ((int)pixel.g) << ' '
                        << ((int)pixel.b) << '\n';
        }

        Image(size_t width, size_t height, const Pixel& color=kBlack) :
            _width(width), _height(height), _data(width*height, color) {
        }

        const Pixel& operator()(int x, int y) const {
            int index = (_height-y-1)*_width + x;
            return _data[index];
        }

        Pixel& operator()(int x, int y) {
            return const_cast<Pixel&>(
                    static_cast<const Image&>(*this)(x, y));
        }

        size_t Width() const {
            return _width;
        }

        size_t Height() const {
            return _height;
        }
        
        void Clear(const Pixel& color = kBlack)
        {
            for (auto& pixel : _data)
            {
                pixel = color;
            }
        }

    private:

        void LoadBinary(std::ifstream& in) {
            size_t expected_bytes = _data.size()*sizeof(Pixel);
            in.read(reinterpret_cast<char*>(_data.data()), expected_bytes);
            size_t read = in.gcount();
            if (read != expected_bytes) {
                throw runtime_error("Expected " + to_string(expected_bytes) +
                        " bytes, received " + to_string(read) + " instead");
            }
        }

        void LoadAscii(std::ifstream& in) {
            for (Pixel& pixel : _data) {
                pixel.r = detail::NextInt(in);
                pixel.g = detail::NextInt(in);
                pixel.b = detail::NextInt(in);
            }
        }
 
        size_t _width, _height;
        std::vector<Pixel> _data;
};

void DrawVerticalSegment(Image& image, int x, int y0, int y1, const Pixel& color)
{
    for (int y = min(y0,y1); y <= max(y0,y1); ++y)
    {
        image(x,y) = color;
    }
}

void DrawHorizontalSegment(Image& image, int y, int x0, int x1, const Pixel& color)
{
    for (int x = min(x0,x1); x <= max(x0,x1); ++x)
    {
        image(x,y) = color;
    }
}

void DrawSegment(Image& image, int x0, int y0, int x1, int y1, const Pixel& color)
{
    if (x0 == x1)
    {
        DrawVerticalSegment(image, x0, y0, y1, color);
    }
    else if (y0 == y1)
    {
        DrawHorizontalSegment(image, y0, x0, x1, color);
    }
}

void DrawCross(Image& image, int x0, int y0, int size, const Pixel& color)
{
    int w = image.Width();
    int h = image.Height();
    image(x0,y0) = color;
    for (int i = 1; i < size; ++i)
    {
        if (x0-i >=  0 && y0-i >= 0) image(x0-i,y0-i) = color;
        if (x0-i >= 0 && y0+i < h-1) image(x0-i,y0+i) = color;
        if (x0+i < w-1 && y0-i >= 0) image(x0+i,y0-i) = color;
        if (x0+i < w-1 && y0+i < h-1) image(x0+i,y0+i) = color;
    }
}

void FillRectangle(Image& image, int x0, int y0, int x1, int y1, const Pixel& color)
{
    for (int x = min(x0,x1); x <= max(x0,x1); ++x)
    {
        for (int y = min(y0,y1); y <= max(y0,y1); ++y)
        {
            image(x,y) = color;
        }
    }
}

} // namespace ash

