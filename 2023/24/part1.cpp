#include <array>
#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
#include <optional>
#include <sstream>
#include <string>
using namespace std;

typedef array<double,3> Point3d;

constexpr double eps = 1e-16;
constexpr double x_min = 200'000'000'000'000;
constexpr double x_max = 400'000'000'000'000;
constexpr double y_min = 200'000'000'000'000;
constexpr double y_max = 400'000'000'000'000;

Point3d operator+(const Point3d& l, const Point3d& r) {
    return Point3d{l[0]+r[0], l[1]+r[1],l[2]+r[2]};
}

Point3d operator-(const Point3d& l, const Point3d& r) {
    return Point3d{l[0]-r[0], l[1]-r[1],l[2]-r[2]};
}

template<class Scalar>
Point3d operator*(const Point3d& p, Scalar s) {
    return Point3d{p[0]*s, p[1]*s, p[2]*s};
}

template<class Scalar>
Point3d operator*(Scalar s, const Point3d& p) {
    return Point3d{p[0]*s, p[1]*s, p[2]*s};
}

ostream& operator<<(ostream& out, const Point3d& point) {
    return out << '(' << point[0] << ',' << point[1] << ',' << point[2] << ')';
}

struct Semiline {
    Point3d begin;
    Point3d direction;
};


struct Trajectory {
    Point3d begin;
    Point3d velocity;

    optional<Point3d> intersection(const Trajectory& other) const {
        // Let's ignore the Z coordinate for now

        const Point3d& d1 = velocity;
        const Point3d& d2 = other.velocity;

        double dx = other.begin[0] - begin[0];
        double dy = other.begin[1] - begin[1];

        double den = d1[0]*d2[1] - d1[1]*d2[0];
        
        if (fabs(den) < eps) {
            return {};
        }

        double lambda = (dx*d2[1] - dy*d2[0])/den;
        double mu = (dx*d1[1] - dy*d1[0])/den;

        if (lambda<0 || mu<0) {
            return {};
        }

        return 0.5*((begin+lambda*d1) + (other.begin+mu*d2));
    }
};

vector<Trajectory> read_input(istream& in) {
    vector<Trajectory> input;
    string line;
    while (getline(cin, line)) {
        istringstream iss(line);
        string x, y, z, dummy, vx, vy, vz;
        iss >> x >> y >> z >> dummy >> vx >> vy >> vz;
        x.pop_back();
        y.pop_back();
        vx.pop_back();
        vy.pop_back();
        Point3d position{stod(x),stod(y),stod(z)};
        Point3d velocity{stod(vx),stod(vy),stod(vz)};
        input.push_back(Trajectory{position,velocity});
    }
    return input;
}

struct BoundingBox {
    Point3d min_coords;
    Point3d max_coords;

    bool contains(const Point3d& p) const {
        for (size_t i = 0; i < p.size(); ++i) {
            if (min_coords[i]>p[i] || p[i] > max_coords[i]) {
                return false;
            }
        }
        return true;
    }
};

size_t find_intersecting_pairs(const vector<Trajectory>& input) {
    constexpr double inf = numeric_limits<double>::infinity();
    BoundingBox bb{{x_min, y_min, -inf},
                   {x_max, y_max,  inf}};

    size_t ans = 0;
    for (size_t i = 0; i < input.size(); ++i) {
        for (size_t j = i+1; j < input.size(); ++j) {
            auto intersection = input[i].intersection(input[j]);
            ans += intersection.has_value() && bb.contains(*intersection);
        }
    }
    return ans;
}

int main() {
    auto input = read_input(cin);
    cout << find_intersecting_pairs(input) << endl;
}
