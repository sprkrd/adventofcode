#include <bits/stdc++.h>
using namespace std;

typedef vector<string> Shape;

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

struct Problem
{
    int width;
    int height;
    vector<int> piecesCount;
};

struct Input
{
    vector<Shape> shapes;
    vector<Problem> problems;
};

Problem ParseProblem(const string& line)
{
    Problem problem;
    auto parts = Split(line);
    auto headParts = Split(parts[0], "x");
    headParts.back().pop_back();
    problem.width = stoi(headParts[0]);
    problem.height = stoi(headParts[1]);
    problem.piecesCount.resize(parts.size()-1);
    for (size_t i = 1 ; i < parts.size(); ++i)
    {
        problem.piecesCount[i-1] = stoi(parts[i]);
    }
    return problem;
}

Input ReadInput()
{
    Input input;
    string line;
    while (getline(cin, line))
    {
        if (line.back() == ':')
        {
            Shape shape;
            while (getline(cin, line) && !line.empty())
            {
                shape.push_back(move(line));
            }
            input.shapes.push_back(move(shape));
        }
        else
        {
            break;
        }
    }

    input.problems.push_back(ParseProblem(line));
    while (getline(cin, line))
    {
        input.problems.push_back(ParseProblem(line));
    }

    return input;
}

enum Verdict { Ok, Ko, Unknown };

ostream& operator<<(ostream& out, Verdict verdict)
{
    static const char* strs[] = { "Ok", "Ko", "Unknown" };
    return out << strs[static_cast<int>(verdict)];
}

Verdict Broadphase(const Input& input, size_t problemIdx)
{
    static vector<int> filledCells = [&]()
    {
        vector<int> ans;
        for (const auto& shape : input.shapes)
        {
            int c = 0;
            for (const auto& row : shape)
            {
                c += count(row.begin(), row.end(), '#');
            }
            ans.push_back(c);
        }
        return ans;
    }();
    static pair<int,int> largestShapeDims = [&]()
    {
        int maxWidth = 0;
        int maxHeight = 0;
        for (const auto& shape : input.shapes)
        {
            maxWidth = max((int)shape[0].size(), maxWidth);
            maxHeight = max((int)shape.size(), maxHeight);
        }
        return make_pair(maxWidth, maxHeight);
    }();
    
    const auto& problem = input.problems[problemIdx];
    
    int nPieces = accumulate(problem.piecesCount.begin(), problem.piecesCount.end(), 0);
    int availableNxNSlots = (problem.width/largestShapeDims.first) * (problem.height/largestShapeDims.second);

    if (availableNxNSlots >= nPieces)
    {
        return Ok;
    }

    int nMinSpaceRequired = 0;
    for (size_t i = 0; i < problem.piecesCount.size(); ++i)
    {
        nMinSpaceRequired += problem.piecesCount[i]*filledCells[i];
    }
    if (nMinSpaceRequired > problem.width*problem.height)
    {
        return Ko;
    }

    return Unknown;
}

int main()
{
    int nOk = 0;
    auto input = ReadInput();
    for (size_t i = 0; i < input.problems.size(); ++i)
    {
        auto verdict = Broadphase(input, i);
        assert(verdict != Unknown);
        nOk += verdict==Ok;
    }
    cout << nOk << endl;
}
