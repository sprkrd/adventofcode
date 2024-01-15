#include <iostream>
#include <unordered_map>
#include <vector>
using namespace std;

typedef unordered_map<string,int> Part;

struct Range {
    int lo;
    int hi;

    Range(int lo = 0, int hi = -1) : lo(lo), hi(hi) {}

    bool empty() const {
        return hi<lo;
    }

    int size() const {
        return hi>=lo? hi-lo+1 : 0;
    }

};

ostream& operator<<(ostream& out, const Range& range) {
    return out << '[' << range.lo << ", " << range.hi << ']';
}

typedef unordered_map<string,Range> PartSet;

ostream& operator<<(ostream& out, const PartSet& part_set) {
    bool first = true;
    for (const auto&[var,range] : part_set) {
        if (!first) {
            out << ", ";
        }
        first = false;
        out << var << ":" << range;
    }
    return out;
}

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

    pair<PartSet,PartSet> split(const PartSet& in) const {
        if (var.empty()) {
            return {in, PartSet()};
        }
        pair<PartSet,PartSet> out(in,in);
        auto it = in.find(var);
        Range range = it==in.end()? Range(0,-1) : it->second;
        if (lt) {
            out.first[var] = Range(range.lo, min(range.hi, arg-1));
            out.second[var] = Range(max(range.lo, arg), range.hi);
        }
        else {
            out.first[var] = Range(max(range.lo,arg+1), range.hi);
            out.second[var] = Range(range.lo, min(range.hi,arg));
        }
        return out;
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

bool empty(const PartSet& ps) {
    for (const auto&[var,range] : ps) {
        if (range.empty()) {
            return true;
        }
    }
    return false;
}

size_t size(const PartSet& ps) {
    size_t ans = 1;
    for (const auto&[var,range] : ps) {
        ans *= range.size();
    }
    return ans;
}

struct Beam {
    PartSet part_set;
    string workflow_name;

    static Beam initial_beam() {
        static constexpr const char* varnames[] = {"x", "m", "a", "s"};
        Beam start;
        for (const char* var : varnames) {
            start.part_set.try_emplace(var, 1, 4000);
        }
        start.workflow_name = "in";
        return start;
    }

    vector<Beam> advance(const WorkflowSet& wfs) const {
        vector<Beam> beams;
        PartSet filtered;
        PartSet remaining = part_set;
        for (const auto& rule : wfs.workflows.find(workflow_name)->second.rules) {
            tie(filtered,remaining) = rule.split(remaining);
            if (!empty(filtered)) {
                beams.push_back(Beam{move(filtered), rule.dst});
            }
            if (empty(remaining)) {
                break;
            }
        }
        return beams;
    }
};


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


int main() {
    vector<Part> parts;
    WorkflowSet workflow_set;
    string line;
    while (getline(cin,line) && !line.empty()) {
        workflow_set.add_workflow(line);
    }

    size_t ans = 0;
    vector<Beam> beam_stk{Beam::initial_beam()};
    while (!beam_stk.empty()) {
        Beam beam = move(beam_stk.back());
        beam_stk.pop_back();
        if (beam.workflow_name == "R") {
            continue;
        }
        if (beam.workflow_name == "A") {
            ans += size(beam.part_set);
            continue;
        }
        auto children = beam.advance(workflow_set);
        beam_stk.insert(beam_stk.end(), children.begin(), children.end());
    }
    cout << ans << endl;
}
