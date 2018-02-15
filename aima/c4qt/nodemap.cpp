#include "nodemap.h"

void NodeMap::add_to_nodes(int where, int howmany) {
    std::unordered_map<int, Node> new_nodes;
    for(auto node : nodes) {
        node.second.add_to_states(where, howmany, width);
        new_nodes.emplace(node.second.state(), node.second);
    }
    nodes = new_nodes;
    if(previous_state != -1) previous_state = add_to_offset(previous_state, where, howmany, width);
    if(goal != -1) goal = add_to_offset(goal, where, howmany, width);
    if(where == UP || where == DOWN)
        width += howmany;
    else if(where == LEFT || where == RIGHT)
        height += howmany;
}

NodeMap & NodeMap::operator=(const NodeMap &other) {
    goal = other.goal;
    previous_state = other.previous_state;
    width = other.width;
    height = other.height;
    nodes = other.nodes;
}
