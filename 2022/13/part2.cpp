#include <algorithm>
#include <cassert>
#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Packet {
    int content;
    vector<Packet> subpackets;
    bool atomic;

    Packet() {
        reset();
    }

    Packet(const string& packet_str) {
        read(packet_str);
    }

    size_t read(const string& packet_str, size_t idx) {
        reset();
        size_t idx_start = idx;
        if (packet_str[idx] == '[') {
            ++idx;
            atomic = false;
            Packet p;
            size_t read_count;
            while ((read_count=p.read(packet_str, idx)) != 0) {
                subpackets.push_back(move(p));
                idx += read_count;
                if (packet_str[idx] == ',')
                    ++idx;
            }
            if (packet_str[idx] != ']') {
                reset();
                idx = idx_start;
            }
            else {
                ++idx;
            }
        }
        else if (isdigit(packet_str[idx])) {
            atomic = true;
            while (isdigit(packet_str[++idx]));
            content = stoi(packet_str.substr(idx_start, idx-idx_start));
        }
        size_t total_read = idx - idx_start;
        return total_read;
    }

    bool read(const string& packet_str) {
        return read(packet_str, 0) == packet_str.size();
    }

    void reset() {
        content = 0;
        subpackets.clear();
        atomic = true;
    }

    bool operator<(const Packet& other) const {
        if (atomic && other.atomic) {
            // atomic vs atomic case
            return content < other.content;
        }
        if (!atomic && !other.atomic) {
            // non-atomic vs non-atomic case
            size_t min_size = min(subpackets.size(), other.subpackets.size());
            for (size_t i = 0; i < min_size; ++i) {
                if (subpackets[i] < other.subpackets[i])
                    return true;
                else if (other.subpackets[i] < subpackets[i])
                    return false;
            }
            return subpackets.size() < other.subpackets.size();
        }
        if (atomic) {
            // atomic vs non-atomic case
            if (other.subpackets.empty())
                return false;
            if ((*this) < other.subpackets[0])
                return true;
            if (other.subpackets[0] < (*this))
                return false;
            return other.subpackets.size() > 1;
        }
        // !atomic vs atomic case
        if (subpackets.empty())
            return true;
        return subpackets[0] < other; 
    }

    bool operator==(const Packet& other) const {
        return !((*this) < other) && !(other < (*this));
    }
};

ostream& operator<<(ostream& out, const Packet& packet) {
    if (packet.atomic) {
        return out << packet.content;
    }
    out << '[';
    for (size_t i = 0; i < packet.subpackets.size(); ++i) {
        if (i > 0) out << ',';
        out << packet.subpackets[i];
    }
    return out << ']';
}


int main() {
    Packet divider1("[[2]]");
    Packet divider2("[[6]]");
    vector<Packet> packets{divider1, divider2};
    string line;
    while (getline(cin, line)) {
        if (!line.empty()) {
            Packet p;
            assert(p.read(line));
            packets.push_back(move(p));
        }
    }
    sort(packets.begin(), packets.end());

    size_t index1 = 0;
    size_t index2 = 0;
    for (size_t i = 0; i < packets.size(); ++i) {
        cout << packets[i] << endl;
        if (packets[i] == divider1)
            index1 = i+1;
        if (packets[i] == divider2)
            index2 = i+1;

    }
    size_t product = index1*index2;
    cout << product << endl;
}
