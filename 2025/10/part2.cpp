#include <bits/stdc++.h>
using namespace std;

typedef int64_t Int;
typedef uint64_t UInt;

constexpr double kErrorTolerance = 1e-5;
constexpr UInt kMaxNIterations = 5000;
mt19937_64 rng;

// input handling

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

bool IsBetween(double x, double lo, double hi)
{
    return lo-kErrorTolerance <= x && x <= hi+kErrorTolerance;
}

// linear algebra

ostream& operator<<(ostream& out, const vector<double>& v)
{
    out << '[';
    out << v[0];
    for (size_t i = 1; i < v.size(); ++i) out << ',' << v[i];
    return out << ']';
}

vector<double> operator+(const vector<double>& v, const vector<double>& u)
{
    vector<double> result(v.size());
    for (size_t i = 0; i < v.size(); ++i)
    {
        result[i] = u[i] + v[i];
    }
    return result;
}

vector<double>& operator+=(vector<double>& u, const vector<double>& v)
{
    for (size_t i = 0; i < v.size(); ++i)
    {
        u[i] += v[i];
    }
    return u;
}

vector<double> operator-(const vector<double>& v, const vector<double>& u)
{
    vector<double> result(v.size());
    for (size_t i = 0; i < v.size(); ++i)
    {
        result[i] = u[i] - v[i];
    }
    return result;
}

vector<double>& operator-=(vector<double>& u, const vector<double>& v)
{
    for (size_t i = 0; i < v.size(); ++i)
    {
        u[i] -= v[i];
    }
    return u;
}

vector<double> operator*(double f, const vector<double>& u)
{
    vector<double> result(u.size());
    for (size_t i = 0; i < u.size(); ++i)
    {
        result[i] = f*u[i];
    }
    return result;
}

vector<double> operator*(const vector<double>& u, double f)
{
    return f*u;
}

vector<double>& operator*=(vector<double>& u, double f)
{
    for (double& x : u)
    {
        x *= f;
    }
    return u;
}

double SumAbs(const vector<double>& u)
{
    return accumulate(u.begin(), u.end(), 0.0, [](double acc, double x){ return acc + fabs(x);});
}

double Dot(const vector<double>& u, const vector<double>& v)
{
    double ans = 0;
    for (size_t i = 0; i < u.size(); ++i)
    {
        ans += u[i]*v[i];
    }
    return ans;
}

double SquaredNorm(const vector<double>& u)
{
    return Dot(u,u);
}

double Norm(const vector<double>& u)
{
    return sqrt(SquaredNorm(u));
}

void Normalize(vector<double>& u)
{
    double invNorm = 1.0 / Norm(u);
    u *= invNorm;
}

void RemoveProjection(vector<double>& u, const vector<double>& v)
{
    double normSq = SquaredNorm(v);
    double dot = Dot(u, v);
    double f = dot/normSq;
    u -= f*v;
}

void NormalizeRow(vector<vector<double>>& a, vector<double>& b, size_t row, size_t pivot)
{
    double f = 1.0 / a[row][pivot];
    a[row][pivot] = 1.0;
    for (size_t i = pivot+1; i < a[row].size(); ++i)
    {
        a[row][i] *= f;
    }
    b[row] *= f;
}

void Reduce(vector<vector<double>>& a, vector<double>& b, size_t row, size_t pivot)
{
    NormalizeRow(a, b, row, pivot);
    for (size_t i = 0; i < a.size(); ++i)
    {
        if (i==row) continue;
        double f = a[i][pivot];
        a[i][pivot] = 0;
        for (size_t j = pivot+1; j < a[i].size(); ++j)
        {
            a[i][j] -= a[row][j]*f;
        }
        b[i] -= b[row]*f;
    }
}

void ReducedRowEchelon(vector<vector<double>>& a, vector<double>& b)
{
    size_t currentRow = 0;
    size_t currentPivot = 0;
    while (currentRow < a.size() && currentPivot < a[0].size())
    {
        for (size_t i = currentRow; i < a.size(); ++i)
        {
            if (fabs(a[i][currentPivot]) >= kErrorTolerance)
            {
                swap(a[currentRow], a[i]);
                swap(b[currentRow], b[i]);
                Reduce(a, b, currentRow, currentPivot);
                ++currentRow;
                break;
            }
        }
        ++currentPivot;
    }
}

struct SystemSolution
{
    vector<double> basicSolution;
    vector<vector<double>> nullSpace;
};

pair<vector<size_t>,vector<size_t>> GetPivotsAndFreeVariables(const vector<vector<double>>& a, const vector<double>& b)
{
    vector<size_t> pivots;
    vector<size_t> freeVariables;
    size_t currentRow = 0;
    size_t currentPivot = 0;
    while (currentRow < a.size() && currentPivot < a[0].size())
    {
        bool isFreeVariable = true;
        for (size_t i = currentRow; i < a.size(); ++i)
        {
            if (fabs(a[i][currentPivot] - 1.0) < kErrorTolerance)
            {
                isFreeVariable = false;
                pivots.push_back(currentPivot);
                ++currentRow;
                break;
            }
        }
        if (isFreeVariable)
        {
            freeVariables.push_back(currentPivot);
        }
        ++currentPivot;
    }
    while (currentPivot < a[0].size())
    {
        freeVariables.push_back(currentPivot++);
    }
    return {pivots, freeVariables};
}

void GramSchmidt(vector<vector<double>>& vectorSpace)
{
    vector<vector<double>> orthonormalSpace;
    for (size_t i = 0; i < vectorSpace.size(); ++i)
    {
        vector<double> u = vectorSpace[i];
        for (size_t j = 0; j < i; ++j)
        {
            RemoveProjection(u, orthonormalSpace[j]);
        }
        Normalize(u);
        orthonormalSpace.push_back(move(u));
    }
    vectorSpace = move(orthonormalSpace);
}

SystemSolution LinSolve(vector<vector<double>> a, vector<double>b)
{
    SystemSolution solution;
    solution.basicSolution.resize(a[0].size());
    ReducedRowEchelon(a, b);
    auto[pivots, freeVariables] = GetPivotsAndFreeVariables(a, b);
    for (size_t i = 0; i < pivots.size(); ++i)
    {
        solution.basicSolution[pivots[i]] = b[i];
    }
    for (size_t freeVar : freeVariables)
    {
        vector<double> nullVector(a[0].size());
        nullVector[freeVar] = 1.0;
        for (size_t i = 0; i < pivots.size(); ++i)
        {
            nullVector[pivots[i]] = -a[i][freeVar];
        }
        solution.nullSpace.push_back(move(nullVector));
    }
    return solution;
}

class LPSolver
{
    public:

        explicit LPSolver(const Row& row)
            : _a(row.joltages.size(), vector<double>(row.buttons.size()))
            , _b(row.joltages.size())
            , _x(row.buttons.size())
            , _xLowBounds(row.buttons.size(), 0)
            , _xHighBounds(row.buttons.size(), numeric_limits<double>::infinity())
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
                // double invNorm = 1.0 / Norm(_a[i]);
                // _a[i] *= invNorm;
                _b[i] = row.joltages[i]; //*invNorm;
            }
            Init();
        }

        void Solve()
        {   
            size_t nIterations = 0;
            while (nIterations<kMaxNIterations && CheckConstraints())
            {
                ++nIterations;
            }
            if (nIterations == 5000)
            {
                bool first = true;
                for (const auto& nullVector : _systemSolution.nullSpace)
                {
                    if (!first) cout << ", ";
                    cout << nullVector;
                    first = false;
                }
                cout << endl;
            }
            cout << nIterations << " iteration(s), error = " << TotalError() << endl;
        }

        const vector<double>& GetSolution() const
        {
            return _x;
        }

    private:
    
        bool CheckConstraints()
        {
            for (size_t var = 0; var < _x.size(); ++var)
            {
                if (_x[var] < _xLowBounds[var]-kErrorTolerance)
                {
                    ReprojectIntoFeasibleRegion(var, _xLowBounds[var]-_x[var]);
                    return true;
                }
                else if (_x[var] > _xHighBounds[var]+kErrorTolerance)
                {
                    ReprojectIntoFeasibleRegion(var, _xHighBounds[var]-_x[var]);
                    return true;
                }
            }
            return false;
        }
        
        void ReprojectIntoFeasibleRegion(size_t var, double errorMagnitude)
        {
            vector<double> error(_x.size());
            error[var] = errorMagnitude;
            vector<double> errorNullProjection = ProjectOntoNullSpace(error);
            assert(fabs(errorNullProjection[var]) > 0);
            double lambda = errorMagnitude/errorNullProjection[var];
            assert(lambda < 1e18);
            _x += lambda*errorNullProjection;
        }
    
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
        
        vector<double> ProjectOntoNullSpace(const vector<double>& v)
        {
            vector<double> projection(_x.size());
            for (const auto& nullVector : _systemSolution.nullSpace)
            {
                double f = Dot(v, nullVector);
                for (size_t i = 0; i < projection.size(); ++i)
                {
                    projection[i] += f*nullVector[i];
                }
            }
            return projection;
        }
        
        void Init()
        {
            _systemSolution = LinSolve(_a, _b);
            _x = _systemSolution.basicSolution;
            GramSchmidt(_systemSolution.nullSpace);
        }

        vector<vector<double>> _a;
        vector<double> _b;
        vector<double> _x;
        SystemSolution _systemSolution;
        vector<double> _xLowBounds;
        vector<double> _xHighBounds;
};

int main()
{
    // vector<vector<double>> A{
        // {-3,2,-1,-1},
        // {6,-6,7,-7},
        // {3,-4,4,-6}
    // };
    // vector<double> b{1,2,3};
    // vector<vector<double>> A{
        // {1,3,3,8,5},
        // {0,1,3,10,8},
        // {0,0,0,-1,-4},
        // {0,0,0,2,8}
    // };
    // vector<double> b{1,2,3,4};
    // vector<vector<double>> A{
        // {0,1,1,0},
        // {1,0,0,1},
        // {1,-1,0,1},
        // {0,0,-1,-1}
    // };
    // vector<double> b{1,2,3,4};

    // auto solution = LinSolve(A, b);

    // cout << "Basic solution: " << solution.basicSolution << endl;
    // cout << "Null space: <";
    // bool first = true;
    // for (const auto& nullVector : solution.nullSpace)
    // {
        // if (!first) cout << ", ";
        // cout << nullVector;
        // first = false;
    // }
    // cout << '>' << endl;
    // GramSchmidt(solution.nullSpace);
    // cout << "Null space (orthonormal basis): <";
    // first = true;
    // for (const auto& nullVector : solution.nullSpace)
    // {
        // if (!first) cout << ", ";
        // cout << nullVector;
        // first = false;
    // }
    // cout << '>' << endl;


    // ReducedRowEchelon(A, b);
    // for (const auto& row : A)
    // {
        // for (auto column : row)
        // {
            // cout << column << '\t';
        // }
        // cout << endl;
    // }

    // cout << endl;
    // for (double row : b)
    // {
        // cout << row << '\t';
    // }
    // cout << endl;
    rng.seed(random_device{}());

    auto input = Input();
    
    for (const auto& row : input)
    {
        LPSolver solver(row);
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
