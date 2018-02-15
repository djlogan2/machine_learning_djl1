#ifndef NODE_H
#define NODE_H

#include "map.h"

#include <iostream>

extern int add_to_offset(int offset, DIRECTION where, int howmany, int width);

struct NodeMove {
    NodeMove(int state, DIRECTION move) { this->result_state = state ; this->move = move; }
    int result_state;
    DIRECTION move;
    friend std::ostream& operator<<(std::ostream &os, const NodeMove &nm) { os << "NodeMove(result_state=" << nm.result_state << ", move=" << Debug::d(nm.move) << ')'; return os; }
};

class Node {
public:
    Node(const Node &other) {
      this->_state = other._state ;
      this->_cost = other._cost;
      resultstates = other.resultstates;
    }
    Node(int width, int state, int cost, std::vector<DIRECTION> legalmoves);
    int cost() { return _cost; }
    void cost(int value) { _cost = value; }
    int state() { return this->_state; }
    void addMove(DIRECTION move, int newstate) { resultstates.push_back(NodeMove(newstate, move)); }
    std::vector<int> availablestates();
    DIRECTION action(int towhichstate);
    Node &operator=(const Node& other) { this->_state = other._state ; this->_cost = other._cost; this->resultstates = other.resultstates; return *this; }
    void add_to_states(DIRECTION where, int howmany, int width);
    friend std::ostream& operator<<(std::ostream &os, const Node &n) {
      os << "Node(_state=" << n._state << ", _cost=" << n._cost << ", resultstates=[";
      for(auto nm : n.resultstates) os << nm;
      os << "])";
      return os;
    }
protected:
    int _state;
    int _cost; // Number of moves to the goal
    std::vector<NodeMove> resultstates;
};

#endif // NODE_H
