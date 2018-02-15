#include "lrtastar.h"

#include <algorithm>
#include <iostream>

std::vector<int> Node::availablestates()
{
    std::vector<int> ret;
    for(auto nm : resultstates)
        ret.push_back(nm.result_state);
    std::random_shuffle(ret.begin(), ret.end());
    return ret;
}

int Node::action(int towhichstate) {
    auto nodemoveptr = std::find_if(std::begin(resultstates), std::end(resultstates), [towhichstate](NodeMove nm) {return nm.result_state==towhichstate;});
    if(nodemoveptr != resultstates.end())
      return nodemoveptr->move;
    else {
      return STOP;
    };
}

Node::Node(int width, int state, int cost, std::vector<int> legalmoves) {
    this->_state = state ;
    this->_cost = cost;
    for(auto move : legalmoves) {
        int newstate = state;
        switch(move) {
        case UP:
            newstate -= width;
            break;
        case DOWN:
            newstate += width;
            break;
        case LEFT:
            newstate--;
            break;
        case RIGHT:
            newstate++;
            break;
        }
        resultstates.push_back(NodeMove(newstate, move));
    }
}
LRTAStar::LRTAStar(Map *m)
{
    this->m = m;
}

#define ADJUST(V, OLDWIDTH, ADD, USEROW) (V+(USEROW?V/OLDWIDTH:0)+ADD)

void Node::add_to_states(int oldwidth, int add, bool userow) {
    _state = ADJUST(_state, oldwidth, add, userow);
    std::transform(resultstates.begin(), resultstates.end(), resultstates.begin(),
       [oldwidth, add, userow](NodeMove nm) -> NodeMove { 
        return NodeMove(ADJUST(nm.result_state, oldwidth, add, userow), nm.move);
     });
}

void LRTAStar::add_to_nodes(int oldwidth, int add, bool userow) {
    std::unordered_map<int, Node> new_nodes;
    for(auto node : current_nodes) {
        node.second.add_to_states(oldwidth, add, userow);
        new_nodes.emplace(node.second.state(), node.second);
    }
    current_nodes = new_nodes;
    if(previous_state != -1) previous_state = ADJUST(previous_state, oldwidth, add, userow);
    if(goal != -1) goal = ADJUST(goal, oldwidth, add, userow);
}

int LRTAStar::minimum_cost(int whichstate) {
  int cost = std::numeric_limits<int>::max();
  for(auto state : current_nodes.at(whichstate).availablestates()) {
      if(current_nodes.find(state) == current_nodes.end())
          return 0; // Any node we haven't yet traveled to automatically has a heuristic of 0, assuming it's going to be the goal.
      if(cost > current_nodes.at(state).cost())
          cost = current_nodes.at(state).cost();
  };
  return cost + 1; // Return the cost of this node plus the cost of getting to this node from the previous node (one.)
}

int LRTAStar::minimum_cost_action(int whichstate) {
    int cost = std::numeric_limits<int>::max();
    int action = 0;
    for(auto state : current_nodes.at(whichstate).availablestates()) {
        if(current_nodes.find(state) == current_nodes.end()) {
            return current_nodes.at(whichstate).action(state); // The "minimum cost action" is any for which we don't have a cost, so it's assumed to be 0, or it's the goal.
        };
        if(cost > current_nodes.at(state).cost()) {
            cost = current_nodes.at(state).cost();
            action = current_nodes.at(whichstate).action(state);
        }
    }
    return action;
}

void LRTAStar::update_scores(int previous_state, int state, int score)
{
  if(current_nodes.find(state) == current_nodes.end()) return;
  current_nodes.at(state).cost(score);
  for(int next_state : current_nodes.at(state).availablestates()) {
    if(next_state != previous_state)
      update_scores(state, next_state, score+1);
  };
}

int LRTAStar::nextaction(std::vector<int> legalmoves) {
    //
    // If we won, blow out of here
    //
  if(m->won()) {
      goal = current_state();
      update_scores(-1, goal, 0);
      std::cout << "We won!" << std::endl << std::flush;
      std::cout << "Goal is at " << current_state() << std::endl << std::flush;
      for(auto it = current_nodes.begin() ; it != current_nodes.end() ; ++it) {
        std::cout << it->second<< std::endl;
      };
      std::cout << std::flush;
    return TELEPORT;
  };

  if(previous_action == UP) current_r--;
  else if(previous_action == DOWN) current_r++;
  else if(previous_action == LEFT) current_c--;
  else if(previous_action == RIGHT) current_c++;

  if(std::find(legalmoves.begin(), legalmoves.end(), UP) != legalmoves.end() && current_r == 0) {
      current_r++;
      height++;
      add_to_nodes(width, width, false);
  };
  if(std::find(legalmoves.begin(), legalmoves.end(), LEFT) != legalmoves.end() && current_c == 0) {
      current_c++;
      add_to_nodes(width, 1, true);
      width++;
  };
  if(std::find(legalmoves.begin(), legalmoves.end(), RIGHT) != legalmoves.end() && (current_c+1) == width) {
      add_to_nodes(width, 0, true);
      width++;
  };
  if(std::find(legalmoves.begin(), legalmoves.end(), DOWN) != legalmoves.end() && (current_r+1) == height) {
      height++;
  };

  if(current_nodes.find(current_state()) == current_nodes.end()) {
      current_nodes.emplace(current_state(), Node(width, current_state(), 1, legalmoves)); // We use a cost of 1 because we know this isn't the goal node, so we assume the goal is one step away
  }

  if(previous_state != -1) {
      current_nodes.at(previous_state).cost(minimum_cost(previous_state));
  }
  previous_action = minimum_cost_action(current_state());
  previous_state = current_state();
  return previous_action;
}
