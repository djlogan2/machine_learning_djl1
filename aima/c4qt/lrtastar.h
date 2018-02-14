#ifndef LRTASTAR_H
#define LRTASTAR_H

#include "ai.h"
#include "map.h"

#include <ostream>
#include <unordered_map>
#include <vector>

struct NodeMove {
    NodeMove(int state, int move) { this->result_state = state ; this->move = move; }
    int result_state;
    int move;
    friend std::ostream& operator<<(std::ostream &os, const NodeMove &nm) { os << "NodeMove(result_state=" << nm.result_state << ", move=" << Debug::d(nm.move) << ')'; return os; }
};

class Node {
public:
    //Node() { this->state = std::numeric_limits<int>::max(); this->_cost = std::numeric_limits<int>::max(); }
    Node(const Node &other) { this->_state = other._state ; this->_cost = other._cost; resultstates = other.resultstates; }
    //Node(int state) { this->state = state ; this->_cost = 0; }
    Node(int width, int state, int cost, std::vector<int> legalmoves);
    int cost() { return _cost; }
    void cost(int value) { _cost = value; }
    int state() { return this->_state; }
    void addMove(int move, int newstate) { resultstates.push_back(NodeMove(newstate, move)); }
    std::vector<int> availablestates();
    int action(int towhichstate);
    Node &operator=(const Node& other) { this->_state = other._state ; this->_cost = other._cost; return *this; }
    void add_to_states(int oldwidth, int add, bool userow);
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

class LRTAStar : public AI
{
public:
    LRTAStar(Map *m);
    virtual int nextaction(std::vector<int> legalmoves);
protected:
    std::unordered_map<int, Node> current_nodes;
    int goal = -1;
    int current_r = 0;
    int current_c = 0;
    int current_state() { return current_r * width + current_c; }
    int previous_state = -1;
    int width = 1;
    int height = 1;
    Map *m;

    void add_to_nodes(int oldwidth, int add, bool userow);

    int minimum_cost(int whichstate);
    int minimum_cost_action(int whichstate);
    void update_scores(int previous_state, int state, int score);
};

#endif // LRTASTAR_H
