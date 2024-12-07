#include <cmath>
#include <iostream>
#include <vector>
#include <string>
using namespace std;

typedef int64_t i64;

vector<string> split(const string& str, const string& delim=" \n\t") {
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

//bool canBeSolved(i64 target, const vector<i64>& operands)
//{
    //vector<bool> dp((target+1)*(operands.size()+1));

    //auto dp_at = [&](i64 accumulated, size_t operandIndex)
    //{
        //return dp[accumulated*(operands.size()+1) + operandIndex];
    //};

    //dp_at(target, operands.size()) = true;

    //for (i64 accumulated = target-1; accumulated >= 0; --accumulated)
    //{
        //for (size_t operandIndex = 0; operandIndex < operands.size(); ++operandIndex)
        //{
            //if (accumulated + operands[operandIndex] <= target)
            //{
                //dp_at(accumulated, operandIndex) =
                    //dp_at(accumulated,operandIndex) ||
                    //dp_at(accumulated+operands[operandIndex], operandIndex+1);
            //}
            //if (accumulated * operands[operandIndex] <= target)
            //{
                //dp_at(accumulated, operandIndex) =
                    //dp_at(accumulated,operandIndex) ||
                    //dp_at(accumulated*operands[operandIndex], operandIndex+1);
            //}
        //}
    //}

    //return dp_at(0, 0);
//}

i64 concatenate(i64 left, i64 right)
{
    if (right == 0)
    {
        return left*10;
    }
    i64 rightCopy = right;
    while (rightCopy)
    {
        left *= 10;
        rightCopy /= 10;
    }
    return left + right;
}

i64 eval(const vector<i64> operands, uint64_t operators)
{
    i64 ans = operands[0];
    for (size_t index = 1; index < operands.size(); ++index)
    {
        switch (operators%3)
        {
            case 0:
                ans += operands[index];
                break;
            case 1:
                ans *= operands[index];
                break;
            default:
                ans = concatenate(ans, operands[index]);
                break;
        }
        operators /= 3;
    }
    return ans;
}

bool canBeSolved(i64 target, const vector<i64>& operands)
{
    for (uint64_t operators = 0; operators < pow(3, operands.size()-1); ++operators)
    {
        if (eval(operands, operators) == target)
        {
            return true;
        }
    }
    return false;
}

int main()
{
    i64 ans = 0;
    string line;
    while (getline(cin, line))
    {
        auto parts = split(line, ": ");
        i64 target = stoll(parts[0]);
        vector<i64> operands(parts.size()-1);
        for (size_t i = 0; i < operands.size(); ++i)
        {
            operands[i] = stoll(parts[i+1]);
        }
        ans += canBeSolved(target, operands)*target;
    }
    cout << ans << endl;
}
