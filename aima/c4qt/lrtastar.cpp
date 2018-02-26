#include "lrtastar.h"

#include <algorithm>
#include <iostream>
#include <unordered_map>

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
  if(current_state().uncharted()) return true;
  if(!final_map.valid()) return false;
  if(final_current_state().moves_match(current_state().legalmoves2())) return true;
  return false;
}

DIRECTION LRTAStar::traveling_to_nearest_uncharted_territory() {
  if(!uncharted_travel.size()) return STOP;
  DIRECTION d = uncharted_travel.front();
  uncharted_travel.pop();
  return d;
}

DIRECTION LRTAStar::lowest_combined_cost_action() {
  std::unordered_map<DIRECTION, int> costs;

  if(!final_map.empty())
    for(DirectionAndState lm : final_map.legalmoves())
      if(final_map.exists(lm.second))
        costs[lm.first] = final_map[lm.second].cost();
      
  for(DirectionAndState lm : current_map.legalmoves())
    if(costs[lm.first] != std::numeric_limits<int>::max()) {
      if(current_map.exists(lm.second) && costs.find(lm.first) != costs.end())
        costs[lm.first] += current_map[lm.second].cost();
    };

  auto it = std::min_element(costs.begin(), costs.end(),
        [](const DirectionAndState &a, const DirectionAndState &b) -> bool {return a.second < b.second; }
  );

  if(it == costs.end())
    return current_state().legalmoves().at(0).first; // If we don't have any good costs, just return the first available
  else
    return it->first;
}

DIRECTION LRTAStar::find_ourselves_online() {
  return STOP;
}

DIRECTION LRTAStar::move_to_closest_unmapped_node() {
//
// What I wanted was a breadth first search to find the closest
// unmapped node. But BFS doesn't really maintain actual path
// information. So instead I am just going to use an interative
// depth first search. Regardless of the size of the maze, the
// depth should be minimal if we get here. We will only get here
// if we can't find ourselves in final_map at all. The only way to
// have any depth at all is for the online search routine to try to
// find itself and fail. Then we will have to travel from wherever we
// are to the nearest uncharted node.
//

  int curstate = current_map.current_state();
  unsigned int max_depth = 1;
  std::unordered_map<int, std::vector<DirectionAndState>> untried;
  std::vector<DirectionAndState> vec;

  while(true) {
    if(current_map[curstate].uncharted()) {
      for(auto it = vec.begin() ; it != vec.end() ; ++it)
        uncharted_travel.push(it->first);
      DIRECTION dir = uncharted_travel.front();
      uncharted_travel.pop();
      return dir;
    };

//
// So yea, this will result in an endless loop if there are no uncharted nodes
// In the words of all wise souls that have come before me, "That will never happen" :)
// If it is ever a problem for any reason, I would just add in some type of
// hard coded depth limit like "if new max depth==100 then throw a WTF error"
//
doagain:
    if(vec.size() != max_depth && untried.find(curstate) == untried.end())
      untried[curstate] = current_map[curstate].legalmoves();

    while(vec.size() == max_depth || untried[curstate].empty()) {
      if(vec.empty()) {
        untried.clear();
        max_depth++;
        curstate = current_map.current_state();
        goto doagain;
      };
      DirectionAndState ds = vec.back();
      vec.pop_back();
      curstate = ds.second;
    };

    DirectionAndState newstate = untried[curstate].front();
    untried[curstate].erase(untried[curstate].begin());
    vec.push_back(DirectionAndState(newstate.first, curstate));
    curstate = newstate.second;
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

  int uncharted = false;
  if(current_map.previous_state() == -1 || current_map.uncharted()) {
    current_map.set_legal_moves(legalmoves);
    uncharted = true;
  };
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
  if(dir == STOP && (final_map.empty() || final_map.valid())) {
    if(we_are_where_we_think_we_are()) {
      if(current_map.previous_state() != -1)
        update_previous_location_costs();
      dir = lowest_combined_cost_action();
    } else {
      final_map.set_current_state_invalid();
      current_map.reset_costs();
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
  if(dir == STOP && uncharted && we_can_find_ourselves_offline()) {
    dir = lowest_combined_cost_action();
  };

  //
  // If we think we can find ourselves with an online search,
  //   make that move to figure out if we know where we are.
  //
  if(dir == STOP && uncharted)
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
  //std::cout << "find ourselves, possible_state=" << possible_state << ", cm_state=" << cm_state << std::endl;
  //
  // Get the square node of the possible state we are in
  //
  PossibleNode node = final_map.possible(possible_state);

  //
  // If it has a node location, set final map's current location to that spot and
  //    return success
  //
  if(node.node_location != -1) {
    if(final_map[node.node_location].uncharted()) // It's in the final map, but we haven't actually been in that node, so it doesn't have any legal moves yet
      return false;
    //std::cout << "We have found our final location, setting to " << node.node_location << " and returning true" << std::endl << std::flush;
    final_map.set_current_state(node.node_location);
    return true;
  };

  //
  // If we are in an uncharted node, then obviously we cannot continue to
  //    find ourselves.
  //
  if(current_map[cm_state].uncharted()) {
    //std::cout << "The current node at " << cm_state << " is uncharted, so we return false" << std::endl << std::flush;
    return false;
  }

  //
  // If not, get the direction node from the correct child square node
  //
  if(node.child(current_map.lmb(cm_state)) == -1) {
    //std::cout << " There is no square node in this possibles node that matches our legal moves, so we return false" << std::endl;
    return false;
  };
  node = final_map.possible(node.child(current_map.lmb(cm_state)));

  //
  // Loop through our possible directions, finding out if we can get to a known node from any
  //   legal direction. If we can, we adjust final map's current location from the direction
  //   we came from, and return true.
  //
  for(DirectionAndState child : node.children) {
    //std::cout << "We are trying to move " << child.first << " from state " << cm_state << " to state " << current_map.state_in_direction(cm_state, child.first) << std::endl << std::flush;
    if(/*current_map.legalmove(cm_state, child.first) && */ we_can_find_ourselves_offline(child.second, current_map.state_in_direction(cm_state, child.first))) {
      //std::cout << " Moving " << child.first << " from state " << cm_state << " to state worked, so we are moving final map in the reverse direction" << std::endl << std::flush;
      final_map.move(reverse_move(child.first));
      return true;
    };
  };
  //
  // If we can't find any valid final map state, we return false
  //
  //std::cout << "We were unable to find any path with possible_state=" << possible_state << " and cm_state=" << cm_state << ", so we return false" << std::endl << std::flush;
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
  std::unordered_map<DIRECTION, int> costs;

  if(!previous_state().adjacent_nodes().empty()) {
    for(DirectionAndState lm : previous_state().legalmoves()) {
      costs[lm.first] = current_map[lm.second].cost();
    };
  };

  if(final_map.valid()) {
    for(DirectionAndState lm : final_previous_state().legalmoves()) {
      costs[lm.first] += final_map[lm.second].cost();
    };
  };

  int cost = std::numeric_limits<int>::max();
  for(std::pair<DIRECTION, int> ucost : costs)
    if(ucost.second < cost) cost = ucost.second;

  cost++; // Assume we are one square (further) away from the goal
  //
  // Here, we are adding an efficiency. There is no way that the goal
  //  is closer than the manhattan distance to the known goal, if we
  //  are where we think we are in the final map. Thus, if the calculated
  //  cost is less than the manhattan distance, return that instead.
  //
  // Also, in case it's unclear, we maintain two maps. The "final" map,
  //  an accumulation of everything we have found and costs we have accumulated
  //  in previous runs, and the "current" run of the nodes we are finding or
  //  re-costing in this run. We have to keep them separate because when we
  //  teleport, we don't know where we are, and even more importantly, it's
  //  very probable that early on we will think we know where we are only to
  //  realize we were wrong, and have to start over figuring out where we are.
  //  In those cases, the current maps costs get reset to zero, and we start
  //  the recalulations over.
  //
  // Otherwise, costs are calculated here, and in the "lowest cost move" method,
  //  by adding up the final nodes cost and the current nodes cost. Any updates
  //  to the cost are applied only to the current nodes cost. When the current map
  //  gets merged with the final map, we add the current maps cost to the final
  //  map. You can see these additions here as well as in the lowest cost move
  //  method.
  //
  if(final_map.valid()) {
    int distance = final_map.manhattan_distance_to_goal();
    if(cost < distance) cost = distance;
    cost -= final_previous_state().cost();
    if(previous_state().cost() != cost)
      std::cout << "Updating cost from " << previous_state().cost() << " to " << cost << std::endl << std::flush;
    previous_state().cost(cost);
  } else {
    if(previous_state().cost() != cost)
      std::cout << "Updating cost from " << previous_state().cost() << " to " << cost << std::endl << std::flush;
    previous_state().cost(cost);
  };
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

void NodeMap::reset_costs() {
  for(auto it = _map.begin() ; it != _map.end() ; ++it) it->second.cost(0);
}

int NodeMap::manhattan_distance_to_goal() {
  if(_goal_state == -1) return 0;
  if(_current_state == -1) return 0;
  Square g = _map[_goal_state].square();
  Square c = _map[_current_state].square();
  return std::abs(g.x - c.x) + std::abs(g.y - c.y);
}
