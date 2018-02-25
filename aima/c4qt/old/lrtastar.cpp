#include "lrtastar.h"

#include <algorithm>
#include <limits>  // for numeric_limits

LRTAStar::LRTAStar(Map *m) {
    possibles = new Possibles(&known_nodes, &current_nodes);
    this->m = m;
}

int LRTAStar::minimum_cost(int whichstate) {
  int cost = std::numeric_limits<int>::max();
  for(auto state : current_nodes.at(whichstate).availablestates()) {
      if(current_nodes.find(state) == current_nodes.end())
          return 1; // Any node we haven't yet traveled to automatically has a heuristic of 0, assuming it's going to be the goal.
      if(cost > current_nodes.at(state).cost())
          cost = current_nodes.at(state).cost();
  };
  return cost;
}

DIRECTION LRTAStar::minimum_cost_action(std::vector<DIRECTION> legalmoves) {
    DIRECTION test_direction = possibles->minimum_cost_action(current_state(), legalmoves);
    return test_direction;
}

DIRECTION LRTAStar::nextaction(std::vector<DIRECTION> legalmoves) {

  if(m->won()) {
      current_nodes.goal = current_state();
      known_nodes.merge(current_nodes);
      known_nodes.buildPossibles();
      current_nodes.clear();
      current_r = current_c = 0;
      possibles->reset();

      //update_scores(-1, goal, 0); This is illegal in LRTAStar. LRTAStar is supposed to converge on accurate costs over repeated runs. Let's see how it works!
      std::cout << "We won!" << std::endl << std::flush;
      std::cout << "Goal is at " << current_state() << std::endl << std::flush;
      return TELEPORT;
  };

  if(std::find(legalmoves.begin(), legalmoves.end(), UP) != legalmoves.end() && current_r == 0) {
      current_r++;
      current_nodes.add_to_nodes(UP, 1);
      possibles->add_to_nodes(UP, 1);
  };
  if(std::find(legalmoves.begin(), legalmoves.end(), LEFT) != legalmoves.end() && current_c == 0) {
      current_c++;
      current_nodes.add_to_nodes(LEFT, 1);
      possibles->add_to_nodes(LEFT, 1);
  };
  if(std::find(legalmoves.begin(), legalmoves.end(), RIGHT) != legalmoves.end() && (current_c+1) == current_nodes.width) {
      current_nodes.add_to_nodes(RIGHT, 1);
      possibles->add_to_nodes(RIGHT, 1);
  };
  if(std::find(legalmoves.begin(), legalmoves.end(), DOWN) != legalmoves.end() && (current_r+1) == current_nodes.height) {
      current_nodes.add_to_nodes(DOWN, 1);
      possibles->add_to_nodes(DOWN, 1);
  };

  if(current_nodes.find(current_state()) == current_nodes.end()) {
      // We use a cost of 1 because we know this isn't the goal node, so we assume the goal is one step away
      current_nodes.emplace(current_state(), Node(current_nodes.width, current_state(), 1, legalmoves));
  }

  if(current_nodes.previous_state != -1) {
      current_nodes.at(current_nodes.previous_state).cost(minimum_cost(current_nodes.previous_state) + 1);
  }

  previous_action = minimum_cost_action(legalmoves);
  current_nodes.previous_state = current_state();

  if(previous_action == UP) current_r--;
  else if(previous_action == DOWN) current_r++;
  else if(previous_action == LEFT) current_c--;
  else if(previous_action == RIGHT) current_c++;

  return previous_action;
}
