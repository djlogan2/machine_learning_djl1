#include "agent.h"

#include <algorithm>
#include <iostream>
#include <limits>

Wumpus::Wumpus() {
  current_square = XY(0,0);
  _map[current_square].wumpus = _map[current_square].pit = PNO;
  _map[current_square].wall = WNO;
  wumpus_dead = false;
  previous_action = NONE;
  have_gold = false;
  have_arrow = true;
  found_wumpus = false;
}

void Wumpus::clear_all_other_wumpuses() {
  for(auto it = _map.begin(); it != _map.end() ; ++it)
    if((it->second).wumpus != YES) (it->second).wumpus = PNO;
}

void Wumpus::update_node(const XY &square) {
  int wcount = 0;
  XY _wumpus_square;
  int pcount = 0;
  XY _pit_square;

  if(_map.find(square) == _map.end()) return;

  for(DIRECTION d = DUP ; d <= DLEFT ; d = (DIRECTION)((int)d + 1)) {
    XY check = square.adjacent(d);
    if(_map.find(check) == _map.end()) {
      return;
    } else if(_map[check].wall != VERTICAL && _map[check].wall != HORIZONTAL) {
      if(_map[check].pit == MAYBE) {
         pcount++; _pit_square = check;
      };
      if(_map[check].wumpus == MAYBE) {
         wcount++; _wumpus_square = check;
      };
    };
  };

  if(pcount == 1) {
    _map[_pit_square].pit = YES;
  };

  if(wcount == 1) {
    _map[_wumpus_square].wumpus = YES;
    _map[_wumpus_square].pit = PNO;
    found_wumpus = true;
    wumpus_square = _wumpus_square;
    update_adjacent_nodes(_wumpus_square);
  };
}

void Wumpus::update_adjacent_nodes(const XY &square) {
  for(DIRECTION d = DUP ; d <= DLEFT ; d = (DIRECTION)((int)d + 1)) {
    update_node(square.adjacent(d));
  };
}

void Wumpus::update_node(const XY &square, const Sensor &sensor) {
  POSSIBLE w = _map[square].wumpus;
  POSSIBLE p = _map[square].pit;

  if(!is_wall(square)) {
    if(sensor & STENCH) {
      if(found_wumpus && wumpus_square != current_square) {
       if(_map[wumpus_square].wumpus != YES && _map[square].wumpus == MAYBE) {
         _map[square].wumpus = YES;
         _map[square].pit = PNO;
         wumpus_square = square;
         clear_all_other_wumpuses();
         update_adjacent_nodes(square);
       };
      } else if(_map[square].wumpus != PNO) {
        _map[square].wumpus = MAYBE;
      };
    } else {
      _map[square].wumpus = PNO;
    };
    if(sensor & BREEZE) {
      if(_map[square].pit == PUNKNOWN)
        _map[square].pit = MAYBE;
    } else {
      _map[square].pit = PNO;
    };
  };
  if(w != _map[square].wumpus || p != _map[square].pit) {
    update_adjacent_nodes(square);
  };
}

void Wumpus::update_adjacent_nodes(const XY &square, const Sensor &sensor) {
  for(DIRECTION d = DUP ; d <= DLEFT ; d = (DIRECTION)((int)d + 1)) {
    update_node(square.adjacent(d), sensor);
  };
}

bool Wumpus::is_wall(const XY &square) {
  if(_map.find(square) != _map.end() && (_map[square].wall == VERTICAL || _map[square].wall == HORIZONTAL)) return true;
  auto it = std::find_if(_map.begin(), _map.end(),
     [square](std::pair<const XY, const AgentSquare> sq)
     {return (sq.second.wall == HORIZONTAL && sq.first.second == square.second) ||
             (sq.second.wall == VERTICAL && sq.first.first == square.first);
     });
  if(it == _map.end()) return false;
  _map[square].wall = (it->second).wall;
  _map[square].pit = _map[square].wumpus = PNO;
  return true;
}

void Wumpus::update_walls() {
  bool vertical = (current_direction == DLEFT || current_direction == DRIGHT);

  for(auto it = _map.begin() ; it != _map.end(); ++it) {
    if((vertical && (it->first).first == current_square.first) ||
       (!vertical && (it->first).second == current_square.second)) {
         (it->second).wall = (vertical ? VERTICAL : HORIZONTAL);
         (it->second).pit = (it->second).wumpus = PNO;
         update_adjacent_nodes(it->first);
    };
  };
}

void Wumpus::tell(const Sensor &sensor) {

  if(sensor & WON) {
    std::cout << "We won!" << std::endl << std::flush;
  } else if(sensor & DIED) {
    std::cout << "We died!" << std::endl << std::flush;
  };

  if(sensor & BUMP) { // So obviously if we get a bump, we were already on the square
                      // So we aren't going to update with breezes and stenches
    update_walls();
    current_square.reverse(current_direction);
  } else {
    _map[current_square].wumpus = _map[current_square].pit = PNO;
    _map[current_square].wall = WNO;
    update_adjacent_nodes(current_square, sensor);
    if((sensor & STENCH) && !found_wumpus) {
      wumpus_square = current_square;
      found_wumpus = true;
    };
  };

  if(sensor & SCREAM) {
    wumpus_dead = true;
  };

  std::vector<int> previous_nodes;
  nodelist.clear();

  if(sensor & GLITTER) {
    previous_nodes.push_back(0);
    have_gold = true;
    int node = -1;
    for(int depth = 2 ; nodelist.empty() || node == -1 || nodelist.at(node).cost <= 0 ; depth++) {
      nodelist.clear();
      nodelist.emplace(0, Node(current_square, current_direction, have_arrow));
      nodelist.at(0).cost = -1;
      nodelist.at(0).parent_action = NONE;
      node = get_node(GRAB, previous_nodes, depth);
    };
    nodelist.at(0).children.emplace(GRAB, node);
    nodelist.at(0).cost = nodelist.at(0).cost - 1; // Not that this matters, as it's the only legal move
    nodelist.at(0).best_action = GRAB;
  } else {
    if(have_gold) {
      for(int depth = 2 ; nodelist.empty() || nodelist.at(0).cost <= 0 ; depth++) {
        nodelist.clear();
        get_node(NONE, previous_nodes, depth);
      };
    } else
      get_node(NONE, previous_nodes); // Should return node 0 as the root node
  };
}

bool Wumpus::prune_me(int cost, const std::vector<int> &previous_nodes, int index) {
  if(index < 0) {
    return false;
  };
  cost -= (nodelist.at(previous_nodes[index]).parent_action == SHOOT ? 10 : 1);
  if(cost < nodelist.at(previous_nodes[index]).cost) {
    return true;
  } else return prune_me(cost, previous_nodes, --index);
}

int Wumpus::get_node(const ACTION &action, std::vector<int> &previous_nodes, int depth) {

  if(!depth)
    return -1;

  XY newsquare = current_square;
  DIRECTION newdirection = current_direction;

  bool new_arrow = have_arrow;
  int previous_node = -1;
  int current_cost = std::numeric_limits<int>::min();
  //
  // First get the information from the previous node
  //
  if(!previous_nodes.empty()) {
    previous_node = previous_nodes.back();
    newsquare = nodelist.at(previous_node).square;
    newdirection = nodelist.at(previous_node).direction;
    new_arrow = nodelist.at(previous_node).have_arrow;
    current_cost = nodelist.at(previous_node).cost - (action == SHOOT ? 10 : 1);
    if(prune_me((have_gold ? 1000 : 0) - (action == SHOOT ? 10 : 1), previous_nodes, previous_nodes.size()-1))
      return -1;
  };


  if(action == SHOOT) have_arrow = false;

  //
  // Now update the square if we made some type of move
  //
  switch(action) {
    case FORWARD:
      newsquare.move(nodelist.at(previous_node).direction);
      break;
    case LEFT:
      newdirection = direction_left(newdirection);
      break;
    case RIGHT:
      newdirection = direction_right(newdirection);
    default:
      break;
  };

  //
  // Now if we have already been to this square,
  //   return no node for this action
  //
  for(int i : previous_nodes) {
    if(nodelist.at(i).parent_action == action &&
       nodelist.at(i).square == newsquare &&
       nodelist.at(i).direction == newdirection &&
       nodelist.at(i).have_arrow == new_arrow &&
       nodelist.at(i).best_action != GRAB)
      return -1;
  };

  //
  // Create the new node
  //
  int retnode = nodelist.size();
  nodelist.emplace(retnode, Node(newsquare, newdirection, new_arrow));
  nodelist.at(retnode).cost = std::numeric_limits<int>::min();
  nodelist.at(retnode).parent_action = action;
  //
  // This rule isn't in the book, but I had to make a choice. Either we have to
  //   limit costs to prevent an endless loop, or we have to simply return an
  //   awful score if we are going to bump into a wall. I chose the latter,
  //   as you can see from above that we don't allow a tree to contain repeating
  //   nodes. The alternative was to bump and stay, increasing the cost, until
  //   the cost was prohobitive as compare to other tree paths. I didn't do that.
  //
  if(_map[newsquare].wall == HORIZONTAL || _map[newsquare].wall == VERTICAL) {
    nodelist.at(retnode).cost = -2000;
    return retnode;
  } else
  //
  // If we know we have a wumpus or pit, don't even consider going to that square
  //
  if((_map[newsquare].wumpus == YES && !wumpus_dead) || _map[newsquare].pit == YES) {
    nodelist.erase(retnode);
    return -1;
  } else
  //
  // If we might run into a live wumpus or run into a pit, this is a final node
  //   with a bad score. Notice we would rather do this than bump into a wall! :)
  //
  if((_map[newsquare].wumpus != PNO && !wumpus_dead) || _map[newsquare].pit != PNO) {
    nodelist.at(retnode).cost = -1000;
    return retnode;
  } else
  //
  // If we have the gold and we are at the starting square, it's the winning cost
  //
  if(have_gold && newsquare == XY(0,0) && action == CLIMB) {
    nodelist.at(retnode).cost = 1000;
    return retnode;
  } else
  //
  // A climb action either without the gold or without being at the start is simply
  //   not allowed.
  //
  if(action == CLIMB) {
    nodelist.erase(retnode);
    return -1;
  } else
  //
  // If this square isn't in the map, add the node
  //  as a final node, and return
  //  But only if we haven't found the gold. If we have,
  //  we need to find the path back to the start.
  //
  if(!have_gold && _map.find(newsquare) == _map.end()) {
    return retnode;
  } else
  //
  // If we haven't visited a square, it's wall status will be unknown.
  // A bit of a logic flaw, since our map should be able to update all
  // known verticals and horizontals to not be walls when it knows they
  // aren't, but it happens to help us return a destination square.
  //
  // Without this addition, once it has explored, it returns -1000 or
  // worse because all it can do is find pits or wumpuses to run into.
  //
  if(!have_gold && _map[newsquare].wall == WUNKNOWN) {
    nodelist.at(retnode).cost = 0;
    return retnode;
  //
  // OK, once we get here, we can: LEFT, RIGHT, FORWARD, or SHOOT.
  // GRAB is handled at the root, so we don't have to worry about that.
  //
  } else {
    std::vector<ACTION> actions = {FORWARD, LEFT, RIGHT};
    int n;
    previous_nodes.push_back(retnode);
    if(have_gold && newsquare == XY(0,0)) {
      n = get_node(CLIMB, previous_nodes, (depth == -1 ? depth : depth - 1));
      if(n != -1) {
        nodelist.at(retnode).children[CLIMB] = n;
        int newcost = nodelist.at(n).cost - (action == SHOOT ? 10 : 1);
        if(newcost > nodelist.at(retnode).cost) {
          nodelist.at(retnode).cost = newcost;
          nodelist.at(retnode).best_action = CLIMB;
        };
      };
    };
    for(ACTION a : actions) {
      n = get_node(a, previous_nodes, (depth == -1 ? depth : depth - 1));
      if(n != -1) {
        nodelist.at(retnode).children[a] = n;
        int newcost = nodelist.at(n).cost - (action == SHOOT ? 10 : 1);
        if(newcost > nodelist.at(retnode).cost) {
          nodelist.at(retnode).cost = newcost;
          nodelist.at(retnode).best_action = a;
        };
      };
    };
    if(have_arrow && pointing_at_wumpus()) {
      n = get_node(SHOOT, previous_nodes, (depth == -1 ? depth : depth - 1));
      if(n != -1) {
        nodelist.at(retnode).children[SHOOT] = n;
        int newcost = nodelist.at(n).cost - (action == SHOOT ? 10 : 1);
        if(newcost > nodelist.at(retnode).cost) {
          nodelist.at(retnode).cost = newcost;
          nodelist.at(retnode).best_action = SHOOT;
        };
      };
    };
    previous_nodes.pop_back();
  };

  if(nodelist.at(retnode).children.empty()) {
    nodelist.erase(retnode);
    return -1;
  };

  return retnode;
}

ACTION Wumpus::ask() {
  previous_action = nodelist.at(0).best_action;
  switch(previous_action) {
      default:
          break;
    case FORWARD:
      current_square.move(current_direction);
      break;
    case LEFT:
      current_direction = direction_left(current_direction);
      break;
    case RIGHT:
      current_direction = direction_right(current_direction);
      break;
  };
  return previous_action;
}

bool Wumpus::pointing_at_wumpus() {
  if(!found_wumpus) return false;
  if(_map[wumpus_square].wumpus != YES) return false;
  return (current_square.which_direction(wumpus_square) == current_direction);
}

void Wumpus::print_node(int i) {
  if(i < nodelist.size())
    std::cout << "Node " << i << ": " << nodelist.at(i) << std::endl << std::flush;
  else
    std::cout << "Node " << i << " out of range" << std::endl << std::flush;
}

void Wumpus::print_node() {
  for(int i = 0 ; i < nodelist.size(); i++)
    print_node(i);
}

std::string dir(DIRECTION d) {
  switch(d) {
    case DUP: return "UP";
    case DRIGHT: return "RIGHT";
    case DDOWN: return "DOWN";
    case DLEFT: return "LEFT";
  };
}

std::string act(ACTION a) {
  switch(a) {
    case NONE:    return "NONE";
    case FORWARD: return "FORWARD";
    case LEFT:    return "LEFT";
    case RIGHT:   return "RIGHT";
    case CLIMB:   return "CLIMB";
    case SHOOT:   return "SHOOT";
    case GRAB:    return "GRAB";
  };
}

std::ostream &operator<<(std::ostream &os, const Node &node) {
    os << "Node[" << node.square << "] direction=" << dir(node.direction) << ", have_arrow=" << node.have_arrow << ", cost=" << node.cost << ", best_action=" << act(node.best_action) << ", children=[";
    for(auto it = node.children.begin() ; it != node.children.end() ; ++it) {
      os << '[' << act(it->first) << ',' << it ->second << ']';
    };
    os << ']';
    return os;
};

Node::Node(const XY &square, DIRECTION direction, bool have_arrow) {
  this->square = square;
  this->direction = direction;
  this->have_arrow = have_arrow;
  cost = std::numeric_limits<int>::min();
  best_action = NONE;
}

#define POSS_CHAR(P) (P == PUNKNOWN ? 'u' : (P == MAYBE ? '?' : (P == YES ? '!' : 'N')))
#define WALL_CHAR(P) (P == WUNKNOWN ? 'u' : (P == WNO ? 'N' : (P == VERTICAL ? 'V' : 'H')))

void Wumpus::print_map() {
  XY min(std::numeric_limits<int>::max(),std::numeric_limits<int>::max());
  XY max(std::numeric_limits<int>::min(),std::numeric_limits<int>::min());

  for(auto it = _map.begin() ; it != _map.end() ; ++it) {
    min.first = std::min((it->first).first, min.first);
    min.second = std::min((it->first).second, min.second);
    max.first = std::max((it->first).first, max.first);
    max.second = std::max((it->first).second, max.second);
  };
  for(int y = min.second ; y <= max.second ; y++) {
    for(int x = min.first ; x <= max.first ; x++) {
      if(_map.find(XY(x,y)) == _map.end()) {
        std::cout << "......  ";
      } else {
        std::cout << 'W' << POSS_CHAR(_map[XY(x,y)].wumpus);
        std::cout << 'P' << POSS_CHAR(_map[XY(x,y)].pit);
        std::cout << 'w' << WALL_CHAR(_map[XY(x,y)].wall);
        std::cout << (XY(x,y) == current_square ? "* " : "  ");
      };
    };
    std::cout << std::endl;
  };
  std::cout << "Facing " << dir(current_direction) << std::endl;
  std::cout << std::flush;
}
