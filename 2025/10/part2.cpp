#include <bits/stdc++.h>
using namespace std;

typedef int64_t Int;
typedef uint64_t UInt;

constexpr double kErrorTolerance = 1e-12;
constexpr double kInf = numeric_limits<double>::infinity();

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

struct Constraint
{
    enum Type { Equality, LessOrEqual, GreaterOrEqual };
    Type type;
    std::vector<double> left;
    double right;
};

enum class LPResult { Ok, Infeasible, Unbound };

ostream& operator<<(ostream& out, LPResult result)
{
    switch (result)
    {
        case LPResult::Ok:
            out << "Ok";
            break;
        case LPResult::Infeasible:
            out << "Infeasible";
            break;
        case LPResult::Unbound:
            out << "Unbound";
            break;
    }
    return out;
}

class LPSolver
{
    public:

        LPSolver()
            : _nProblemVariables(0)
            , _nSlackVariables(0)
            , _nSurplusVariables(0)
            , _nArtificialVariables(0)
        {
        }

        LPResult Solve()
        {
            SetUpTableau();
            auto result = Phase1();
            if (result != LPResult::Ok)
            {
                return result;
            }
            auto ret = Phase2();
            return ret;
        }

        void AddConstraint(Constraint constraint)
        {
            if (_nProblemVariables == 0)
            {
                _nProblemVariables = constraint.left.size();
            }
            else
            {
                assert(constraint.left.size() == _nProblemVariables);
            }
            switch (constraint.type)
            {
                case Constraint::Equality:
                    ++_nArtificialVariables;
                    break;
                case Constraint::LessOrEqual:
                    ++_nSlackVariables;
                    break;
                case Constraint::GreaterOrEqual:
                    ++_nSurplusVariables;
                    ++_nArtificialVariables;
                    break;
            }
            _constraints.push_back(move(constraint)); 
        }

        void AddBoundConstraint(size_t var, double bound, Constraint::Type type)
        {
            Constraint constraint;
            constraint.left.resize(_nProblemVariables);
            constraint.left[var] = 1.0;
            constraint.right = bound;
            constraint.type = type;
            AddConstraint(move(constraint));

        }

        void AddLowerBoundConstraint(size_t var, double lowerBound)
        {
            AddBoundConstraint(var, lowerBound, Constraint::GreaterOrEqual);
        }

        void AddUpperBoundConstraint(size_t var, double upperBound)
        {
            AddBoundConstraint(var, upperBound, Constraint::LessOrEqual);
        }

        void PopConstraint()
        {
            switch (_constraints.back().type)
            {
                case Constraint::Equality:
                    --_nArtificialVariables;
                    break;
                case Constraint::LessOrEqual:
                    --_nSlackVariables;
                    break;
                case Constraint::GreaterOrEqual:
                    --_nSurplusVariables;
                    --_nArtificialVariables;
                    break;
            }
            _constraints.pop_back();
        }

        void SetObjective(vector<double> objective)
        {
            if (_nProblemVariables == 0)
            {
                _nProblemVariables = objective.size();
            }
            else
            {
                assert(_nProblemVariables == objective.size());
            }
            _objective = move(objective);
        }

        void ShowTableau(ostream& out) const
        {
            constexpr size_t cellWidth = 8;
            out << right << setw(cellWidth) << "basic";
            for (size_t i = 0; i < _tableau[0].size()-1; ++i)
            {
                string varname = "x" + to_string(i);
                out << setw(cellWidth) << right << varname;
            }
            out << right << setw(cellWidth) << "RHS" << '\n';
            out.precision(2);
            out.setf(ios::fixed);
            for (size_t i = 0; i < _basicVariables.size(); ++i)
            {
                string varname = "x" + to_string(_basicVariables[i]);
                out << right << setw(cellWidth) << varname;
                for (double x : _tableau[i])
                {
                    out << right <<setw(cellWidth) << x;
                }
                out << '\n';;
            }
            out << right << setw(cellWidth) << "z";
            for (double x : _tableau.back())
            {
                out << right << setw(cellWidth) << x;
            }
            out << '\n';
        }

        vector<double> GetSolution() const
        {
            vector<double> solution(_nProblemVariables);
            for (size_t i = 0; i < _tableau.size()-1; ++i)
            {
                if (_basicVariables[i] < _nProblemVariables)
                {
                    solution[_basicVariables[i]] = _tableau[i].back();
                }
            }
            return solution;
        }

        double Objective() const
        {
            return _tableau.back().back();
        }

    private:

        void NormalizeRow(size_t row, size_t pivot)
        {
            double scale = 1.0 / _tableau[row][pivot];
            for (size_t j = 0; j < _tableau[row].size(); ++j)
            {
                _tableau[row][j] *= scale;
            }
        }

        void Reduce(size_t src, size_t dst, size_t pivot)
        {
            double f = - _tableau[dst][pivot];
            for (size_t j = 0; j < _tableau[dst].size(); ++j)
            {
                _tableau[dst][j] += f*_tableau[src][j];
            }
        }

        optional<size_t> ChooseEnteringVariable()
        {
            auto it = max_element(_tableau.back().begin(), _tableau.back().end()-1);
            if (*it > kErrorTolerance)
            {
                return it - _tableau.back().begin();
            }
            return {};
        }

        optional<size_t> ChooseLeavingVariable(size_t pivot)
        {
            double minRatio = kInf;
            optional<size_t> leaving;
            for (size_t i = 0; i < _tableau.size()-1; ++i)
            {
                if (_tableau[i][pivot] < kErrorTolerance)
                {
                    continue;
                }
                double ratio = _tableau[i].back() / _tableau[i][pivot];
                if (ratio < minRatio)
                {
                    minRatio = ratio;
                    leaving = i;
                }
            }
            return leaving;
        }

        void EnterNewVariable(size_t row, size_t pivot)
        {
            NormalizeRow(row, pivot);
            for (size_t otherRow = 0; otherRow < _tableau.size(); ++otherRow)
            {
                if (row == otherRow) continue;
                Reduce(row, otherRow, pivot);
            }
            _basicVariables[row] = pivot;
        }

        LPResult Run()
        {
            while (auto pivot = ChooseEnteringVariable())
            {
                if (auto leavingRow = ChooseLeavingVariable(*pivot))
                {
                    EnterNewVariable(*leavingRow, *pivot);
                }
                else
                {
                    return LPResult::Unbound;
                }
            }
            return LPResult::Ok;
        }

        LPResult Phase1()
        {
            auto result = Run();
            assert(result != LPResult::Unbound);
            double obj = _tableau.back().back();
            return obj < kErrorTolerance? LPResult::Ok : LPResult::Infeasible;
        }

        LPResult Phase2()
        {
            EliminateArtificialVariables();
            fill(_tableau.back().begin(), _tableau.back().end(), 0);
            for (size_t j = 0; j < _objective.size(); ++j)
            {
                _tableau.back()[j] = -_objective[j];
            }
            _tableau.back().back() = 0;
            MakeCanonical();
            return Run();
        }

        void EliminateArtificialVariables()
        {
            size_t nVariables = _tableau[0].size() - 1;
            size_t firstArtificialVariable = nVariables - _nArtificialVariables;

            vector<bool> keep(_tableau.size(), true);
            for (size_t i = 0; i < _basicVariables.size(); ++i)
            {
                bool isArtificial = _basicVariables[i] >= firstArtificialVariable;
                if (!isArtificial)
                {
                    continue;
                }
                auto end = _tableau[i].begin() + firstArtificialVariable;
                auto picked = find_if(_tableau[i].begin(), end,
                        [](double x){return fabs(x) > kErrorTolerance;});
                if (picked == end)
                {
                    keep[i] = false;
                    continue;
                }
                size_t pivot = picked - _tableau[i].begin();
                EnterNewVariable(i, pivot);
            }

            FilterTableau(keep);

            for (auto& row : _tableau)
            {
                row.erase(row.end()-_nArtificialVariables-1, row.end()-1);
            }
        }

        void MakeCanonical()
        {
            for (size_t i = 0; i < _basicVariables.size(); ++i)
            {
                if (fabs(_tableau.back()[_basicVariables[i]]) > kErrorTolerance)
                {
                    Reduce(i, _tableau.size()-1, _basicVariables[i]);
                }
            }
        }

        void SetUpTableau()
        {
            size_t nVariables = _nProblemVariables + _nSlackVariables + _nSurplusVariables + _nArtificialVariables;
            size_t currentSlackVariable = _nProblemVariables;
            size_t currentSurplusVariable = currentSlackVariable + _nSlackVariables;
            size_t currentArtificialVariable = currentSurplusVariable + _nSurplusVariables; 
            
            _basicVariables.clear();
            _tableau.clear();
            _tableau.resize(1 + _constraints.size(), vector<double>(nVariables+1));

            for (size_t i = 0; i < _constraints.size(); ++i)
            {
                _tableau[i].back() = _constraints[i].right;
                for (size_t j = 0; j < _nProblemVariables; ++j)
                {
                    _tableau[i][j] = _constraints[i].left[j];
                }
                switch (_constraints[i].type)
                {
                    case Constraint::Equality:
                        _basicVariables.push_back(currentArtificialVariable);
                        _tableau.back()[currentArtificialVariable] = -1.0;
                        _tableau[i][currentArtificialVariable++] = 1.0;
                        break;
                    case Constraint::LessOrEqual:
                        _basicVariables.push_back(currentSlackVariable);
                        _tableau[i][currentSlackVariable++] = 1.0;
                        break;
                    case Constraint::GreaterOrEqual:
                        _basicVariables.push_back(currentArtificialVariable);
                        _tableau.back()[currentArtificialVariable] = -1.0;
                        _tableau[i][currentArtificialVariable++] = 1.0;
                        _tableau[i][currentSurplusVariable++] = -1.0;
                        break;
                }
            }

            MakeCanonical();
        }

        void FilterTableau(const vector<bool>& mask)
        {
            vector<size_t> basicVariables;
            vector<vector<double>> tableau;
            for (size_t i = 0; i < mask.size()-1; ++i)
            {
                if (mask[i])
                {
                    basicVariables.push_back(_basicVariables[i]);
                    tableau.push_back(move(_tableau[i]));
                }
            }
            tableau.push_back(move(_tableau.back()));
            _basicVariables = move(basicVariables);
            _tableau = move(tableau);
        }

        vector<double> _objective;

        vector<Constraint> _constraints;

        size_t _nProblemVariables;
        size_t _nSlackVariables;
        size_t _nSurplusVariables;
        size_t _nArtificialVariables;

        vector<size_t> _basicVariables;
        vector<vector<double>> _tableau;
};

class IPSolver
{
    public:

        explicit IPSolver(const Row& row)
            : _upperBound(kInf)
        {
            _lpSolver.SetObjective(vector<double>(row.buttons.size(), 1.0));
            for (size_t i = 0; i < row.joltages.size(); ++i)
            {
                Constraint constraint;
                constraint.type = Constraint::Equality;
                constraint.left.resize(row.buttons.size());
                for (size_t j = 0; j < row.buttons.size(); ++j)
                {
                    constraint.left[j] = (row.buttons[j]>>i)&1? 1.0 : 0.0;
                }
                constraint.right = row.joltages[i];
                _lpSolver.AddConstraint(move(constraint));
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

        unsigned Objective() const
        {
            return static_cast<unsigned>(round(_upperBound));
        }

    private:

        bool SolveR()
        {
            if (_lpSolver.Solve() != LPResult::Ok || _lpSolver.Objective() >= _upperBound)
            {
                return false;
            }
            auto x = _lpSolver.GetSolution();
            auto it = find_if_not(x.begin(), x.end(), IsInteger);
            if (it == x.end())
            {
                if (_lpSolver.Objective() < _upperBound)
                {
                    _upperBound = _lpSolver.Objective();
                    _x = x;
                    return true;
                }
                return false;
            }

            bool found = false;
            size_t branchVar = it - x.begin();

            _lpSolver.AddLowerBoundConstraint(branchVar, ceil(x[branchVar]));
            found = SolveR() || found;
            _lpSolver.PopConstraint();

            _lpSolver.AddUpperBoundConstraint(branchVar, floor(x[branchVar]));
            found = SolveR() || found;
            _lpSolver.PopConstraint();

            return found;
        }

        LPSolver _lpSolver;
        vector<double> _x;
        double _upperBound;
};

int main()
{
    auto input = Input();
    unsigned answer = 0;
    for (const auto& row : input)
    {
        IPSolver solver(row);
        assert(solver.Solve());
        answer += solver.Objective();
        //cout << solver.GetSolution() << ' ' << solver.Objective() << endl;
    }
    cout << answer << endl;
}
