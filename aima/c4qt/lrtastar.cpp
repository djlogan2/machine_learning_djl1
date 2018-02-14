#include "lrtastar.h"

#include <iostream>

std::vector<int> Node::availablestates()
{
    std::vector<int> ret;
    for(auto nm : resultstates)
        ret.push_back(nm.result_state);
    return ret;
}

int Node::action(int towhichstate) {
    auto nodemoveptr = std::find_if(std::begin(resultstates), std::end(resultstates), [towhichstate](NodeMove nm) {return nm.result_state==towhichstate;});
    if(nodemoveptr != resultstates.end())
      return nodemoveptr->move;
    else
      return STOP;
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
        std::cout << "Adding move state=" << state << ", action=" << Debug::d(move) << ", newstate=" << newstate << " to Node" << std::endl << std::flush;
        resultstates.push_back(NodeMove(newstate, move));
    }
}
LRTAStar::LRTAStar(Map *m)
{
    this->m = m;
}

void Node::add_to_states(int width, int add, bool addrow) {
    if(addrow) _state += (this->_state / width);
    _state += add;
    std::transform(resultstates.begin(), resultstates.end(), resultstates.begin(),
                       [width, add, addrow](NodeMove nm) -> NodeMove { return NodeMove(nm.result_state+(addrow?(nm.result_state/width):0)+add, nm.move); });
//    for(auto nm : resultstates) {
//        if(addrow) nm.result_state += (nm.result_state / width);
//        nm.result_state += add;
//    }
}

void LRTAStar::add_to_nodes(int width, int add, bool addrow) {
    std::cout << "Adjusting all nodes, width=" << width << ", add=" << add << ", addrow=" << addrow << std::endl << std::flush;
    std::unordered_map<int, Node> new_nodes;
    for(auto node : current_nodes) {
        node.second.add_to_states(width, add, addrow);
        new_nodes.emplace(node.second.state(), node.second);
    }
    current_nodes = new_nodes;
    if(previous_state != -1) {
        if(addrow) previous_state += (previous_state/width);
        previous_state += add;
    }
    if(goal != -1) {
        if(addrow) goal += (goal/width);
        goal += add;
    }
}

int LRTAStar::minimum_cost(int whichstate) {
  int cost = std::numeric_limits<int>::max();
  for(auto state : current_nodes.at(whichstate).availablestates())
      if(cost > current_nodes.at(state).cost())
          cost = current_nodes.at(state).cost();
  return cost;
}

int LRTAStar::minimum_cost_action(int whichstate) {
    int cost = std::numeric_limits<int>::max();
    int action = 0;
    for(auto state : current_nodes.at(whichstate).availablestates()) {
        if(current_nodes.find(state) == current_nodes.end())
            return current_nodes.at(whichstate).action(state); // The "minimum cost action" is any for which we don't have a cost, so it's assumed to be 1.
        if(cost > current_nodes.at(state).cost()) {
            cost = current_nodes.at(state).cost();
            action = current_nodes.at(state).action(state);
        }
    }
    return action;
}

int LRTAStar::nextaction(std::vector<int> legalmoves) {
    //
    // If we won, blow out of here
    //
  if(m->won()) {
      std::cout << "We won!" << std::endl << std::flush;
    return TELEPORT;
  };

  if(previous_action == UP && current_r == 0) {
      height++;
      add_to_nodes(width, width, false);
  }
  else if(previous_action == DOWN && ++current_r == height)
      height++;
  else if(previous_action == LEFT && current_c == 0) {
      width++;
      add_to_nodes(width, 1, true);
  }
  else if(previous_action == RIGHT && ++current_c >= width) {
      width++;
      add_to_nodes(width, 0, true);
  }

  if(current_nodes.find(current_state()) == current_nodes.end()) {
      std::cout << "Adding " << current_state() << " to current_nodes" << std::endl << std::flush;
      current_nodes.emplace(current_state(), Node(width, current_state(), 1, legalmoves));
  }

  if(previous_state != -1) {
      //current_nodes.at(previous_state).addMove(previous_action, current_state());
      //current_nodes.at(current_state()).addMove(m->reverse_move(previous_action), previous_state);
      current_nodes.at(previous_state).cost(minimum_cost(previous_state));
  }
  previous_action = minimum_cost_action(current_state());
  previous_state = current_state();
  return previous_action;
}
