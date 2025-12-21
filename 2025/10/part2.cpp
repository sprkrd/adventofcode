#include <bits/stdc++.h>
using namespace std;

typedef int64_t Int;
typedef uint64_t UInt;

vector<string> Split(const string& str, const string& delim=" \n\t") {
    vector<string> parts;
    size_t pos = 0;
    while (pos < str.length())
    {
        size_t pos_next = str.find_first_of(delim, pos);
        if (pos_next > pos)
        {
            parts.push_back(str.substr(pos, pos_next-pos));
        }
        pos = pos_next == string::npos? string::npos : pos_next + 1;
    }
    return parts;
}

struct Row
{
    UInt target;
    vector<UInt> buttons;
    vector<UInt> joltages;
};

UInt TargetToUInt(const string& target)
{
    UInt ans = 0;
    for (auto it = target.rbegin(); it < target.rend(); ++it)
    {
        ans = ans*2 + (*it=='#');
    }
    return ans;
}

UInt ButtonToUInt(const string& button)
{
    UInt ans = 0;
    for (const auto& wire : Split(button,","))
    {
        ans |= 1ULL << stoi(wire);
    }
    return ans;
}

vector<Row> Input()
{
    vector<Row> input;
    string line;
    while (getline(cin,line))
    {
        Row row;
        auto parts = Split(line);
        row.target = TargetToUInt(parts[0].substr(1,parts[0].size()-2));
        
        for (size_t i = 1; i < parts.size()-1; ++i)
        {
            row.buttons.push_back(ButtonToUInt(parts[i].substr(1,parts[i].size()-2)));
        }
        
        
        for (const auto& joltsStr : Split(parts.back().substr(1,parts.back().size()-2), ","))
        {
            row.joltages.push_back(stoull(joltsStr));
        }
        input.push_back(move(row));
    }
    return input;
}

constexpr double kErrorTolerance = 1e-18;
constexpr size_t kMaxNIterations = 10000;
mt19937_64 rng;

double Dot(const vector<double>& u, const vector<double>& v)
{
    double ans = 0;
    for (size_t i = 0; i < u.size(); ++i)
    {
        ans += u[i]*v[i];
    }
    return ans;
}

class LinearSolver
{
    public:
        explicit LinearSolver(vector<vector<double>> a, vector<double> b)
            : _a(move(a))
            , _b(move(b))
            , _x(_a[0].size())
        {
            for (size_t i = 0; i < _a.size(); ++i)
            {
                double normSq = 0;
                for (double c : _a[i])
                {
                    normSq += c*c;
                }
                double invNorm = 1.0 / sqrt(normSq);
                for (double& c : _a[i])
                {
                    c *= invNorm;
                }
                _b[i] *= invNorm;
            }
        }

        void Solve()
        {
            size_t nIterations = 0;
            vector<size_t> order(_a.size());
            iota(order.begin(), order.end(), 0ULL);
            while (TotalError()>kErrorTolerance && nIterations<kMaxNIterations)
            {
                ++nIterations;
                shuffle(order.begin(), order.end(), rng);
                for (size_t i : order)
                {
                    ProjectOntoPlane(i);
                }
            }
            cout << nIterations << " iteration(s), error = " << TotalError() << endl;
        }

        const vector<double>& GetSolution() const
        {
            return _x;
        }

    private:
        double TotalError() const
        {
            double totalError = 0;
            for (size_t i = 0; i < _a.size(); ++i)
            {
                totalError += fabs(Error(i));
            }
            return totalError;
        }

        double Error(size_t i) const
        {
            return Dot(_a[i], _x) - _b[i];
        }


        void ProjectOntoPlane(size_t i)
        {
            double error = Error(i);
            for (size_t j = 0; j < _x.size(); ++j)
            {
                _x[j] -= error*_a[i][j];
            }
        }

        vector<vector<double>> _a;
        vector<double> _b;
        vector<double> _x;
};

class Solver
{
    public:

        explicit Solver(const Row& row)
            : _a(row.joltages.size(), vector<double>(row.buttons.size()))
            , _b(row.joltages.size())
            , _x(row.buttons.size())
        {
            for (size_t i = 0; i < row.joltages.size(); ++i)
            {
                for (size_t j = 0; j < row.buttons.size(); ++j)
                {
                    UInt button = row.buttons[j];
                    if ((button>>i)&1)
                    {
                        _a[i][j] = 1.0;
                    }
                }
                _b[i] = row.joltages[i];
            }
        }

        void Solve()
        {
            //LinearSolver linearSolver(_a, _b);
            //linearSolver.Solve();
            //_x = linearSolver.GetSolution();

            vector<vector<double>> lambda(_a.size(), vector<double>(_a.size()));
            vector<double> gamma(_a.size());
            for (size_t i = 0; i < _a.size(); ++i)
            {
                lambda[i][i] = Dot(_a[i], _a[i]);
                for (size_t j = i+1; j < _a.size(); ++j)
                {
                    lambda[i][j] = lambda[j][i] = Dot(_a[i], _a[j]);
                }
                gamma[i] = -accumulate(_a[i].begin(), _a[i].end(), 0.0);
            }

            LinearSolver linearSolver2(lambda, gamma);
            linearSolver2.Solve();

            vector<double> u(_a[0].size(), -1.0);
            for (size_t i = 0; i < _a.size(); ++i)
            {
                double l = linearSolver2.GetSolution()[i];
                for (size_t j = 0; j < _a[0].size(); ++j)
                {
                    u[j] -= l*_a[i][j];
                }
            }

            for (double x : u)
            {
                cout << x << ",";
            }
            cout << endl;
        }

        const vector<double>& GetSolution() const
        {
            return _x;
        }

    private:

        vector<vector<double>> _a;
        vector<double> _b;
        vector<double> _x;
};

int main()
{
    rng.seed(random_device{}());

    auto input = Input();
    
    for (const auto& row : input)
    {
        Solver solver(row);
        solver.Solve();
        const auto& x = solver.GetSolution();
        cout << '[';
        for (size_t i = 0; i < x.size(); ++i)
        {
            if (i > 0) cout << ',';
            cout << x[i];
        }
        cout << ']' << endl;
    }
}
