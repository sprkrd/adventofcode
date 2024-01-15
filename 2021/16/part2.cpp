#include <cassert>
#include <iostream>
#include <limits>
#include <memory>
#include <string>
#include <vector>
using namespace std;

typedef vector<bool> BitString;

constexpr long long inf = numeric_limits<long long>::max();

constexpr int SUM = 0;
constexpr int PRODUCT = 1;
constexpr int MINIMUM = 2;
constexpr int MAXIMUM = 3;
constexpr int LITERAL_VALUE = 4;
constexpr int GREATER_THAN = 5;
constexpr int LESS_THAN = 6;
constexpr int EQUAL_TO = 7;

struct Packet;
typedef shared_ptr<Packet> PacketPtr;

struct Packet {
    long long value;
    int version;
    int id;
    vector<PacketPtr> subpackets;
};

BitString hex2bin(const string& hex) {
    BitString bin(hex.size()*4);
    for (int i = 0; i < hex.size(); ++i) {
        int num_val = (hex[i]>='A' && hex[i]<='F')? (10+hex[i]-'A') : (hex[i]-'0');
        bin[4*i]   = num_val&8;
        bin[4*i+1] = num_val&4;
        bin[4*i+2] = num_val&2;
        bin[4*i+3] = num_val&1;
    }
    return bin;
}

int bin2num(const BitString& bits, int pos = 0, int len = -1) {
    if (len == -1) {
        len = bits.size();
    }
    int end_pos = min((int)bits.size(), pos+len);
    int num = 0;
    for (int i = pos; i < end_pos; ++i) {
        num *= 2;
        num += bits[i];
    }
    return num;
}

long long parse_value(const BitString& bits, int& index) {
    long long value = 0;
    bool final_group;
    do {
        final_group = !bits[index];
        index += 1;
        value = value*16 + bin2num(bits, index, 4);
        index += 4;
    } while (!final_group);
    return value;
}


PacketPtr parse_next_packet(const BitString& bits, int& index) {
    auto packet = make_shared<Packet>();
    packet->version = bin2num(bits, index, 3);
    index += 3;
    packet->id  = bin2num(bits, index, 3);
    index += 3;
    if (packet->id == LITERAL_VALUE) {
        packet->value = parse_value(bits, index);
    }
    else {
        packet->value = 0;
        int length_type = bits[index];
        index += 1;
        if (length_type == 0) {
            int subpacket_bit_length = bin2num(bits, index, 15);
            index += 15;
            int index_end = index + subpacket_bit_length;
            while (index < index_end) {
                packet->subpackets.push_back(parse_next_packet(bits, index));
            }
            assert(index == index_end);
        }
        else {
            int num_subpackets = bin2num(bits, index, 11);
            index += 11;
            while (num_subpackets) {
                packet->subpackets.push_back(parse_next_packet(bits, index));
                --num_subpackets;
            }
        }
    }
    return packet;
}

PacketPtr parse_packets(const string& hex) {
    auto bin = hex2bin(hex);
    int i = 0;
    auto root = parse_next_packet(bin, i);
    // sanity check: make sure that the remaining bits are just padding
    while (i < bin.size()) {
        assert(!bin[i]);
        ++i;
    }
    return root;
}

ostream& operator<<(ostream& out, const BitString& bits) {
    for (bool bit : bits) out << bit;
    return out;
}

void print_packet(ostream& out, const Packet& packet, int indent) {
    string prefix(indent, ' ');
    out << prefix << "Packet{\n"
        << prefix << "  version: " << packet.version << ",\n"
        << prefix << "  id: " << packet.id << ",\n";
    if (packet.id == LITERAL_VALUE) {
        out << prefix << "  value: " << packet.value;
    }
    else {
        out << prefix << "  subpackets: [";
        bool first = true;
        for (const auto& subpacket : packet.subpackets) {
            if (!first) out << ',';
            out << '\n';
            print_packet(out, *subpacket, indent + 4);
            first = false;
        }
        out << '\n' << prefix << "  ]";
    }
    out << '\n' << prefix << '}';
}

ostream& operator<<(ostream& out, const Packet& packet) {
    print_packet(out, packet, 0);
    return out;
}

long long eval(const Packet& packet) {
    long long ans;
    switch (packet.id) {
        case SUM:
            ans = 0;
            for (const auto& subpacket : packet.subpackets)
                ans += eval(*subpacket);
            break;
        case PRODUCT:
            ans = 1;
            for (const auto& subpacket : packet.subpackets)
                ans *= eval(*subpacket);
            break;
        case MINIMUM:
            ans = inf;
            for (const auto& subpacket : packet.subpackets)
                ans = min(ans, eval(*subpacket));
            break;
        case MAXIMUM:
            ans = -inf;
            for (const auto& subpacket : packet.subpackets)
                ans = max(ans, eval(*subpacket));
            break;
        case LITERAL_VALUE:
            ans = packet.value;
            break;
        case GREATER_THAN:
            ans = eval(*packet.subpackets[0]) > eval(*packet.subpackets[1]);
            break;
        case LESS_THAN:
            ans = eval(*packet.subpackets[0]) < eval(*packet.subpackets[1]);
            break;
        case EQUAL_TO:
            ans = eval(*packet.subpackets[0]) == eval(*packet.subpackets[1]);
            break;
        default:
            assert(false);
    }
    return ans;
}

int main() {
    string hex;
    cin >> hex;
    auto packet = parse_packets(hex);
    //~ cout << *packet << endl;
    cout << eval(*packet) << endl;
}
