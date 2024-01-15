#include <cassert>
#include <iostream>
#include <stack>
#include <vector>
using namespace std;


struct StockSpanAlgorithm {
    typedef pair<int,int> Pair2i;
    stack<Pair2i> stk;
    int idx = 0;

    int operator()(int value) {
        while (!stk.empty() && value > stk.top().first) {
            stk.pop();
        }
        int stock_span = stk.empty()? (idx+1) : (idx - stk.top().second);
        stk.emplace(value, idx++);
        return stock_span;
    }
};


struct Matrix {
    vector<int> data;
    int width, height;

    Matrix() {}

    Matrix(int width, int height, int default_value = 0) :
        data(width*height, default_value),
        width(width),
        height(height) {}

    void read(istream& in) {
        string line;
        getline(in, line);
        width = in? line.length() : 0;
        height = 0;
        while (in) {
            assert((int)line.length() == width);
            for (char c : line)
                data.push_back(c-'0');
            ++height;
            getline(in, line);
        }
    }

    int operator()(int i, int j) const {
        return data[i*width+j];
    }

    int& operator()(int i, int j) {
        return data[i*width + j];
    }
};

ostream& operator<<(ostream& out, const Matrix& matrix) {
    for (int i = 0; i < matrix.height; ++i) {
        for (int j = 0; j < matrix.width; ++j) {
            if (j != 0) out << ' ';
            out << matrix(i, j);
        }
        out << '\n';
    }
    return out;
}

int main() {
    Matrix map;
    map.read(cin);

    int w = map.width;
    int h = map.height;

    Matrix acc_left(w, h);
    for (int i = 0; i < h; ++i) {
        StockSpanAlgorithm stock;
        for (int j = 0; j < w; ++j) {
            acc_left(i,j) = stock(map(i,j));
            acc_left(i,j) -= acc_left(i,j) == stock.idx;
        }
    }

    Matrix acc_right(w, h);
    for (int i = 0; i < h; ++i) {
        StockSpanAlgorithm stock;
        for (int j = w-1; j >= 0; --j) {
            acc_right(i,j) = stock(map(i,j));
            acc_right(i,j) -= acc_right(i,j) == stock.idx;
        }
    }

    Matrix acc_top(w, h);
    for (int j = 0; j < w; ++j) {
        StockSpanAlgorithm stock;
        for (int i = 0; i < h; ++i) {
            acc_top(i,j) = stock(map(i,j));
            acc_top(i,j) -= acc_top(i,j) == stock.idx;
        }
    }

    Matrix acc_bottom(w, h);
    for (int j = 0; j < w; ++j) {
        StockSpanAlgorithm stock;
        for (int i = h-1; i >= 0; --i) {
            acc_bottom(i,j) = stock(map(i,j));
            acc_bottom(i,j) -= acc_bottom(i,j) == stock.idx;
        }
    }

    int max_scenic_score = 0;
    for (int i = 1; i < h-1; ++i) {
        for (int j = 1; j < w-1; ++j) {
            int scenic_score =
                acc_left(i,j)*acc_right(i,j)*acc_top(i,j)*acc_bottom(i,j);
            max_scenic_score = max(max_scenic_score, scenic_score);
        }
    }
    cout << max_scenic_score << endl;

}
