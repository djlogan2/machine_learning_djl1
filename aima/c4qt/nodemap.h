#ifndef NODEMAP_H
#define NODEMAP_H

#include "map.h"   // for DIRECTION
#include "node.h"  // for Node

#include <unordered_map>

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

    int goal = -1;
    int previous_state = -1;
    int width = 1;
    int height = 1;

    std::unordered_map<int, Node> nodes;
    std::unordered_map<int, Node>::const_iterator find(int which) { return nodes.find(which); }
    std::unordered_map<int, Node>::const_iterator end() { return nodes.end(); }
    std::unordered_map<int, Node>::const_iterator begin() { return nodes.begin(); }
    Node &at(const int which) { return nodes.at(which); }
    void emplace(int state, Node node) { nodes.emplace(state, node); }
    void merge(NodeMap & other);
};

#endif // NODEMAP_H
