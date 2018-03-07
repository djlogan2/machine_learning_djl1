#ifndef __AGENT_H__
#define __AGENT_H__

#include "map.h"

#include <limits>
#include <unordered_map>
#include <vector>

namespace std {
  template<> struct hash<XY> {
    inline size_t operator()(const XY &key) const {return hash<int>()(key.first << 16 | key.second); }
  };
  template<> struct hash<ACTION> {
    inline size_t operator()(const ACTION &key) const {return hash<int>()((int)key); }
  };
};

enum POSSIBLE {
  PUNKNOWN,
  MAYBE,
  YES,
  PNO
};

enum WALL {
  WUNKNOWN,
  WNO,
  VERTICAL,
  HORIZONTAL
};

struct AgentSquare {
  POSSIBLE wumpus;
  POSSIBLE pit;
  WALL wall;
};

class Agent {
public:
  virtual void tell(const Sensor &sensor) = 0;
  virtual ACTION ask() = 0;
};

struct Node {
  Node(const XY &square, DIRECTION direction, bool have_arrow);
  ACTION parent_action;
  XY square;
  DIRECTION direction;
  bool have_arrow;
  int cost;
  ACTION best_action;
  std::unordered_map<ACTION, int> children;
  friend std::ostream &operator<<(std::ostream &os, const Node &node);
};

class Wumpus : public Agent {
public:
  Wumpus();
  virtual void tell(const Sensor &sensor);
  virtual ACTION ask();
  void print_node();
  void print_node(int i);
  void print_map();

protected:
  DIRECTION current_direction = DRIGHT;
  std::unordered_map<XY, AgentSquare> _map;
  XY current_square;
  XY previous_square;
  XY wumpus_square;
  ACTION previous_action;
  bool wumpus_dead;
  bool have_gold;
  bool have_arrow;
  bool found_wumpus;

  std::unordered_map<int, Node> nodelist;

  void update_node(const XY &square);
  void update_adjacent_nodes(const XY &square);
  void update_node(const XY &square, const Sensor &sensor);
  void update_adjacent_nodes(const XY &square, const Sensor &sensor);
  void clear_all_other_wumpuses();
  bool is_wall(const XY &square);
  void update_walls();
  int get_node(const ACTION &action, std::vector<int> &previous_nodes, int depth=-1);
  bool pointing_at_wumpus();
  bool prune_me(int cost, const std::vector<int> &previous_nodes, int index);
};

#endif // __AGENT_H__
