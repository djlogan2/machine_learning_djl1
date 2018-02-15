#ifndef NODEMAP_H
#define NODEMAP_H

#include "node.h"
#include <unordered_map>

class NodeMap {
public:
    std::unordered_map<int, Node> nodes;
    int goal = -1;
    int previous_state = -1;
    int width = 1;
    int height = 1;
    void add_to_nodes(int where, int howmany);
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
};

#endif // NODEMAP_H
