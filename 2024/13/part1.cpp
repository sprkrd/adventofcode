#include <iostream>
#include <vector>

std::vector<std::string> split(const std::string& str, const std::string& delim=" \n\t") {
    std::vector<std::string> parts;
    size_t pos = 0;
    while (pos < str.length())
    {
        size_t pos_next = str.find_first_of(delim, pos);
        if (pos_next > pos)
        {
            parts.push_back(str.substr(pos, pos_next-pos));
        }
        pos = pos_next == std::string::npos? std::string::npos : pos_next + 1;
    }
    return parts;
}

struct Problem
{
    int xa;
    int ya;
    int xb;
    int yb;
    int xt;
    int yt;

    bool verifySolution(int a, int b) const
    {
        return a >= 0 && b >= 0 && a*xa + b*xb == xt && a*ya + b*yb == yt;
    }

    int solutionCost(int a, int b)
    {
        return 3*a + b;
    }

    std::pair<int,int> solve() const
    {
        // A*XA + B*XB = XT
        // A*YA + B*YB = YB
        //
        // ( XA XB ) ( A ) = ( XT )
        // ( YA YB ) ( B ) = ( YT )
        // 
        // D = XA*YB - XB*YA
        // DA = XT*YB - XB*YT
        // DB = XA*YT - XT*YA
        //
        // D != 0
        // A = DA / D
        // B = DB / D
        //
        // D = DA = DB = 0
        // A = XT / XA, B = 0 if (XT/XB < 3*XT/XA)
        // A = 0, B = XT / XB if (3*XT/XA < XT/XB)
        //
        // Solution valid only iff integer A >= 0, B >= 0
        
        int a = -1;
        int b = -1;

        int d = xa*yb - xb*ya;
        int da = xt*yb - xb*yt;
        int db = xa*yt - xt*ya;

        if (d != 0)
        {
            a = da / d;
            b = db / d;
        }
        else if (da == 0 && db == 0)
        {
            int a1 = xt%xa == 0? xt/xa : -1;
            int b2 = xt%xb == 0? xt/xb : -1;
            if (a1 != -1 && (b2 == -1 || 3*a1 < b2))
            {
                a = a1;
                b = 0;
            }
            else
            {
                a = 0;
                b = b2;
            }
        }

        if (a != -1 && !verifySolution(a, b))
        {
            a = b = -1;
        }

        return {a, b};
    }

    bool read(std::istream& in)
    {
        std::string line;
        if (!getline(in, line))
        {
            return false;
        }
        auto parts = split(line);
        xa = stoi(parts[2].substr(2, parts[2].length()-3));
        ya = stoi(parts[3].substr(2, parts[3].length()-2));
        getline(in, line);
        parts = split(line);
        xb = stoi(parts[2].substr(2, parts[2].length()-3));
        yb = stoi(parts[3].substr(2, parts[3].length()-2));
        getline(in, line);
        parts = split(line);
        xt = stoi(parts[1].substr(2, parts[1].length()-3));
        yt = stoi(parts[2].substr(2, parts[2].length()-2));
        // skip empty line
        in.ignore(1000, '\n');
        return true;
    }
};

std::ostream& operator<<(std::ostream& out, const Problem& problem)
{
    out << "Button A: X+" << problem.xa << ", Y+" << problem.ya << '\n'
        << "Button B: X+" << problem.xb << ", Y+" << problem.yb << '\n'
        << "Prize: X=" << problem.xt << ", Y=" << problem.yt << '\n';
    return out;
}

int main()
{
    int ans = 0;
    Problem problem;
    while (problem.read(std::cin))
    {
        auto[a,b] = problem.solve();
        if (a != -1)
        {
            ans += problem.solutionCost(a, b);
        }
    }
    std::cout << ans << std::endl;
}
