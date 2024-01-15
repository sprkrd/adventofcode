#include <array>
#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>
#include <vector>
using namespace std;

typedef char Amphipod;

constexpr int kRoomCapacity = 4;
constexpr int kCorridorLength = 11;
constexpr int kNumberOfRooms = 4;
constexpr int kMoveCost[] = {1, 10, 100, 1000};

constexpr Amphipod kNull = '.';

inline int RoomXPosition(int room) {
    return 2*(room+1);
}

inline int RoomIndex(int roomX) {
    return roomX/2 - 1;
}

inline int AmphipodRoom(Amphipod amphipod) {
    return amphipod - 'A';
}

struct Action {
    int xSrc, ySrc, xDst, yDst;
};

struct State {
    typedef array<Amphipod,kRoomCapacity> Room;
    array<Amphipod,kCorridorLength> corridor;
    array<Room,4> rooms;

    State() {
        corridor.fill(kNull);
        for (int i = 0; i < kNumberOfRooms; ++i)
            rooms[i].fill(kNull);
    }

    vector<Action> GetAvailableActions() const {
        vector<Action> result;
        for (int room = 0; room < kNumberOfRooms; ++room) {
            int xSrc = RoomXPosition(room); 
            auto[amph, ySrc] = GetTopAmphipod(room);
            int roomDst = AmphipodRoom(amph);
            int xDst = RoomXPosition(roomDst);
            auto[_,yDst] = GetTopAmphipod(roomDst);
            --yDst;
            if (room != roomDst && RoomHasCorrectType(roomDst) && IsPathClear(xSrc, xDst)) {
                // room to room action
                result.push_back({xSrc, ySrc, xDst, yDst});
            }
            else if (!RoomHasCorrectType(room)) {
                // room to corridor actions
                for (int x = xSrc - 1; x >= 0; --x) {
                    if (corridor[x] != kNull)
                        break;
                    if (x%2 || x == 0)
                        result.push_back({xSrc, ySrc, x, 0});
                }
                for (int x = xSrc + 1; x < kCorridorLength; ++x) {
                    if (corridor[x] != kNull)
                        break;
                    if (x%2 || x == 10)
                        result.push_back({xSrc, ySrc, x, 0});
                }
            }
        }
        // corridor to room action
        for (int x = 0; x < kCorridorLength; ++x) {
            if (corridor[x] != kNull) {
                int roomDst = AmphipodRoom(corridor[x]);
                int roomX = RoomXPosition(roomDst);
                if (RoomHasCorrectType(roomDst) && IsPathClear(x, roomX)) {
                    auto[_,yDst] = GetTopAmphipod(roomDst);
                    yDst -= 1;
                    result.push_back({x, 0, roomX, yDst});
                }
            }
        }
        return result;
    }

    int ActionCost(const Action& action) const {
        Amphipod amph = Get(action.xSrc, action.ySrc);
        int moveCost = kMoveCost[AmphipodRoom(amph)];
        return moveCost*(action.ySrc + action.yDst + abs(action.xDst - action.xSrc));
    }

    bool IsPathClear(int xSrc, int xDst) const {
        int lo, hi;
        if (xSrc < xDst) {
            lo = xSrc + 1;
            hi = xDst;
        }
        else {
            lo = xDst;
            hi = xSrc - 1;
        }
        for (int x = lo; x <= hi; ++x) {
            if (corridor[x] != kNull)
                return false;
        }
        return true;
    }

    pair<Amphipod,int> GetTopAmphipod(int room) const {
        for (int y = 1; y <= kRoomCapacity; ++y) {
            if (rooms[room][y-1] != kNull)
                return {rooms[room][y-1], y};
        }
        return {kNull, kRoomCapacity+1};
    }

    Amphipod Get(int x, int y) const {
        Amphipod amph;
        if (y != 0) {
            int roomIndex = RoomIndex(x);
            amph = rooms[roomIndex][y-1];
        }
        else
            amph = corridor[x];
        return amph;
    }

    void Set(int x, int y, Amphipod amph) {
        if (y != 0) {
            int roomIndex = RoomIndex(x);
            rooms[roomIndex][y-1] = amph;
        }
        else
            corridor[x] = amph;
    }

    void Do(const Action& action) {
        Amphipod amph = Get(action.xSrc, action.ySrc);
        Set(action.xSrc, action.ySrc, kNull);
        Set(action.xDst, action.yDst, amph);
    }

    bool RoomHasCorrectType(int room) const {
        for (int y = 1; y <= kRoomCapacity; ++y) {
            if (rooms[room][y-1] != kNull &&
                    AmphipodRoom(rooms[room][y-1]) != room)
                return false;
        }
        return true;
    }

    bool RoomIsDone(int room) const {
        for (int y = 1; y < kRoomCapacity; ++y) {
            if (AmphipodRoom(rooms[room][y-1]) != room)
                return false;
        }
        return true;
    }


    bool Final() const {
        for (int room = 0; room < kNumberOfRooms; ++room) {
            if (!RoomIsDone(room))
                return false;
        }
        return true;
    }

    void Load(istream& in) {
        string line;
        getline(in, line);
        getline(in, line);
        for (int y = 1; y <= kRoomCapacity; ++y) {
            getline(in, line);
            for (int room = 0; room < kNumberOfRooms; ++room) {
                int x = RoomXPosition(room);
                Set(x, y, line[x+1]);
            }
        }
        getline(in, line);
    }

    bool operator==(const State& other) const {
        return corridor == other.corridor &&
               rooms == other.rooms;
    }
};

namespace std {
    template<>
    struct hash<State> {
        size_t operator()(const State& state) const {
            size_t result = 0;
            for (int i = 0; i < kCorridorLength; ++i) {
                result = result*127 + state.corridor[i];
            }
            for (int room = 0; room < kNumberOfRooms; ++room) {
                for (int i = 0; i < kRoomCapacity; ++i)
                    result = result*127 + state.rooms[room][i];
            }
            return result;
        }
    };
}

ostream& operator<<(ostream& out, const State& state) {
    auto& rooms = state.rooms;
    out << "#############\n";
    out << "#";
    for (char amph : state.corridor)
        out << amph;
    out << "#\n";
    out << "###" << rooms[0][0] << '#' << rooms[1][0] << '#'
        << rooms[2][0] << '#' << rooms[3][0] << "###\n";
    for (int i = 1; i < kRoomCapacity; ++i) {
        out << "  #" << rooms[0][i] << '#' << rooms[1][i] << '#'
            << rooms[2][i] << '#' << rooms[3][i] << "#\n";
    }
    out << "  #########\n";
    return out;
}

ostream& operator<<(ostream& out, const Action& action) {
    out << '(' << action.xSrc << ',' << action.ySrc << ") -> "
        << '(' << action.xDst << ',' << action.yDst << ')';
    return out;
}

int Search(const State& start) {
    typedef pair<int,State> CostState;
    auto cmp = [](const CostState& cs1, const CostState& cs2) {
        return cs1.first > cs2.first;
    };
    priority_queue<CostState,vector<CostState>,decltype(cmp)> open(cmp);
    open.emplace(0,start);
    unordered_map<State,int> costMap;
    costMap[start] = 0;
    int expanded = 0;
    int visited = 1;
    int result = -1;
    while (!open.empty()) {
        auto[cost,state] = open.top();
        open.pop();
        if (cost != costMap[state])
            continue;
        if (state.Final()) {
            result = cost;
            break;
        }
        ++expanded;
        for (const Action& action : state.GetAvailableActions()) {
            ++visited;
            State successor = state;
            successor.Do(action);
            int actionCost = state.ActionCost(action);
            int tentativeCost = cost + actionCost;
            int currentCost = numeric_limits<int>::max();
            auto it = costMap.find(successor);
            if (it != costMap.end())
                currentCost = it->second;
            if (tentativeCost < currentCost) {
                open.emplace(tentativeCost, successor);
                costMap[successor] = tentativeCost;
            }
        }
    }
    cout << "#expanded nodes: " << expanded << endl;
    cout << "#visited nodes: " << visited << endl;
    return result;
}

int main() {
    State state;
    state.Load(cin);
    cout << state << endl;
    cout << Search(state) << endl;
}
