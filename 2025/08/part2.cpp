#include <bits/stdc++.h>
using namespace std;

typedef int64_t Int;
typedef array<Int,3> Point3i;

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

vector<Point3i> Input()
{
    vector<Point3i> input;
    string line;
    while (getline(cin,line))
    {
        auto parts = Split(line, ",");
        Int x = stoll(parts[0]);
        Int y = stoll(parts[1]);
        Int z = stoll(parts[2]);
        input.push_back(Point3i{x,y,z});
    }
    return input;
}

Int SquareDistance(const Point3i& p, const Point3i& q)
{
    Int dx = p[0] - q[0];
    Int dy = p[1] - q[1];
    Int dz = p[2] - q[2];
    return dx*dx + dy*dy + dz*dz;
}

ostream& operator<<(ostream& out, const Point3i& point)
{
    out << '(' << point[0] << ',' << point[1] << ',' << point[2] << ')';
    return out;
}

vector<tuple<Int,size_t,size_t>> AllSqDistancePairs(const vector<Point3i>& points)
{
    vector<tuple<Int,size_t,size_t>> sqDistances;
    for (size_t i = 0; i < points.size(); ++i)
    {
        for (size_t j = i+1; j < points.size(); ++j)
        {
            sqDistances.emplace_back(SquareDistance(points[i],points[j]),i,j);
        }
    }
    sort(sqDistances.begin(), sqDistances.end());
    return sqDistances;
}

class DisjointSets
{
    public:
        DisjointSets(size_t size)
        : _parent(size)
        , _setSize(size, 1)
        , _nSets(size)
        {
            iota(_parent.begin(), _parent.end(), 0);
        }
        
        int Find(int i)
        {
            if (_parent[i] == i) return i;
            _parent[i] = Find(_parent[i]);
            return _parent[i];
        }
        
        void Join(int i, int j)
        {
            int parentI = Find(i);
            int parentJ = Find(j);
            if (parentI == parentJ) return;
            if (_setSize[parentI] <= _setSize[parentJ])
            {
                _parent[parentI] = parentJ;
                _setSize[parentJ] += _setSize[parentI];
            }
            else
            {
                _parent[parentJ] = parentI;
                _setSize[parentI] += _setSize[parentJ];
            }
            --_nSets;
        }
        
        int SetSize(int i) const
        {
            return _setSize[i];
        }
        
        int NumberOfSets() const
        {
            return _nSets;
        }
        
    private:
        vector<int> _parent;
        vector<int> _setSize;
        int _nSets;
};

int main()
{
    auto input = Input();
    auto sqDistances = AllSqDistancePairs(input);
    DisjointSets disjointSets(input.size());
    int n = 0;
    while (true)
    {
        auto[_,i,j] = sqDistances[n];
        disjointSets.Join(i,j);
        ++n;
        if (disjointSets.NumberOfSets() == 1)
        {
            Int Xi = input[i][0];
            Int Xj = input[j][0];
            Int ans = Xi*Xj;
            cout << ans << endl;
            break;
        }
    }
}
