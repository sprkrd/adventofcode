#include <array>
#include <cassert>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>
using namespace std;

typedef array<int,3> Point3i;

Point3i operator+(const Point3i& point1, const Point3i& point2) {
    return Point3i {point1[0]+point2[0], point1[1]+point2[1], point1[2]+point2[2]};
}

Point3i operator-(const Point3i& point1, const Point3i& point2) {
    return Point3i {point1[0]-point2[0], point1[1]-point2[1], point1[2]-point2[2]};
}

ostream& operator<<(ostream& out, const Point3i& point) {
    return out << '(' << point[0] << ',' << point[1] << ',' << point[2] << ')';
}

struct Rot {
    array<array<int,3>,3> matrix;
    
    Rot(int m00 = 0, int m01 = 0, int m02 = 0,
        int m10 = 0, int m11 = 0, int m12 = 0,
        int m20 = 0, int m21 = 0, int m22 = 0) :
        matrix{{ {m00, m01, m02},
                 {m10, m11, m12},
                 {m20, m21, m22} }} {
        
    }
    
    static Rot identity() {
        return Rot(1, 0, 0, 0, 1, 0, 0, 0, 1);
    }
    
    Rot operator*(const Rot& other) const {
        Rot ans;
        for (int i = 0; i < 3; ++i) {
            for (int j = 0; j < 3; ++j) {
                ans.matrix[i][j] = 0;
                for (int k = 0; k < 3; ++k) {
                    ans.matrix[i][j] += matrix[i][k]*other.matrix[k][j];
                }
            }
        }
        return ans;
    }
    
    Rot pow(int n) const {
        Rot ans = identity();
        for (int i = 0; i < n; ++i) {
            ans = ans * (*this);
        }
        return ans;
    };
    
    Point3i transform(const Point3i& point) const {
        Point3i ans{0,0,0};
        ans[0] = point[0]*matrix[0][0] + point[1]*matrix[0][1] + point[2]*matrix[0][2];
        ans[1] = point[0]*matrix[1][0] + point[1]*matrix[1][1] + point[2]*matrix[1][2];
        ans[2] = point[0]*matrix[2][0] + point[1]*matrix[2][1] + point[2]*matrix[2][2];
        return ans;
    }
    
    bool operator==(const Rot& other) const {
        return matrix == other.matrix;
    }
    
};

struct PointCloud {
    set<Point3i> points;
    Point3i origin;
    int reference;
};  

struct Transform {
    Rot rotation;
    Point3i offset;
    
    Point3i transform(const Point3i& point) const {
        return rotation.transform(point) + offset;
    }
    
    PointCloud transform(const PointCloud& pcl) const {
        PointCloud ans;
        ans.reference = pcl.reference;
        ans.origin = transform(pcl.origin);
        for (const auto& point : pcl.points) {
            ans.points.insert(transform(point));
        }
        return ans;
    }
};

ostream& operator<<(ostream& out, const Rot& rot) {
    out << rot.matrix[0][0] << '\t' << rot.matrix[0][1] << '\t' << rot.matrix[0][2] << '\n';
    out << rot.matrix[1][0] << '\t' << rot.matrix[1][1] << '\t' << rot.matrix[1][2] << '\n';
    out << rot.matrix[2][0] << '\t' << rot.matrix[2][1] << '\t' << rot.matrix[2][2] << '\n';
    return out;
}

Rot all_orientations[24];

void init() {
    Rot x_rotation(1, 0,  0,
                   0, 0, -1,
                   0, 1,  0);
    Rot y_rotation(0, 0, 1,
                   0, 1, 0,
                  -1, 0, 0);
    Rot z_rotation(0, -1, 0,
                   1,  0, 0,
                   0,  0, 1);
    int index = 0;
    
    for (int roll = 0; roll < 4; ++roll) {
        for (int yaw = 0; yaw < 4; ++yaw) {
            all_orientations[index++] = x_rotation.pow(roll) * z_rotation.pow(yaw);
        }
    }
    for (int pitch = 1; pitch <= 3; pitch += 2) {
        for (int yaw = 0; yaw < 4; ++yaw) {
            all_orientations[index++] = y_rotation.pow(pitch) * z_rotation.pow(yaw);
        }
    }
}

PointCloud rotate_pointcloud(const PointCloud& pcl, const Rot& rot) {
    PointCloud ans;
    ans.reference = pcl.reference;
    ans.origin = pcl.origin;
    for (const auto& point : pcl.points) {
        ans.points.insert(rot.transform(point));
    }
    return ans;
}

bool align(const PointCloud& pcl1, const PointCloud& pcl2, Transform& out) {
    for (int orientation = 0; orientation < 24; ++orientation) {
        auto pcl2_trans = rotate_pointcloud(pcl2, all_orientations[orientation]);
        map<Point3i,int> offset_frequency;
        Point3i most_frequent_offset{0};
        int largest_frequency = 0;
        for (const auto& point1 : pcl1.points) {
            for (const auto& point2 : pcl2_trans.points) {
                auto offset = point1 - point2;
                int freq = ++offset_frequency[offset];
                if (freq > largest_frequency) {
                    most_frequent_offset = offset;
                    largest_frequency = freq;
                }
            }
        }
        //~ cout << largest_frequency << ' ' << most_frequent_offset << endl;
        if (largest_frequency >= 12) {
            out.rotation = all_orientations[orientation];
            out.offset = most_frequent_offset;
            return true;
        }
    }
    return false;
}

vector<string> split(const string& str, const string& delim=" \n\t") {
    vector<string> parts;
    size_t pos = 0;
    while (pos < str.length()) {
        size_t pos_next = str.find_first_of(delim, pos);
        if (pos_next > pos)
            parts.push_back(str.substr(pos, pos_next-pos));
        pos = pos_next == string::npos? string::npos : pos_next + 1;
    }
    return parts;
}



int main() {
    init();
    vector<PointCloud> readings;
    string line;
    while (getline(cin, line)) {
        if (line.substr(0, 3) == "---") {
            readings.emplace_back();
            readings.back().reference = readings.size() - 1;
            readings.back().origin.fill(0);
        }
        else if (!line.empty()) {
            auto parts = split(line, ",");
            readings.back().points.insert(Point3i{stoi(parts[0]), stoi(parts[1]), stoi(parts[2])});
        }
    }
    
    vector<vector<pair<int,Transform>>> parent_transform(readings.size());
    
    for (int i = 0; i < readings.size(); ++i) {
        for (int j = 0; j < readings.size(); ++j) {
            Transform transform;
            if (i != j && align(readings[i], readings[j], transform)) {
                parent_transform[j].emplace_back(i, transform);
            }
        }
    }
    
    vector<pair<int,Transform>> frame_tree(readings.size(), {-1,Transform()});
    bool work_pending = true;
    while (work_pending) {
        work_pending = false;
        for (int i = 1; i < parent_transform.size(); ++i) {
            if (frame_tree[i].first != -1) {
                continue;
            }
            for (const auto&[parent,transform] : parent_transform[i]) {
                if (parent == 0 || frame_tree[parent].first != -1) {
                    work_pending = true;
                    frame_tree[i].first = parent;
                    frame_tree[i].second = transform;
                    break;
                }
            }
        }
    }
    
    //~ for (int i = 0; i < frame_tree.size(); ++i) {
        //~ cout << i << ": " << frame_tree[i].first << ' ' << frame_tree[i].second.offset << endl;
        //~ bool first = true;
        //~ for (const auto&[parent,transform] : parent_transform[i]) {
            //~ if (!first) {
                //~ cout << ", ";
            //~ }
            //~ cout << parent << ' ' << transform.offset;
            //~ first = false;
        //~ }
        //~ cout << endl;
    //~ }
    
    work_pending = true;
    while (work_pending) {
        work_pending = false;
        for (auto& scanner : readings) {
            int parent = frame_tree[scanner.reference].first;
            if (parent != -1) {
                scanner = frame_tree[scanner.reference].second.transform(scanner);
                scanner.reference = frame_tree[scanner.reference].first;
                work_pending = true;
            }
        }
    }
    
    set<Point3i> beacons;
    for (const auto& scanner : readings) {
        for (const auto& point : scanner.points) {
            beacons.insert(point);
        }
    }
    
    cout << beacons.size() << endl;
    
    //~ for (int i = 0; i < readings.size(); ++i) {
        //~ cout << i << ": " << readings[i].origin << endl;
    //~ }
}
