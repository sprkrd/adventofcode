#include <iostream>
#include <list>
#include <unordered_map>
using namespace std;

class indexed_list {
  public:
    typedef list<int> container_t;
    typedef container_t::iterator iterator;
    typedef container_t::const_iterator const_iterator;

    void push_back(int x) {
      if (x < m_min || m_list.empty())
        m_min = x;
      if (x > m_max || m_list.empty())
        m_max = x;
      m_list.push_back(x);
      m_index[x] = --m_list.end();
    }

    void rotate_left() {
      m_list.splice(m_list.end(), m_list, m_list.begin());
    }

    iterator find(int x) {
      auto it = static_cast<const indexed_list*>(this)->find(x);
      return m_list.erase(it,it);
    }

    const_iterator find(int x) const {
      auto it = m_index.find(x);
      if (it == m_index.end())
        return m_list.end();
      return it->second;
    }

    int front() const {
      return m_list.front();
    }

    int min() const {
      return m_min;
    }

    int max() const {
      return m_max;
    }

    void move(iterator first, iterator last, iterator dst) {
      m_list.splice(++dst, m_list, first, ++last);
    }

    iterator begin() {
      return m_list.begin();
    }

    iterator end() {
      return m_list.end();
    }

    const_iterator begin() const {
      return m_list.begin();
    }

    const_iterator end() const {
      return m_list.end();
    }

  private:
    unordered_map<int,iterator> m_index;
    container_t m_list;
    int m_min, m_max;
};

ostream& operator<<(ostream& out, const indexed_list& lst) {
  bool first = true;
  for (int x : lst) {
    if (!first) cout << ", ";
    cout << x;
    first = false;
  }
  return out;
}

void update(indexed_list& lst) {
  auto it1 = ++lst.begin();
  auto it2 = it1; ++it2;
  auto it3 = it2; ++it3;
  int target = lst.front();
  auto dst_it = lst.end();
  do {
    --target;
    if (target < lst.min())
      target = lst.max();
    dst_it = lst.find(target);
  } while (dst_it==it1 || dst_it==it2 || dst_it==it3);
  lst.move(it1,it3,dst_it);
  lst.rotate_left();
}

int main() {
  indexed_list lst;
  char cup;
  while (cin >> cup)
    lst.push_back(cup-'0');
  while (lst.max() != 1000000)
    lst.push_back(lst.max()+1);
  for (int i = 0; i < 10000000; ++i)
    update(lst);
  auto it_n = ++lst.find(1);
  if (it_n == lst.end()) it_n = lst.begin();
  auto it_nn = it_n; ++it_nn;
  if (it_nn == lst.end()) it_nn = lst.begin();
  cout << *it_n << ' ' << *it_nn << endl;
  long long product = (long long)(*it_n) * (*it_nn);
  cout << product << endl;
}

