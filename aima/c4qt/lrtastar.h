#ifndef __LRTASTAR_H__
#define __LRTASTAR_H__

#include "ai.h"
#include "direction.h"

#include <algorithm>
#include <queue>
#include <unordered_map>

typedef std::pair<DIRECTION, int> DirectionAndState;

//#####################################################################
class Square {
public:
  Square() { x = -1 ; y = -1; }
  Square(const int x, const int y) { this->x = x ; this->y = y; }
  int x;
  int y;
  bool operator==(const Square& other) const { return x == other.x && y == other.y ; }
  bool operator!=(const Square& other) const { return x != other.x || y != other.y ; }
  Square &operator=(const Square& other) { x = other.x ; y = other.y ; return *this; }
  Square operator+(const Square& other) const { return Square(x+other.x, y+other.y); }
  Square &operator+=(const Square& other) { x += other.x ; y += other.y; return *this; }
  Square operator-(const Square& other) const { return Square(x-other.x, y-other.y); }
  Square move(DIRECTION d, int howmany=1) const;
  Square &adjust(DIRECTION d, int howmany=1);
};
const Square UNKNOWN(-1,-1);
//#####################################################################
namespace std {
//  template<> struct hash<Square> {
//    size_t operator()(const Square& square) const { return hash<int>()(((int)square.y << 16) + (int)square.x); }
//  };
//  template<> struct hash<pair<DIRECTION, int>> {
//    size_t operator()(const pair<DIRECTION, int> &p) const {return hash<int>()(p.second << 8 | p.first); }
//  };
  template<> struct hash<DIRECTION> {
    size_t operator()(const DIRECTION &d) const {return hash<int>()((int)d); }
  };
};
//#####################################################################
class Node {
public:
  int cost() const { return _cost; }
  void cost(int value) { _cost = value; }

  void setSquare(const Square &square) {_square = square; }
  const Square&square() const { return _square; }

  int adjacent_node(DIRECTION d);
  std::vector<int> adjacent_nodes();

  void add_legal_move(DIRECTION d, int tostate) { _legalmoves_tostate.push_back(DirectionAndState(d, tostate)); }
  const std::vector<DirectionAndState> &legalmoves() const { return _legalmoves_tostate; }
  std::vector<DIRECTION> legalmoves2() const;

  DIRECTION lmb() const {int d = 0 ; for(auto lm : _legalmoves_tostate) d |= (int)lm.first; return (DIRECTION)d; }
  int legalmove(DIRECTION d);

  int state_in_direction(DIRECTION d) const;
  bool moves_match(const std::vector<DIRECTION> &legalmoves) const;
  bool uncharted() const { return _legalmoves_tostate.empty(); } // We haven't been to this node yet if there are no legal moves
  //void shift(const Square &square) { _square += square; }
protected:
  Square _square;
  int _cost;
  std::vector<DirectionAndState> _legalmoves_tostate;
};
//#####################################################################
class PossibleNode {
public:
  PossibleNode() { cost = std::numeric_limits<int>::max(); node_location = -1; }
  int cost;
  int node_location;
  std::vector<DirectionAndState> children;
  int child(DIRECTION d);
  int child(std::vector<DIRECTION> legalmoves);
};
//#####################################################################
class NodeMap {
public:
  NodeMap() { clear(); }
  int previous_state() const { return _previous_state; }
  int current_state() const { return _current_state; }
  void set_current_state_invalid() { _current_state = -1; }
  void set_current_state(const int state) { _current_state = state; }
  int goal() const { return _goal_state; }

  Node &operator[](int state) {return _map.at(state); }

  std::vector<int> adjacent_nodes() { return adjacent_nodes(_current_state); }
  std::vector<int> adjacent_nodes(int whichstate) { return _map.at(whichstate).adjacent_nodes(); }

  const std::vector<DirectionAndState> &legalmoves() const { return _map.at(_current_state).legalmoves(); }

  DIRECTION lmb() { return lmb(_current_state); }
  DIRECTION lmb(int state) { return _map.at(state).lmb(); }

  int legalmove(DIRECTION d) { return _map.at(_current_state).legalmove(d); }
  int legalmove(int state, DIRECTION d);

  int state_in_direction(DIRECTION d) const { return state_in_direction(_current_state, d); }
  int state_in_direction(int state, DIRECTION d) const;

  bool exists(int state) const { return _map.find(state) != _map.end(); }
  bool empty() const { return _map.empty(); }
  bool valid() const { return !empty() && _current_state != -1; }

  void move(DIRECTION d);
  void set_legal_moves(std::vector<DIRECTION> legalmoves);

  void merge(NodeMap &other);
  void clear();

  void atgoal() { _goal_state = _current_state; }

  const PossibleNode &possible(int state) const;
  int pmroot() const { return possibles_root; }

protected:
  //int _width;
  //int _height;
  int _current_state;
  int _previous_state;
  int _goal_state;
  std::unordered_map<int, Node> _map;

  void merge(std::vector<int> &explored, NodeMap & other, int ours, int theirs);
  //
  // This is so we never end up with a never-ending tree
  // If any maze has multiple paths to the same state, we need to figure out that
  //  we already have this state, and point both paths at the same state.
  // This is the one and only actual use for maintaining the "square" object in the
  //  node, other than the future goal of having the GUI show what we are doing.
  //
  int find_node_by_square(const Square &square) const;

  std::vector<PossibleNode> possibles_map;
  int possibles_root;
  void buildPossibles();
  int buildPossiblesDirection(const std::vector<int> &possibles, DIRECTION legalmovesbyte, DIRECTION skipmove);
  int buildPossiblesSquare(const std::vector<int> &possibles, DIRECTION skipdirection);

  void sort(std::vector<DirectionAndState> &children);

  const std::vector<int> get_possibles(const std::vector<DIRECTION> &legalmoves) const;
  const std::vector<int> get_next_possibles_in_direction(const std::vector<int> &possibles, DIRECTION dir) const;
  const std::vector<int> get_next_possibles_with_matching_walls(const std::vector<int> &possibles, DIRECTION lmb) const;
};
//#####################################################################
class LRTAStar : public AI
{
public:
  virtual DIRECTION nextaction(std::vector<DIRECTION> legalmoves);
  virtual void atgoal(std::vector<DIRECTION> legalmoves);

protected:
  NodeMap final_map;
  NodeMap current_map;
  bool _atgoal = false;

  //
  // The methods that try to figure out where we are
  //
  bool we_are_where_we_think_we_are();
  bool we_can_find_ourselves_offline();
  bool we_can_find_ourselves_offline(int possible_state, int cm_state);
  DIRECTION traveling_to_nearest_uncharted_territory();
  DIRECTION lowest_combined_cost_action();
  DIRECTION find_ourselves_online();
  DIRECTION move_to_closest_unmapped_node();

  Node &current_state() { return current_map[current_map.current_state()]; }
  Node &final_state() { return final_map[final_map.current_state()]; }
  void update_previous_location_costs();
  int node_cost(int whichstate);
};

#endif // __LRTASTAR_H__
