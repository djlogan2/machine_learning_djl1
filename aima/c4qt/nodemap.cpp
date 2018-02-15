#include "nodemap.h"
#include "node.h"  // for add_to_offset

void NodeMap::add_to_nodes(DIRECTION where, int howmany) {
    std::unordered_map<int, Node> new_nodes;
    for(auto node : nodes) {
        node.second.add_to_states(where, howmany, width);
        new_nodes.emplace(node.second.state(), node.second);
    }
    nodes = new_nodes;
    if(previous_state != -1) previous_state = add_to_offset(previous_state, where, howmany, width);
    if(goal != -1) goal = add_to_offset(goal, where, howmany, width);
    if(where == UP || where == DOWN)
        height += howmany;
    else if(where == LEFT || where == RIGHT)
        width += howmany;
}

NodeMap & NodeMap::operator=(const NodeMap &other) {
    goal = other.goal;
    previous_state = other.previous_state;
    width = other.width;
    height = other.height;
    nodes = other.nodes;
    return *this;
}

void NodeMap::merge(NodeMap & other) {
//
// If we don't have anything in our map,
// just copy it in
//
  if(goal == -1) {
    *this = other;
    return;
  };
//
// The first step is to make sure the widths match. So insert
// columns on the right side of whichever map has the smallest
// width.
   if(width < other.width)
     add_to_nodes(RIGHT, other.width - width);
   else if(width > other.width)
     other.add_to_nodes(RIGHT, width - other.width);
//
// Now we can compare apples to apples. Whichever map has a
// goal closer to column zero must be moved over to match
// the other maps goal.
//
   if(goal % width < other.goal % other.width)
     add_to_nodes(LEFT, other.goal - goal);
   else if(goal % width > other.goal % other.width)
     other.add_to_nodes(LEFT, goal - other.goal);
//
// Now one more time we have to make the widths match. Since
// we moved one or the other over, it's probable that that
// maps width is now greater than the other one. Get the
// maximum of both again.
   if(width < other.width)
     add_to_nodes(RIGHT, other.width - width);
   else if(width > other.width)
     other.add_to_nodes(RIGHT, width - other.width);
//
// Now we finally have apples to apples! Both goals should match,
// and both widths should match. We don't really care about height,
// but for correctness, we will go ahead and "add rows to the bottom"
// to match heights.
   if(height < other.height)
     add_to_nodes(DOWN, other.height - height);
   else if(height > other.height)
     other.add_to_nodes(DOWN, height - other.height);

   if(goal != other.goal) throw "What the heck happened?";
   if(width != other.width) throw "Yea, this is bad too";
   if(height != other.height) throw "Not that it matters, but we tried to make them match. Why don't they?";
//
// OK, so now we can finally merge the other map into this map!
//
    for(auto othernode : other.nodes) {
        if(nodes.find(othernode.first) == nodes.end())
          nodes.emplace(othernode.first, othernode.second);
    }
}
