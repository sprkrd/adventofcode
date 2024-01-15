#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

typedef unordered_map<string,int> Part;

struct Rule {
    string dst;
    string var;
    int arg;
    bool lt;

    bool accepts(const Part& part) const {
        if (var.empty()) {
            return true;
        }
        auto it = part.find(var);
        int value = it==part.end()? 0 : it->second;
        return ( lt && value<arg) ||
               (!lt && value>arg);
    }

    void parse(const string& in) {
        size_t pos_colon = in.find_first_of(':');
        string lhs;
        if (pos_colon == string::npos) {
            dst = in;
        }
        else {
            lhs = in.substr(0, pos_colon);
            dst = in.substr(pos_colon+1, in.size()-pos_colon-1);
        }

        if (lhs.empty()) {
            var.clear();
        }
        else {
            size_t pos_cmp = lhs.find_first_of("<>");
            var = lhs.substr(0, pos_cmp);
            lt = lhs[pos_cmp] == '<';
            arg = stoi(lhs.substr(pos_cmp+1,lhs.size()-pos_cmp-1));
        }
    }
};

struct Workflow {
    string name;
    vector<Rule> rules;

    string classify(const Part& part) const {
        for (const Rule& rule : rules) {
            if (rule.accepts(part)) {
                return rule.dst;
            }
        }
        return "";
    }

    void parse(const string& in) {
        rules.clear();
        size_t pos_lcb = in.find_first_of('{');
        string data;
        name = in.substr(0, pos_lcb);
        size_t begin = pos_lcb+1;
        while (begin < in.size()-1) {
            size_t pos_comma = in.find_first_of(',', begin);
            if (pos_comma > in.size()-1) {
                pos_comma = in.size() - 1;
            }
            Rule rule;
            rule.parse(in.substr(begin, pos_comma-begin));
            rules.push_back(move(rule));
            begin = pos_comma+1;
        }
    }
};

struct WorkflowSet {
    unordered_map<string,Workflow> workflows;

    void add_workflow(const string& workflow_text) {
        Workflow workflow;
        workflow.parse(workflow_text);
        workflows.emplace(string(workflow.name), move(workflow));
    }

    string classify(const Part& part) const {
        const Workflow* curr_workflow = &(workflows.find("in")->second);
        string dst = curr_workflow->classify(part);
        while (true) {
            string dst = curr_workflow->classify(part);
            auto it = workflows.find(dst);
            if (it == workflows.end()) {
                return dst;
            }
            curr_workflow = &(it->second);
        }
    }
};

Part parse_part(const string& line) {
    Part part;
    size_t begin = 1;
    while (begin < line.size()-1) {
        size_t pos_eq = line.find_first_of('=', begin);
        size_t pos_comma = line.find_first_of(',', begin);
        if (pos_comma > line.size()-1) {
            pos_comma = line.size() - 1;
        }
        part[line.substr(begin, pos_eq-begin)] =
            stoi(line.substr(pos_eq+1, pos_comma-pos_eq-1));
        begin = pos_comma+1;
    }
    return part;
}

int rating(const Part& part) {
    int rating = 0;
    for (const auto&[_,qty] : part) {
        rating += qty;
    }
    return rating;
}

ostream& operator<<(ostream& out, const Rule& rule) {
    if (!rule.var.empty()) {
        out << rule.var << (rule.lt? '<':'>') << rule.arg << ':';
    }
    return out << rule.dst;
}

ostream& operator<<(ostream& out, const Workflow& workflow) {
    out << workflow.name << '{';
    for (size_t i = 0; i < workflow.rules.size(); ++i) {
        if (i>0) {
            out << ',';
        }
        out << workflow.rules[i];
    }
    return out << '}';
}

ostream& operator<<(ostream& out, const WorkflowSet& workflow_set) {
    for (const auto&[_, workflow] : workflow_set.workflows) {
        out << workflow << '\n';
    }
    return out;
}

ostream& operator<<(ostream& out, const Part& part) {
    out << '{';
    bool first = true;
    for (const auto&[var,qty] : part) {
        if (!first) out << ',';
        first = false;
        out << var << '=' << qty;
    }
    return out << '}';
}

int main() {
    vector<Part> parts;
    WorkflowSet workflow_set;
    string line;
    while (getline(cin,line) && !line.empty()) {
        workflow_set.add_workflow(line);
    }
    while (getline(cin,line)) {
        parts.push_back(parse_part(line));
    }

    int ans = 0;
    for (const auto& part : parts) {
        string category = workflow_set.classify(part);
        if (category == "A") {
            ans += rating(part);
        }
    }
    cout << ans << endl;

}
