#include "lrtastar.h"

#include <algorithm>
#include <limits>  // for numeric_limits

LRTAStar::LRTAStar(Map *m)
{
    this->m = m;
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

DIRECTION LRTAStar::minimum_cost_action(int whichstate) {
    int cost = std::numeric_limits<int>::max();
    DIRECTION action = STOP;
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

DIRECTION LRTAStar::nextaction(std::vector<DIRECTION> legalmoves) {

  if(previous_action == UP) current_r--;
  else if(previous_action == DOWN) current_r++;
  else if(previous_action == LEFT) current_c--;
  else if(previous_action == RIGHT) current_c++;

  if(m->won()) {
      current_nodes.goal = current_state();
      known_nodes.merge(current_nodes);
      current_nodes.clear();
      current_r = current_c = 0;

      //update_scores(-1, goal, 0); This is illegal in LRTAStar. LRTAStar is supposed to converge on accurate costs over repeated runs. Let's see how it works!
      std::cout << "We won!" << std::endl << std::flush;
      std::cout << "Goal is at " << current_state() << std::endl << std::flush;
    return TELEPORT;
  };

  if(std::find(legalmoves.begin(), legalmoves.end(), UP) != legalmoves.end() && current_r == 0) {
      current_r++;
      current_nodes.add_to_nodes(UP, 1);
  };
  if(std::find(legalmoves.begin(), legalmoves.end(), LEFT) != legalmoves.end() && current_c == 0) {
      current_c++;
      current_nodes.add_to_nodes(LEFT, 1);
  };
  if(std::find(legalmoves.begin(), legalmoves.end(), RIGHT) != legalmoves.end() && (current_c+1) == current_nodes.width) {
      current_nodes.add_to_nodes(RIGHT, 1);
  };
  if(std::find(legalmoves.begin(), legalmoves.end(), DOWN) != legalmoves.end() && (current_r+1) == current_nodes.height) {
      current_nodes.add_to_nodes(DOWN, 1);
  };

  if(current_nodes.find(current_state()) == current_nodes.end()) {
      current_nodes.emplace(current_state(), Node(current_nodes.width, current_state(), 1, legalmoves)); // We use a cost of 1 because we know this isn't the goal node, so we assume the goal is one step away
  }

  if(current_nodes.previous_state != -1) {
      current_nodes.at(current_nodes.previous_state).cost(minimum_cost(current_nodes.previous_state));
  }
  previous_action = minimum_cost_action(current_state());
  current_nodes.previous_state = current_state();
  return previous_action;
}
