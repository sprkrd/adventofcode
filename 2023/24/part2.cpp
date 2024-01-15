/*
H1: P1 + V1*t
H2: P2 + V2*t
.
.
.
HN: PN + VN*t

R: PR + VR*t

n+6 vars
3n equations


P1 + V1*t1 = PR + VR*t1
P2 + V2*t2 = PR + VR*t2

p1x+v1x*t1 = prx + vrx*t1 -> (v1x-vrx)*t1 = prx - p1x
p1y+v1y*t1 = pry + vry*t1 -> (v1y-vry)*t1 = pry - p1y
p1z+v1z*t1 = prz + vrz*t1 -> (v1z-vrz)*t1 = prz - p1z


prx - p1x   pry - p1y
--------- = --------- -> (prx - p1x) * (v1y - vry) = (pry - p1y) * (v1x - vrx) ->
v1x - vrx   v1y - vry

prx*vny - prx*vry - pnx*vny + pnx*vry = pry*vnx - pry*vrx - pny*vnx + pny*vrx
prx*vnz - prx*vrz - pnx*vnz + pnx*vrz = prz*vnx - prz*vrx - pnz*vnx + pnz*vrx


prx*v1y - prx*vry - p1x*v1y + p1x*vry = pry*v1x - pry*vrx - p1y*v1x + p1y*vrx
prx*v2y - prx*vry - p2x*v2y + p2x*vry = pry*v2x - pry*vrx - p2y*v2x + p2y*vrx

(v1y-v2y)*prx - (p1x*v1y-p2x*v2y) + (p1x-p2x)*vry = (v1x-v2x)*pry - (p1y*v1x - p2y*v2x) + (p1y-p2y)*vrx
(v1y-v2y)*prx - (v1x-v2x)*pry - (p1y-p2y)*vrx + (p1x-p2x)*vry  = (p1x*v1y-p2x*v2y) - (p1y*v1x-p2y*v2x)

prx*v1y - prx*v2y - p1x*v1y + p2x*v2y + p1x*vry - p2x*vry = pry*v1x - pry*v2x - p1y*v1x + p2y*v2x + p1y*vrx - p2y*vrx
(v1y-v2y)*prx + (p1x-p2x)*vry - (p1x*v1y-p2x*v2y) = (v1x-v2x)*pry + (p1y-p2y)*vrx - (p1y*v1x-p2y*v2x)

(v1y-v2y)*prx - (v1x-v2x)*pry - (p1y-p2y)*vrx + (p1x-p2x)*vry  = (p1x*v1y-p2x*v2y) - (p1y*v1x-p2y*v2x)
(v1z-v2z)*prx - (v1x-v2x)*prz - (p1z-p2z)*vrx + (p1x-p2x)*vrz  = (p1x*v1z-p2x*v2z) - (p1z*v1x-p2z*v2x)

*/


#include <array>
#include <cassert>
#include <cmath>
#include <iostream>
#include <gmpxx.h>
#include <limits>
#include <vector>
#include <optional>
#include <sstream>
#include <string>
#include <valarray>
using namespace std;

typedef array<int64_t,3> Point3ll;

struct Trajectory {
    Point3ll begin;
    Point3ll velocity;
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
        Point3ll position{stoll(x),stoll(y),stoll(z)};
        Point3ll velocity{stoll(vx),stoll(vy),stoll(vz)};
        input.push_back(Trajectory{position,velocity});
    }
    return input;
}

template<class Scalar>
struct Matrix {
    size_t nrows;
    size_t ncols;
    valarray<Scalar> data;

    Matrix(size_t nrows, size_t ncols, Scalar default_val = Scalar(0)) :
        nrows(nrows), ncols(ncols), data(default_val, nrows*ncols) {}

    Matrix(size_t ncols, valarray<Scalar> data) :
        nrows(data.size()/ncols), ncols(ncols), data(move(data)) {}

    Scalar at(size_t i, size_t j) const {
        return data[i*ncols + j];
    }

    Scalar& at(size_t i, size_t j) {
        return data[i*ncols + j];
    }

    Matrix<bool> operator==(const Matrix<Scalar>& other) const {
        return Matrix<bool>(ncols, data==other.data);
    }

    Scalar determinant() const {
        assert(nrows == ncols);
        size_t N = nrows;
        Scalar det = 0;

        if (N == 1) {
            return data[0];
        }
        else if (N == 2) {
            return at(0,0)*at(1,1) - at(0,1)*at(1,0);
        }

        for (size_t j = 0; j < N; ++j) {
            Matrix m0j(N-1,N-1);
            for (size_t i_ = 0; i_ < N-1; ++i_) {
                for (size_t j_ = 0; j_ < N-1; ++j_) {
                    m0j.at(i_,j_) = at(1+i_,j_+(j_>=j));
                }
            }
            if (j%2) {
                det -= at(0,j)*m0j.determinant();
            }
            else {
                det += at(0,j)*m0j.determinant();
            }
        }
        return det;
    }

    template<class Collection>
    Matrix<Scalar> replace_column(size_t j, const Collection& data) {
        Matrix<Scalar> result = *this;
        size_t i = 0;
        for (auto x : data) {
            result.at(i,j) = x;
            ++i;
        }
        return result;
    }
};

int main() {
    auto input = read_input(cin);

    Matrix<mpz_class> A(6,6);
    vector<mpz_class> b(6);
 
    for (size_t i = 1; i <= 3; ++i) {
        auto p1 = input[0].begin;
        auto p2 = input[i].begin;
        auto v1 = input[0].velocity;
        auto v2 = input[i].velocity;

        size_t eq1 = 2*(i-1);
        size_t eq2 = 2*(i-1) + 1;

        A.at(eq1,0) = v1[1]-v2[1];
        A.at(eq1,1) = -(v1[0]-v2[0]);
        A.at(eq1,3) = -(p1[1]-p2[1]);
        A.at(eq1,4) = p1[0]-p2[0];
        b[eq1] = p1[0]*v1[1] + p2[1]*v2[0] - p2[0]*v2[1] - p1[1]*v1[0];

        A.at(eq2,0) = v1[2]-v2[2];
        A.at(eq2,2) = -(v1[0]-v2[0]);
        A.at(eq2,3) = -(p1[2]-p2[2]);
        A.at(eq2,5) = p1[0]-p2[0];
        b[eq2] = p1[0]*v1[2] + p2[2]*v2[0] - p2[0]*v2[2] - p1[2]*v1[0];
    }
    mpz_class det_A = A.determinant();

    mpz_class x = A.replace_column(0, b).determinant() / det_A;
    mpz_class y = A.replace_column(1, b).determinant() / det_A;
    mpz_class z = A.replace_column(2, b).determinant() / det_A;

    mpz_class sum = x + y + z;
    cout << sum << endl;
}
