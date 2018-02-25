#include "nodemap.h"
#include "node.h"  // for add_to_offset

bool sortbysec(const std::pair<DIRECTION, int> &a, std::pair<DIRECTION, int> &b) { return a.second < b.second; }

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
// Now we have to move whichever goal has the Y value closest to zero
// down
//
   int t_h = goal/width;
   int o_h = other.goal/width;
   if(t_h < o_h)
     add_to_nodes(UP, o_h - t_h);
   else if(t_h > o_h)
     other.add_to_nodes(UP, t_h - o_h);
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
        else
          nodes.at(othernode.first).addcost(othernode.second.cost());
    }
}

std::vector<int> NodeMap::get_possibles(std::vector<DIRECTION> legalmoves) {
  std::vector<int> possible_states;
  for(auto node : nodes) {
    if(node.second.moves_match(legalmoves))
      possible_states.push_back(node.first);
  };
  return possible_states;
}

std::vector<int> NodeMap::get_next_possibles(std::vector<int> possibles, DIRECTION dir) {
  std::vector<int> newpossibles;
  for(int p : possibles) {
    if(nodes.find(p) != nodes.end()) {
      Node n = nodes.at(p);
      for(NodeMove nm : n.getResultStates()) {
        if(nm.move == dir && nodes.find(nm.result_state) != nodes.end())
          newpossibles.push_back(nm.result_state);
      };
    };
  };
  return newpossibles;
}

std::vector<int> NodeMap::get_next_possibles(std::vector<int> possibles, int legalmovesbyte) {
  std::vector<int> newpossibles;
  for(int p : possibles) {
    if(nodes.find(p) != nodes.end() && nodes.at(p).legalmovesbyte() == legalmovesbyte)
      newpossibles.push_back(p);
  };
  return newpossibles;
}

int NodeMap::buildPossiblesDirection(std::vector<int> possibles, DIRECTION legalmovesbyte, DIRECTION skipdirection) {
  if(possibles.empty()) return -1;
  if(skipdirection) skipdirection = reverse_move(skipdirection);
  possibles_map.push_back(PossibleNode());
  int us = possibles_map.size() - 1;
  for(int d = 1 ; d < 16 ; d <<= 1) {
    if(d != skipdirection && d & legalmovesbyte) {
      std::vector<int> newpossibles = get_next_possibles(possibles, (DIRECTION)d);
      if(!newpossibles.empty()) {
        int dstate = buildPossiblesSquare(newpossibles, (DIRECTION)d);
        if(dstate != -1)
          possibles_map[us].children.push_back(std::pair<DIRECTION, int>((DIRECTION)d, dstate));
      };
    };
  };
  if(!possibles_map[us].children.empty()) {
    sort(possibles_map[us].children);
    if(possibles_map[possibles_map[us].children[0].second].cost != std::numeric_limits<int>::max())
      possibles_map[us].cost = possibles_map[possibles_map[us].children[0].second].cost + 1;
  } else {
    possibles_map.pop_back();
    return -1;
  };
  return us;
}

int NodeMap::buildPossiblesSquare(std::vector<int> possibles, DIRECTION skipdirection) {
  if(possibles.empty()) return -1;

  possibles_map.push_back(PossibleNode());
  int us = possibles_map.size() - 1;

  if(possibles.size() == 1) {
    possibles_map[us].node_location = possibles[0];
    possibles_map[us].cost = 0;
    return us;
  };

  for(int x = 1 ; x < 16 ; x++) {
    std::vector<int> newpossibles = get_next_possibles(possibles, x);
    int dstate = buildPossiblesDirection(newpossibles, (DIRECTION)x, skipdirection);
    if(dstate != -1)
      possibles_map[us].children.push_back(std::pair<DIRECTION, int>((DIRECTION)x, dstate));
  };
  if(!possibles_map[us].children.empty()) {
    sort(possibles_map[us].children);
    if(possibles_map[possibles_map[us].children[0].second].cost != std::numeric_limits<int>::max())
      possibles_map[us].cost = possibles_map[possibles_map[us].children[0].second].cost + 1;
  } else {
    possibles_map.pop_back();
    return -1;
  };
  return us;
}

void NodeMap::buildPossibles() {
  std::vector<int> possibles;
  for(auto it = nodes.begin() ; it != nodes.end() ; ++it)
    possibles.push_back(it->first);
  possibles_root = buildPossiblesSquare(possibles, (DIRECTION)0);
};

void NodeMap::sort(std::vector<std::pair<DIRECTION, int>> &children) {
  std::sort(children.begin(), children.end(), [this](std::pair<DIRECTION, int> left, std::pair<DIRECTION, int> right) {
    return possibles_map[left.second].cost < possibles_map[right.second].cost;
  });
}

int PossibleNode::child(DIRECTION lmb) {
  auto it = std::find_if(children.begin(), children.end(), [lmb] (std::pair<DIRECTION, int> &p) -> bool { return p.first == lmb; });
  if(it == children.end()) return -1;
  return it->second;
}

int PossibleNode::child(std::vector<DIRECTION> legalmoves) {
  int lmb = 0;
  for(DIRECTION d : legalmoves) lmb |= (int)d;
  return child((DIRECTION)lmb);
}

//std::vector<DIRECTION> PossibleNode::moves() {
//  std::vector<DIRECTION> ret;
//  for(std::pair<DIRECTION, int> child : children)
//    ret.push_back(child.first);
//  return ret;
//}
