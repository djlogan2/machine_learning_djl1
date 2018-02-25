#ifndef NODEMAP_H
#define NODEMAP_H

#include "direction.h"   // for DIRECTION
#include "node.h"  // for Node

#include <unordered_map>
#include <vector>

class PossibleNode {
public:
  PossibleNode() { cost = std::numeric_limits<int>::max(); node_location = -1; }
  int cost;
  int node_location;
  std::vector<std::pair<DIRECTION, int>> children;
  int child(DIRECTION d);
  int child(std::vector<DIRECTION> legalmoves);
//  std::vector<DIRECTION> moves();
};

class hashing_func_2
{
  public:
    unsigned long operator()(const std::vector<int> &key) const {
      return key.size()*10000 + key[0];
    };
};

class NodeMap {
public:
    void add_to_nodes(DIRECTION where, int howmany);
    void clear() {
        previous_state = -1;
        goal = -1;
        width = height = 1;
        nodes.clear();
    }
    NodeMap & operator=(const NodeMap &other);

    std::unordered_map<int, Node>::const_iterator find(int which) { return nodes.find(which); }
    std::unordered_map<int, Node>::const_iterator end() { return nodes.end(); }
    std::unordered_map<int, Node>::const_iterator begin() { return nodes.begin(); }
    Node &at(const int which) { return nodes.at(which); }
    void emplace(int state, Node node) { nodes.emplace(state, node); }
    bool empty() { return nodes.empty(); }

    void merge(NodeMap & other);
    void buildPossibles();
    PossibleNode &pm_at(int state) {return possibles_map[state]; }
    int pm_first(std::vector<DIRECTION> legalmoves) { return possibles_map[possibles_root].child(legalmoves); }
    int pmroot() { return possibles_root; }
    const std::vector<std::pair<DIRECTION, int>> &pmchildren(int state) { return possibles_map[state].children; }
    int goal = -1;
    int previous_state = -1;
    int width = 1;
    int height = 1;

    void zerocosts() { for(auto it = nodes.begin() ; it != nodes.end() ; ++it) it->second.cost(0); }
protected:
    std::unordered_map<int, Node> nodes;

    std::vector<int> get_possibles(std::vector<DIRECTION> legalmoves);
    std::vector<int> get_next_possibles(std::vector<int> possibles, DIRECTION dir);
    std::vector<int> get_next_possibles(std::vector<int> possibles, int legalmovesbyte);

    std::vector<PossibleNode> possibles_map;
    int possibles_root;
    int buildPossiblesDirection(std::vector<int> possibles, DIRECTION legalmovesbyte, DIRECTION skipmove);
    int buildPossiblesSquare(std::vector<int> possibles, DIRECTION skipdirection);
    void sort(std::vector<std::pair<DIRECTION, int>> &children);
};

#endif // NODEMAP_H
