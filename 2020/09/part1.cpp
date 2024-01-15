#include <cstdint>
#include <iostream>
#include <unordered_set>
#include <deque>
using namespace std;

class fifo_cache_t {
  public:
    typedef deque<int64_t>::const_iterator const_iterator;

    fifo_cache_t(size_t capacity) : m_capacity(capacity) {}

    void insert(int64_t x) {
      m_queue.push_back(x);
      m_set.insert(x);
      if (m_queue.size() > m_capacity) pop();
    }

    void pop() {
      m_set.erase(m_queue.front());
      m_queue.pop_front();
    }

    bool contains(int64_t x) const { return m_set.find(x) != m_set.end(); }

    bool full() const { return m_queue.size() == m_capacity; }

    bool contains_pair_that_sums_to(int64_t z) const {
      for (int64_t x : m_queue) {
        int64_t y = z - x;
        if (y != x && contains(y))
          return true;
      }
      return false;
    }

    const_iterator begin() const { return m_queue.begin(); }

    const_iterator end() const { return m_queue.end(); }

  private:
    deque<int64_t> m_queue;
    unordered_set<int64_t> m_set;
    size_t m_capacity;
};

ostream& operator<<(ostream& out, const fifo_cache_t& cache) {
  out << '[';
  bool first = true;
  for (auto x : cache) {
    if (!first) out << ", ";
    out << x;
    first = false;
  }
  return out << ']';
}

int main(int argc, char* argv[]) {
  size_t cache_size = 25;
  if (argc > 2) {
    cerr << "Usage: " << argv[0] << " [cache size]\n";
    return 1;
  }
  if (argc == 2)
    cache_size = stoull(argv[1]);
  fifo_cache_t cache(cache_size);
  int64_t x;
  while (cin >> x) {
    if (cache.full() && !cache.contains_pair_that_sums_to(x)) {
      cout << x << endl;
      break;
    }
    cache.insert(x);
  }
}

