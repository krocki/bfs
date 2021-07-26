#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <vector>
#include <unordered_set>
#include <map>
#include <queue>
#include <deque>

#include "timer.h"

using namespace std;
typedef pair<int, float> edge;

struct Graph {

  unordered_set<int> visited;
  map<int, map<int, float>> adj;

  void read_file(string fname) {

    ifstream input(fname);
    string line;

    input.ignore(LONG_MAX, '\n');
    input.ignore(LONG_MAX, '\n');

    while (getline(input, line)) {
      istringstream iss(line);
      int src, dst;
      float w;

      if (!(iss >> src >> dst >> w)) {
        break;
      }

      insert(src, dst, w);
    }
  }

  void print() {
    for (const auto &entry: adj) {
      cout << "[" << entry.first << "] ";
      for (const auto &item: entry.second) {
        cout << std::fixed << std::setprecision(2) << item.first << " (" << item.second << "), ";
        //cout << std::fixed << std::setprecision(2) << item.first << " ";
      }
      cout << endl;
    }
  }

  void insert(int src, int dst, float w) {
    adj[src][dst] = w;
  }

  void unvisit() {
    visited.clear();
  }

  void clear() {
    visited.clear();
    adj.clear();
  }

  void dfs(int src) {
    if (visited.find(src) != visited.end())
      return;

    visited.insert(src);
    for (auto &c: adj[src]) {
      dfs(c.first);
    }
  }

  void bfs(int src) {

    visited.insert(src);

    deque<int> dq[2];
    int buf_no = 0;
    dq[buf_no].push_back(src);

    while (!dq[0].empty() || !dq[1].empty()) {

      while (!dq[buf_no].empty()) {

        int item = dq[buf_no].front();
        cout << "popping " << item << endl;
        dq[buf_no].pop_front();

        for (auto &c: adj[item]) {
          if (visited.find(c.first) == visited.end()) {
            cout << "appending " << c.first << "\n";
            dq[1-buf_no].push_back(c.first);
            visited.insert(c.first);
          }
        }

      }

      dq[buf_no].clear();
      buf_no = 1-buf_no;
    }
  }

  void dijkstra(int src, vector<edge> &out) {

    visited.insert(src);

    auto comp = [&](edge &a, edge &b) {
      return a.second > b.second;
    };

    priority_queue<edge, vector<edge>, decltype(comp)> pq(comp);

    pq.push({src, 0});

    while (!pq.empty()) {
      edge c = pq.top();
      pq.pop();
      out.push_back({c.first, c.second});
      for (auto &x: adj[c.first]) {
        if (visited.find(x.first) == visited.end()) {
          pq.push({x.first, x.second+c.second});
          visited.insert(x.first);
        }
      }
    }

  }

};

int main(int argc, char **argv) {

  if (argc < 2) {
    printf("syntax: %s file\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  Graph g;
  g.read_file(argv[1]);
  g.print();

  //timeit(g.dfs(0));
  //timeit(g.bfs(0));
  vector<edge> out;
  timeit(g.dijkstra(0, out));
  //print
  for (edge &e: out) {
    cout << e.first << ", " << e.second << "\n";
  }

  cout << "visited: " << "(" << g.visited.size() << ") ";

  for (auto &x: g.visited) {
    cout << x << " ";
  }
  cout << "\n";
  return 0;
}
