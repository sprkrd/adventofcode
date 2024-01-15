#include <array>
#include <cassert>
#include <iostream>
#include <string>
#include <tuple>
#include <variant>
#include <vector>
using namespace std;

constexpr char k_wall = '#';
constexpr char k_empty = '.';
constexpr char k_eor = ' ';
constexpr char k_character = '@';

enum class Face { front = 0, back, left, right, top, bottom };
enum class Direction { right = 0, down, left, up };

typedef pair<int,int> Point;

constexpr Point direction_vectors[] { {1,0}, {0,1}, {-1,0}, {0,-1} };
const char* direction_symbols = ">v<^";

struct Pose {
    int x;
    int y;
    Direction dir;

    Pose(int x = 0, int y = 0, Direction dir = Direction::right) : x(x), y(y), dir(dir) {
    }
};

inline int wrap(int x, int m) {
    return (x%m + m)%m;
}

Pose rotate(const Pose& pose_in, int times, int length_side) {
    times = wrap(times, 4);
    Pose pose_out;
    pose_out.dir = static_cast<Direction>(wrap(static_cast<int>(pose_in.dir) - times, 4));
    switch (times) {
        case 0:
            pose_out = pose_in;
            break;
        case 1:
            pose_out.x = pose_in.y;
            pose_out.y = length_side - pose_in.x - 1;
            break;
        case 2:
            pose_out.x = length_side - pose_in.x - 1;
            pose_out.y = length_side - pose_in.y - 1;
            break;
        case 3:
            pose_out.x = length_side - pose_in.y - 1;
            pose_out.y = pose_in.x;
            break;
        default:
            assert(false);
    }
    return pose_out;
}

template <typename T>
void circulate(T& w, T& x, T& y, T& z, int times = 1) {
    times = wrap(times, 4);
    switch (times) {
        case 0:
            break;
        case 1: {
            T tmp(move(w));
            w = move(x);
            x = move(y);
            y = move(z);
            z = move(tmp);
            break;
        }
        case 2: {
            swap(w, y);
            swap(x, z);
            break;
        }
        case 3: {
            T tmp(move(z));
            z = move(y);
            y = move(x);
            x = move(w);
            w = move(tmp);
            break;
        }
        default:
            assert(false);
    }
}

class Cube_map {

    //      ^ z
    //     /
    //    +-----> x
    //    |
    //    |
    //    v y

    public:

        struct Face_data {
            int x_offset;
            int y_offset;
            int rotation;
        };

        struct Turn {
            static Turn null() {
                return Turn{0, 0};
            }

            Turn inverse() const {
                return Turn{axis, -times};
            }

            char axis;
            int times;
        };

        const Face_data& get_face_data(Face face = Face::front) const {
            return cube_data[static_cast<int>(face)];
        }

        const char& at(int x, int y) const {
            const auto& face_data = get_face_data(Face::front);
            auto pose = rotate(Pose(x, y), -face_data.rotation, cube_side);
            int x_abs = face_data.x_offset + pose.x;
            int y_abs = face_data.y_offset + pose.y;
            return data[y_abs][x_abs];
        }

        char& at(int x, int y) {
            const auto* const_this = this;
            return const_cast<char&>(const_this->at(x, y));
        }

        void turn(const Turn& turn) {
            switch (turn.axis) {
                case 0:
                    break;
                case 'x':
                    turn_x(turn.times);
                    break;
                case 'y':
                    turn_y(turn.times);
                    break;
                case 'z':
                    turn_z(turn.times);
                    break;
                default:
                    assert(false);
            }
        }

        void forward(int steps) {
            while (steps) {
                int x = char_at.x;
                int y = char_at.y;
                Direction dir = char_at.dir;
                //at(x,y) = k_empty;
                at(x,y) = direction_symbols[static_cast<int>(dir)];
                auto[dx,dy] = direction_vectors[static_cast<int>(dir)];
                int x_next = x + dx;
                int y_next = y + dy;
                auto turn_cmd = Turn::null();
                if (x_next == cube_side) {
                    x_next = 0;
                    turn_cmd = Turn{'y', 1};
                }
                else if (x_next == -1) {
                    x_next = cube_side - 1;
                    turn_cmd = Turn{'y', -1};
                }
                else if (y_next == cube_side) {
                    y_next = 0;
                    turn_cmd = Turn{'x', -1};
                }
                else if (y_next == -1) {
                    y_next = cube_side - 1;
                    turn_cmd = Turn{'x', 1};
                }
                turn(turn_cmd);
                if (at(x_next, y_next) == k_wall) {
                    turn(turn_cmd.inverse());
                    steps = 0;
                }
                else {
                    char_at.x = x_next;
                    char_at.y = y_next;
                    turn_z(get_face_data().rotation);
                    --steps; 
                }
            }
            at(char_at.x, char_at.y) = k_character;
        }

        void turn_char(char turn_dir) {
            if (turn_dir == 'R') {
                char_at.dir = static_cast<Direction>(
                        wrap(static_cast<int>(char_at.dir) + 1, 4));
            }
            else if (turn_dir == 'L') {
                char_at.dir = static_cast<Direction>(
                        wrap(static_cast<int>(char_at.dir) - 1, 4));
            }
            else {
                assert(false);
            }
        }

        const vector<string>& flat_map() const {
            return data;
        }

        void read(istream& in) {
            cube_side = numeric_limits<int>::max();
            string line;
            while (getline(cin, line) && !line.empty()) {
                int x_start = line.find_first_not_of(k_eor);
                int x_end = line.find_last_not_of(k_eor);
                cube_side = min(cube_side, x_end - x_start + 1);
                data.push_back(move(line));
            }
            for (auto& face_data : cube_data) {
                face_data.x_offset = face_data.y_offset = -1;
            }
            int x_first_patch =  data[0].find_first_not_of(k_eor);
            read(x_first_patch, 0);

            // assumption: there will always be an open position in
            // the first face of the cube.
            for (int y = 0; y < cube_side; ++y) {
                for (int x = 0; x < cube_side; ++x) {
                    if (at(x, y) == k_empty) {
                        at(x, y) = k_character;
                        char_at = Pose(x, y, Direction::right);
                        return;
                    }
                }
            }
        }

        Pose char_at_abs() const {
            const auto& face_data = get_face_data();
            Pose pose_out = rotate(char_at, -face_data.rotation, cube_side);
            pose_out.x += face_data.x_offset;
            pose_out.y += face_data.y_offset;
            return pose_out;
        }

        bool in_range(int x, int y) const {
            return 0 <= y && y < (int)data.size() &&
                   0 <= x && x < (int)data[y].size() &&
                   data[y][x] != k_eor;
        }

    private:

        Face_data& get_face_data(Face face = Face::front) {
            return cube_data[static_cast<int>(face)];
        }

        void read(int x, int y) {
            auto& face_data = get_face_data();
            if (face_data.x_offset >= 0) {
                return;
            }

            cout << "Processing face at patch " << x << ' ' << y << endl;

            face_data.rotation = 0;
            face_data.x_offset = x;
            face_data.y_offset = y;

            if (in_range(x-cube_side,y)) {
                turn_y(-1);
                read(x-cube_side, y);
                turn_y(1);
            }
            if (in_range(x, y-cube_side)) {
                turn_x(1);
                read(x, y-cube_side);
                turn_x(-1);
            }
            if (in_range(x+cube_side, y)) {
                turn_y(1);
                read(x+cube_side, y);
                turn_y(-1);
            }
            if (in_range(x, y+cube_side)) {
                turn_x(-1);
                read(x, y+cube_side);
                turn_x(1);
            }
        }

        void turn_x(int times) {
            auto& front_face = get_face_data(Face::front);
            auto& back_face = get_face_data(Face::back);
            auto& left_face = get_face_data(Face::left);
            auto& right_face = get_face_data(Face::right);
            auto& top_face = get_face_data(Face::top);
            auto& bottom_face = get_face_data(Face::bottom);

            back_face.rotation = wrap(back_face.rotation + 2, 4);
            circulate(back_face,
                      bottom_face,
                      front_face,
                      top_face,
                      times);
            back_face.rotation = wrap(back_face.rotation + 2, 4);

            left_face.rotation = wrap(left_face.rotation - times, 4);
            right_face.rotation = wrap(right_face.rotation + times,4);
        }

        void turn_y(int times) {
            auto& front_face = get_face_data(Face::front);
            auto& back_face = get_face_data(Face::back);
            auto& left_face = get_face_data(Face::left);
            auto& right_face = get_face_data(Face::right);
            auto& top_face = get_face_data(Face::top);
            auto& bottom_face = get_face_data(Face::bottom);

            circulate(back_face,
                      left_face,
                      front_face,
                      right_face,
                      times);

            top_face.rotation = wrap(top_face.rotation - times, 4);
            bottom_face.rotation = wrap(bottom_face.rotation + times, 4);
        }

        void turn_z(int times) {
            auto& front_face = get_face_data(Face::front);
            auto& back_face = get_face_data(Face::back);
            auto& left_face = get_face_data(Face::left);
            auto& right_face = get_face_data(Face::right);
            auto& top_face = get_face_data(Face::top);
            auto& bottom_face = get_face_data(Face::bottom);

            circulate(top_face,
                      left_face,
                      bottom_face,
                      right_face,
                      times);

            front_face.rotation = wrap(front_face.rotation - times, 4);
            back_face.rotation = wrap(back_face.rotation + times, 4);
            left_face.rotation = wrap(left_face.rotation - times, 4);
            right_face.rotation = wrap(right_face.rotation - times, 4);
            top_face.rotation = wrap(top_face.rotation - times, 4);
            bottom_face.rotation = wrap(bottom_face.rotation - times, 4);
            char_at = rotate(char_at, -times, cube_side);
        }

        typedef array<Face_data, 6> Cube_data;

        vector<string> data;
        Cube_data cube_data;
        Pose char_at;
        int cube_side;
};

ostream& draw_face(ostream& out, const Cube_map& cube_map, Face face, int line) {
    int face_id = static_cast<int>(face);
    int face_rot = cube_map.get_face_data(face).rotation;
    if (line == 0) {
        out << "   ";
        if (face_rot == 1) {
            out << 'x';
        }
        else if (face_rot == 2) {
            out << 'y';
        }
        else {
            out << ' ';
        }
        out << "   ";
    }
    else if (line == 1) {
        out << "  ";
        if (face_rot == 2) {
            out << 'x';
        }
        else if (face_rot == 3) {
            out << 'y';
        }
        else {
            out << ' ';
        }
        out << face_id;
        if (face_rot == 0) {
            out << 'x';
        }
        else if (face_rot == 1) {
            out << 'y';
        }
        else {
            out << ' ';
        }
        out << "  ";
    }
    else if (line == 2) {
        out << "   ";
        if (face_rot == 0) {
            out << 'y';
        }
        else if (face_rot == 3) {
            out << 'x';
        }
        else {
            out << ' ';
        }
        out << "   ";
    }
    return out;
}

ostream& print_cube_status(ostream& out, const Cube_map& cube_map) {
    out << "        +-------+\n"
        << "        |"; draw_face(out, cube_map, Face::top, 0) << "|\n"
        << "        |"; draw_face(out, cube_map, Face::top, 1) << "|\n"
        << "        |"; draw_face(out, cube_map, Face::top, 2) << "|\n"
        << "+-------+-------+-------+-------+\n"
        << "|"; draw_face(out, cube_map, Face::left, 0) << "|";
                draw_face(out, cube_map, Face::front, 0) << "|";
                draw_face(out, cube_map, Face::right, 0) << "|";
                draw_face(out, cube_map, Face::back, 0) << "|\n"
        << "|"; draw_face(out, cube_map, Face::left, 1) << "|";
                draw_face(out, cube_map, Face::front, 1) << "|";
                draw_face(out, cube_map, Face::right, 1) << "|";
                draw_face(out, cube_map, Face::back, 1) << "|\n"
        << "|"; draw_face(out, cube_map, Face::left, 2) << "|";
                draw_face(out, cube_map, Face::front, 2) << "|";
                draw_face(out, cube_map, Face::right, 2) << "|";
                draw_face(out, cube_map, Face::back, 2) << "|\n"
        << "+-------+-------+-------+-------+\n"
        << "        |"; draw_face(out, cube_map, Face::bottom, 0) << "|\n"
        << "        |"; draw_face(out, cube_map, Face::bottom, 1) << "|\n"
        << "        |"; draw_face(out, cube_map, Face::bottom, 2) << "|\n"
        << "        +-------+\n";
    return out;
}

ostream& operator<<(ostream& out, const Cube_map& map) {
    const auto& flat_map = map.flat_map();
    for (const auto& row : flat_map) {
        out << row << '\n';
    }
    return out;
}

typedef variant<int,char> Cmd;

vector<Cmd> parse_commands(const string& line) {
    vector<Cmd> commands;
    size_t pos = 0;
    while (pos < line.length()) {
        if (line[pos] == 'R' || line[pos] == 'L') {
            commands.emplace_back(line[pos++]);
        }
        else {
            size_t next_pos = line.find_first_of("LR", pos);
            size_t count = next_pos - pos;
            commands.emplace_back(stoi(line.substr(pos, count)));
            pos = next_pos;
        }
    }
    return commands;
}

struct Cmd_exec {
    Cube_map& map;

    Cmd_exec(Cube_map& map) : map(map) {}

    void operator()(int steps) const {
        map.forward(steps);
    }

    void operator()(char turn_cmd) const {
        map.turn_char(turn_cmd);
    }
};

int main() {
    Cube_map map;
    map.read(cin);
    string instructions;
    cin >> instructions;

    Cmd_exec exec(map);
    for (auto&& cmd : parse_commands(instructions)) {
        visit(exec, cmd);
    }

    //map.forward(10);
    //map.turn('R');
    //map.forward(5);
    //map.turn('L');
    //map.forward(5);
    //map.turn('R');
    //map.forward(10);
    //map.turn('L');
    //map.forward(4);
    //map.turn('R');
    //map.forward(5);
    //map.turn('L');
    //map.forward(5);

    cout << map << endl;
    print_cube_status(cout, map) << endl;

    auto char_at = map.char_at_abs();

    int x = char_at.x;
    int y = char_at.y;
    int row = y + 1;
    int col = x + 1;
    int dir = static_cast<int>(char_at.dir);

    int ans = 1000*row + 4*col + dir;

    cout << ans << endl;
}
