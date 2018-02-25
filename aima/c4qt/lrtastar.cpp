#include "lrtastar.h"

#include <algorithm>

Square& Square::adjust(DIRECTION d, int howmany) {
  switch(d) {
    case UP:
      y -= howmany;
      break;
    case DOWN:
      y += howmany;
      break;
    case LEFT:
      x -= howmany;
      break;
    case RIGHT:
      x += howmany;
      break;
    default:
      throw "Invalid move";
  };
  return *this;
}

Square Square::move(DIRECTION d, int howmany) const {
  switch(d) {
    case UP:
      return Square(x,y-howmany);
    case DOWN:
      return Square(x,y+howmany);
    case LEFT:
      return Square(x-howmany,y);
    case RIGHT:
      return Square(x+howmany,y);
    default:
      throw "Invalid move";
  };
}

bool LRTAStar::we_are_where_we_think_we_are() {
  if(final_map.empty()) return true;
  if(!final_map.valid()) return false;
  if(!final_state().moves_match(current_state().legalmoves2()))
    return false;
  return true;
}

DIRECTION LRTAStar::traveling_to_nearest_uncharted_territory() {
  return STOP;
}

DIRECTION LRTAStar::lowest_combined_cost_action() {
  std::unordered_map<DIRECTION, int> costs;

  if(!final_map.empty())
    for(DirectionAndState lm : final_map.legalmoves())
      if(final_map.exists(lm.second))
        costs[lm.first] = final_map[lm.second].cost();
      else
        costs[lm.first] = 0;
      
  for(DirectionAndState lm : current_map.legalmoves())
    if(current_map.exists(lm.second))
      costs[lm.first] += current_map[lm.second].cost();
    else
      costs[lm.first] += 0; // This should be useless.

  //int cost = std::numeric_limits<int>::max();
  //DIRECTION dir = STOP;

  auto it = std::min_element(costs.begin(), costs.end(),
        [](const DirectionAndState &a, const DirectionAndState &b) -> bool {return a.second < b.second; }
  );
  return it->first;

//  for(auto it = costs.begin() ; it != costs.end() ; ++it)
//    if(cost > it->second) {
//      cost = it->second;
//      dir = it->first;
//    };
//  return dir;
}

DIRECTION LRTAStar::find_ourselves_online() {
  return STOP;
}

DIRECTION LRTAStar::move_to_closest_unmapped_node() {
  int curstate = current_map.current_state();
  int pstate = -1;
  std::unordered_map<int, std::vector<int>> untried;
  std::queue<int> queue;

  while(true) {
    if(current_map[curstate].uncharted()) {
    };
    if(untried.find(curstate) == untried.end())
      untried[curstate] = current_map.adjacent_nodes(curstate);
    if(pstate != -1)
      queue.push(pstate);
    pstate = curstate;
    if(untried[curstate].empty()) {
      curstate = queue.front();
      queue.pop();
    } else {
      curstate = untried[curstate].front();
      untried[pstate].erase(untried[pstate].begin());
    };
  };
}

void LRTAStar::atgoal(std::vector<DIRECTION> legalmoves) {
  current_map.set_legal_moves(legalmoves);
  current_map.atgoal();
  //
  // Merge current into final, then clear current
  //
  final_map.merge(current_map);
  current_map.clear();

  _atgoal = true;
}

DIRECTION LRTAStar::nextaction(std::vector<DIRECTION> legalmoves) {
  DIRECTION dir = STOP;

  if(_atgoal) {
    _atgoal = false;
    return TELEPORT;
  };

  current_map.set_legal_moves(legalmoves);
  //
  // If we are currently traveling to an uncharted node,
  //   just continue to go there.
  dir = traveling_to_nearest_uncharted_territory();

  //
  // If we think we know where we are,
  //   check to make sure. If we don't, set we don't,
  //   so we can then try to find ourselves.
  //
  // Otherwise, return the LRTA* action
  //
  if(dir == STOP && (final_map.empty() || !final_map.valid())) {
    if(we_are_where_we_think_we_are()) {
      if(current_map.previous_state() != -1)
        update_previous_location_costs();
      dir = lowest_combined_cost_action();
    } else {
      final_map.set_current_state_invalid();
      dir = STOP; // Have no direction if we don't know where we are
    };
  };

  //
  // If we get here, final_map.current_location == UNKNOWN
  // meaning we have no idea where we are.
  //
  // If we can find our current location with the
  //   offline depth first search, then again, return
  //   the best cost move
  //
  if(dir == STOP && we_can_find_ourselves_offline()) {
    dir = lowest_combined_cost_action();
  };

  //
  // If we think we can find ourselves with an online search,
  //   make that move to figure out if we know where we are.
  //
  if(dir == STOP)
    dir = find_ourselves_online();
  //
  // If all else fails, find our way to the closest unmapped node
  //    using a breadth first search. Then go there.
  //
  if(dir == STOP)
    dir = move_to_closest_unmapped_node();

  if(current_map.valid()) current_map.move(dir);
  if(final_map.valid()) final_map.move(dir);

  return dir;
}

bool LRTAStar::we_can_find_ourselves_offline(int possible_state, int cm_state) {

  //
  // Get the square node of the possible state we are in
  //
  PossibleNode node = final_map.possible(possible_state);

  //
  // If it has a node location, set final map's current location to that spot and
  //    return success
  //
  if(node.node_location != -1) {
    final_map.set_current_state(node.node_location);
    return true;
  };

  //
  // If not, get the direction node from the correct child square node
  //
  node = final_map.possible(node.child(current_map.lmb(cm_state)));

  //
  // Loop through our possible directions, finding out if we can get to a known node from any
  //   legal direction. If we can, we adjust final map's current location from the direction
  //   we came from, and return true.
  //
  for(DirectionAndState child : node.children) {
    if(current_map.legalmove(cm_state, child.first) && we_can_find_ourselves_offline(child.second, current_map.state_in_direction(cm_state, child.first))) {
      final_map.move(reverse_move(child.first));
      return true;
    };
  };
  //
  // If we can't find any valid final map state, we return false
  //
  return false;
}

bool LRTAStar::we_can_find_ourselves_offline() {
  return we_can_find_ourselves_offline(final_map.pmroot(), current_map.current_state());
}

int LRTAStar::node_cost(int whichstate) {
  if(!current_map.exists(whichstate)) return 0; // An unmapped node is assumed to be the goal
  return current_map[whichstate].cost();
}

void LRTAStar::update_previous_location_costs() {
  int cost = std::numeric_limits<int>::max();
  if(current_state().adjacent_nodes().empty()) {
    current_state().cost(1); // If we haven't traveled to any nodes, we assume the goal is one step away
    return;
  };
  for(int state : current_state().adjacent_nodes()) {
    int nodecost = node_cost(state);
    if(cost > nodecost) cost = nodecost;
  };
  current_state().cost(cost + 1);
}

void NodeMap::move(DIRECTION d) {
  if(_current_state == -1) throw "Cannot be asking us to move";
  _previous_state = _current_state;
  Node n = _map.at(_current_state);
  int newstate = n.adjacent_node(d);
  if(newstate == -1) {
    int newstate = _map.size();
    _map[newstate].setSquare(_map.at(_current_state).square().move(d));
  };
  _current_state = newstate;
}

bool Node::moves_match(const std::vector<DIRECTION> &legalmoves) const {
  if(_legalmoves_tostate.size() != legalmoves.size()) return false;
  for(DirectionAndState lm : _legalmoves_tostate)
    if(std::find(legalmoves.begin(), legalmoves.end(), lm.first) == legalmoves.end()) return false;
  return true;
}

int Node::legalmove(DIRECTION d) {
  auto it = std::find_if(_legalmoves_tostate.begin(), _legalmoves_tostate.end(), [d](const DirectionAndState &lm) -> bool {return lm.first==d;});
  if(it == _legalmoves_tostate.end()) return -1;
  return it->second;
}

int Node::adjacent_node(DIRECTION d) {
  auto it = std::find_if(_legalmoves_tostate.begin(), _legalmoves_tostate.end(),
       [d] (const DirectionAndState &lm) {return lm.first == d;});
  if(it == _legalmoves_tostate.end()) return -1;
  else return it->second;
}

int NodeMap::find_node_by_square(const Square &square) const {
  for(auto it = _map.begin() ; it != _map.end() ; ++it) {
    if((it->second).square() == square)
      return it->first;
  };
  return -1;
}

void NodeMap::set_legal_moves(std::vector<DIRECTION> legalmoves) {

  if(_map.empty()) {
    _map[0].setSquare(Square(0,0));
    _current_state = 0;
  };

  for(DIRECTION d : legalmoves) {
    Square square = _map[_current_state].square().move(d);
    int lmstate = find_node_by_square(square);
    if(lmstate == -1) {
      lmstate = _map.size();
      _map[lmstate].setSquare(square);
    };
    _map[_current_state].add_legal_move(d, lmstate);
  };
}

std::vector<int> Node::adjacent_nodes() {
  std::vector<int> ret;
  std::transform(_legalmoves_tostate.begin(), _legalmoves_tostate.end(),
                 std::back_inserter(ret),
                 [](const DirectionAndState &lm) {return lm.second;});
  return ret;
}

void NodeMap::merge(std::vector<int> &explored, NodeMap & other, int ours, int theirs) {
  explored.push_back(ours);

  _map[ours].cost(_map[ours].cost() + other._map[theirs].cost());

  for(DirectionAndState lm : other._map[theirs].legalmoves()) {
    int s1 = _map[ours].legalmove(lm.first);
    if(s1 == -1) {
      Square square = _map[ours].square().move(lm.first);
      s1 = find_node_by_square(square);
      if(s1 == -1) {
        s1 = _map.size();
        _map[s1].setSquare(square);
      };
      _map[ours].add_legal_move(lm.first, s1);
    };
    if(std::find(explored.begin(), explored.end(), s1) == explored.end()) {
      merge(explored, other, s1, lm.second);
    };
  }
}

void NodeMap::merge(NodeMap & other) {
  std::vector<int> explored;
  if(_map.empty()) {
    _goal_state = 0;
    _map[0].setSquare(Square(0,0));
  };
  merge(explored, other, _goal_state, other._goal_state);
  possibles_map.clear();
  buildPossibles();
}

void NodeMap::clear() {
  /*_width = _height = */ _current_state = _previous_state = _goal_state = -1;
  _map.clear();
}

int NodeMap::buildPossiblesDirection(const std::vector<int> &possibles, DIRECTION legalmovesbyte, DIRECTION skipdirection) {
  if(possibles.empty()) return -1;
  if(skipdirection) skipdirection = reverse_move(skipdirection);
  possibles_map.push_back(PossibleNode());
  int us = possibles_map.size() - 1;

  for(int d = 1 ; d < 16 ; d <<= 1) {
    if((d != skipdirection) && (d & legalmovesbyte)) {
      std::vector<int> newpossibles = get_next_possibles_in_direction(possibles, (DIRECTION)d);
      if(!newpossibles.empty()) {
        int dstate = buildPossiblesSquare(newpossibles, (DIRECTION)d);
        if(dstate != -1)
          possibles_map[us].children.push_back(DirectionAndState((DIRECTION)d, dstate));
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

int NodeMap::buildPossiblesSquare(const std::vector<int> &possibles, DIRECTION skipdirection) {
  if(possibles.empty()) return -1;

  possibles_map.push_back(PossibleNode());
  int us = possibles_map.size() - 1;

  if(possibles.size() == 1) {
    possibles_map[us].node_location = possibles[0];
    possibles_map[us].cost = 0;
    return us;
  };

  for(int x = 1 ; x < 16 ; x++) {
    std::vector<int> newpossibles = get_next_possibles_with_matching_walls(possibles, (DIRECTION)x);
    int dstate = buildPossiblesDirection(newpossibles, (DIRECTION)x, skipdirection);
    if(dstate != -1)
      possibles_map[us].children.push_back(DirectionAndState((DIRECTION)x, dstate));
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
  std::transform(_map.begin(), _map.end(),
                 std::back_inserter(possibles),
                 [] (const std::pair<int, Node> &p) { return p.first; });
  possibles_root = buildPossiblesSquare(possibles, (DIRECTION)0);
};

const std::vector<int> NodeMap::get_possibles(const std::vector<DIRECTION> &legalmoves) const {
  std::vector<int> possible_states;
  for(auto node : _map) {
    if(node.second.moves_match(legalmoves))
      possible_states.push_back(node.first);
  };
  return possible_states;
}

const std::vector<int> NodeMap::get_next_possibles_in_direction(const std::vector<int> &possibles, DIRECTION dir) const {
  std::vector<int> newpossibles;
  for(int p : possibles) {
    if(exists(p)) {
      auto it = std::find_if(_map.at(p).legalmoves().begin(), _map.at(p).legalmoves().end(),
                [dir](const DirectionAndState &lm){return lm.first==dir;});
      if(it != _map.at(p).legalmoves().end() && exists(it->second))
        newpossibles.push_back(it->second);
    };
  };
  return newpossibles;
}

const std::vector<int> NodeMap::get_next_possibles_with_matching_walls(const std::vector<int> &possibles, DIRECTION lmb) const {
  std::vector<int> newpossibles;
  for(int p : possibles)
    if(exists(p) && _map.at(p).lmb() == lmb)
      newpossibles.push_back(p);
  return newpossibles;
}

void NodeMap::sort(std::vector<DirectionAndState> &children) {
  std::sort(children.begin(), children.end(), [this](DirectionAndState left, DirectionAndState right) {
    return possibles_map[left.second].cost < possibles_map[right.second].cost;
  });
}

const PossibleNode &NodeMap::possible(int state) const {
  return possibles_map.at(state);
}

int PossibleNode::child(DIRECTION d) {
  auto it = std::find_if(children.begin(), children.end(), [d](const DirectionAndState & c){return c.first==d;});
  if(it == children.end()) return -1;
  else return it->second;
}


int NodeMap::legalmove(int state, DIRECTION d) {
  return _map.at(state).legalmove(d);
}

int NodeMap::state_in_direction(int state, DIRECTION d) const {
  return _map.at(state).state_in_direction(d);
}

int Node::state_in_direction(DIRECTION d) const {
  std::vector<DirectionAndState> _legalmoves_tostate;
  auto it = std::find_if(_legalmoves_tostate.begin(), _legalmoves_tostate.end(),
                         [d](const DirectionAndState &c){return c.first == d; });
  if(it == _legalmoves_tostate.end()) return -1;
  else return it->second;
}

std::vector<DIRECTION> Node::legalmoves2() const {
  std::vector<DIRECTION> ret;
  std::transform(_legalmoves_tostate.begin(), _legalmoves_tostate.end(),
                 std::back_inserter(ret),
                 [](const DirectionAndState &lm) {return lm.first;});
  return ret;
}
