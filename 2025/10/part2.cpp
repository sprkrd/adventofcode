#include <bits/stdc++.h>
using namespace std;

typedef int64_t Int;
typedef uint64_t UInt;

constexpr double kErrorTolerance = 1e-9;

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

bool InRange(const vector<double>& x, const vector<double>& lo, const vector<double>& hi)
{
    for (size_t i = 0; i < x.size(); ++i)
    {
        if (x[i] < lo[i]-kErrorTolerance || x[i] > hi[i]+kErrorTolerance)
        {
            return false;
        }
    }
    return true;
}

// linear algebra

ostream& operator<<(ostream& out, const vector<double>& v)
{
    out << '[';
    if (!v.empty())
    {
        out << v[0];
        for (size_t i = 1; i < v.size(); ++i) out << ',' << v[i];
    }
    return out << ']';
}

bool IsInteger(double x)
{
    return fabs(x-round(x)) < kErrorTolerance;
}

class System
{
    public:

        System(vector<vector<double>> a, vector<double> b)
            : _a(move(a))
            , _b(move(b))
        {
        }

        bool Solve()
        {
            ReducedRowEchelon();
            for (size_t i = 0; i < _a.size(); ++i)
            {
                bool allZeroes = all_of(_a[i].begin(), _a[i].end(),
                        [](double x) {return fabs(x) < kErrorTolerance; });
                if (allZeroes && fabs(_b[i] >= kErrorTolerance))
                {
                    return false;
                }
            }

            _x.resize(_a[0].size());

            for (size_t i = 0; i < _pivots.size(); ++i)
            {
                _x[_pivots[i]] = _b[i];
            }

            _nullSpace.clear();
            for (size_t freeVar : _freeVariables)
            {
                vector<double> nullVector(_a[0].size());
                nullVector[freeVar] = 1.0;
                for (size_t i = 0; i < _pivots.size(); ++i)
                {
                    nullVector[_pivots[i]] = -_a[i][freeVar];
                }
                _nullSpace.push_back(move(nullVector));
            }

            return true;
        }

        const vector<double>& GetSolution() const
        {
            return _x;
        }

        const vector<vector<double>>& GetNullSpace() const
        {
            return _nullSpace;
        }

        size_t DegreesOfFreedom() const
        {
            return _nullSpace.size();
        }

    private:

        void NormalizeRow(size_t row, size_t pivot)
        {
            double f = 1.0 / _a[row][pivot];
            _a[row][pivot] = 1.0;
            for (size_t i = pivot+1; i < _a[row].size(); ++i)
            {
                _a[row][i] *= f;
            }
            _b[row] *= f;
        }

        void Reduce(size_t row, size_t pivot)
        {
            NormalizeRow(row, pivot);
            for (size_t i = 0; i < _a.size(); ++i)
            {
                if (i==row) continue;
                double f = _a[i][pivot];
                _a[i][pivot] = 0;
                for (size_t j = pivot+1; j < _a[i].size(); ++j)
                {
                    _a[i][j] -= _a[row][j]*f;
                    
                }
                _b[i] -= _b[row]*f;
            }
        }

        void ReducedRowEchelon()
        {
            _pivots.clear();
            _freeVariables.clear();
            size_t currentRow = 0;
            size_t currentPivot = 0;
            while (currentRow < _a.size() && currentPivot < _a[0].size())
            {
                bool isFreeVariable = true;
                for (size_t i = currentRow; i < _a.size(); ++i)
                {
                    if (fabs(_a[i][currentPivot]) >= kErrorTolerance)
                    {
                        isFreeVariable = false;
                        _pivots.push_back(currentPivot);
                        swap(_a[currentRow], _a[i]);
                        swap(_b[currentRow], _b[i]);
                        Reduce(currentRow, currentPivot);
                        ++currentRow;
                        break;
                    }
                }
                if (isFreeVariable)
                {
                    _freeVariables.push_back(currentPivot);
                }
                ++currentPivot;
            }
            while (currentPivot < _a[0].size())
            {
                _freeVariables.push_back(currentPivot++);
            }
        }

        vector<size_t> _pivots;
        vector<size_t> _freeVariables;

        vector<vector<double>> _a;
        vector<double> _b;

        vector<double> _x;
        vector<vector<double>> _nullSpace;
};

class LPSolver
{
    public:

        LPSolver(vector<vector<double>> a, vector<double> b, const vector<double>& lowerBounds, const vector<double>& upperBounds)
            : _a(move(a))
            , _b(move(b))
            , _lowerBounds(lowerBounds)
            , _upperBounds(upperBounds)
            , _x(_a[0].size())
            , _nVertices(0)
            , _minObj(numeric_limits<double>::infinity())
        {
        }

        bool Solve()
        {
            System system(_a, _b);
            if (!system.Solve())
            {
                return false;
            }

            if (system.DegreesOfFreedom() == 0)
            {
                _x = system.GetSolution();
                _minObj = accumulate(_x.begin(), _x.end(), 0.0);
                return InRange(_x, _lowerBounds, _upperBounds);
            }

            return Solve(_x.size(), system.DegreesOfFreedom());
        }

        const vector<double>& GetSolution() const
        {
            return _x;
        }

        double Obj() const
        {
            return _minObj; //accumulate(_x.begin(), _x.end(), 0.0);
        }

    private:

        bool Solve(size_t i, size_t dof)
        {
            if (dof == 0)
            {
                ++_nVertices;
                System system(_a, _b);
                bool okSolution = system.Solve() &&
                    system.DegreesOfFreedom() == 0 &&
                    InRange(system.GetSolution(), _lowerBounds, _upperBounds);
                if (!okSolution)
                {
                    return false;
                }
                const auto& x = system.GetSolution();
                double obj = accumulate(x.begin(), x.end(), 0.0);
                if (obj < _minObj)
                {
                    _x = x;
                    _minObj = obj;
                }
                return true;
            }

            bool solutionFound = false;
            if (i > dof)
            {
                solutionFound = Solve(i-1, dof);
            }
            vector<double> constraint(_a[0].size());
            constraint[i-1] = 1.0;

            _a.push_back(move(constraint));
            _b.push_back(_lowerBounds[i-1]);
            solutionFound = Solve(i-1, dof-1) || solutionFound;

            if (_upperBounds[i-1] < numeric_limits<double>::infinity())
            {
                _b.back() = _upperBounds[i-1];
                solutionFound = Solve(i-1, dof-1) || solutionFound;
            }

            _a.pop_back();
            _b.pop_back();

            return solutionFound;
        }

        vector<vector<double>> _a;
        vector<double> _b;
        const vector<double>& _lowerBounds;
        const vector<double>& _upperBounds;
        vector<double> _x;
        size_t _nVertices;
        double _minObj;

};

class IPSolver
{
    public:

        explicit IPSolver(const Row& row)
            : _a(row.joltages.size(), vector<double>(row.buttons.size()))
            , _b(row.joltages.size())
            , _x(row.buttons.size())
            , _upperBound(numeric_limits<double>::infinity())
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
                _b[i] = row.joltages[i];
            }
        }

        bool Solve()
        {
            return SolveR();
        }

        const vector<double>& GetSolution() const
        {
            return _x;
        }

        unsigned Obj() const
        {
            return static_cast<unsigned>(round(_upperBound));
        }

    private:

        bool SolveR()
        {
            LPSolver lpSolver(_a, _b, _xLowBounds, _xHighBounds);
            if (!lpSolver.Solve() || lpSolver.Obj() >= _upperBound)
            {
                return false;
            }
            const auto& x = lpSolver.GetSolution();
            double obj = accumulate(x.begin(), x.end(), 0.0);
            auto it = find_if_not(x.begin(), x.end(), IsInteger);
            if (it == x.end())
            {
                if (obj < _upperBound)
                {
                    _upperBound = obj;
                    _x = x;
                }
                return true;
            }

            bool found = false;
            size_t i = it - x.begin();

            double prevLowBound = _xLowBounds[i];
            _xLowBounds[i] = ceil(x[i]);
            found = SolveR() || found;
            _xLowBounds[i] = prevLowBound;

            double prevUpperBound = _xHighBounds[i];
            _xHighBounds[i] = floor(x[i]);
            found = SolveR() || found;
            _xHighBounds[i] = prevUpperBound;

            return found;
        }

        vector<vector<double>> _a;
        vector<double> _b;
        vector<double> _x;
        double _upperBound;
        vector<double> _xLowBounds;
        vector<double> _xHighBounds;
};

int main()
{
    auto input = Input();
    unsigned answer = 0;    
    for (const auto& row : input)
    {
        IPSolver solver(row);
        assert(solver.Solve());
        answer += solver.Obj();
        //const auto& x = solver.GetSolution();
        //cout << '[';
        //for (size_t i = 0; i < x.size(); ++i)
        //{
            //if (i > 0) cout << ',';
            //cout << x[i];
        //}
        //cout << ']' << endl;
    }
    cout << answer << endl;
}
