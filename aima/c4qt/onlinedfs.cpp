#include "onlinedfs.h"

#include <algorithm>
#include <iostream>

OnlineDFS::OnlineDFS(Map *m)
{
    this->m = m;
}

int OnlineDFS::nextaction() {

    //
    // If we won, blow out of here
    //
  if(m->won()) {
      std::cout << "We won!" << std::endl << std::flush;
      previous_state = m->_at();
      previous_action = STOP;
    return STOP;
  };

  //
  // Look for available actions we have already recorded. If we don't have untried moves, add the untried moves
  //
  auto available_actions = untried.find(m->_at());
  if(available_actions == untried.end()) {
    untried[m->_at()] = m->available_actions();
  };
  //
  // If we are on the 2nd+ move, record the state changes in results
  //   and if we don't already have this square in the list of unbacktracked states, add it
  //
  if(previous_state != -1) {
      results[{{previous_state, m->_at()}}] = previous_action;
      results[{{m->_at(), previous_state}}] = m->reverse_move(previous_action);
      if(!std::any_of(unbacktracked[m->_at()].begin(), unbacktracked[m->_at()].end(), [this](int s){return s == previous_state;}))
        unbacktracked[m->_at()].insert(unbacktracked[m->_at()].begin(), previous_state);
      //
      // Remove the "untried" move that just goes back to where we were
      //
      auto reverse_move = std::find(std::begin(untried[m->_at()]), std::end(untried[m->_at()]), m->reverse_move(previous_action));
      if(reverse_move != std::end(untried[m->_at()]))
          untried[m->_at()].erase(reverse_move);
  };

  //
  // If we don't have any untried moves, and there is nothing to backtrack, no goal was found
  //
  if(!untried.size() || !untried[m->_at()].size()) {
    if(!unbacktracked.size() || !unbacktracked[m->_at()].size()) {
        previous_state = m->_at();
        previous_action = STOP;
      return STOP;
    };
    //
    // Since we had  no untried moves, backtrack and try again
    //
    int undo_state = unbacktracked[m->_at()].back();
    unbacktracked[m->_at()].pop_back();
    int undo_action = results[{{m->_at(), undo_state}}];
    std::cout << "We have no place to go, backtracking" << std::endl << std::flush;
    previous_state = m->_at();
    previous_action = undo_action;
    return undo_action;
  } else {
      //
      // We have an untried move, so let's try that
      //
    int action = untried[m->_at()].front();
    untried[m->_at()].erase(untried[m->_at()].begin());
    std::cout << "Trying untried move " << (action ==STOP ? "STOP" : (action == UP ? "UP" : (action == DOWN ? "DOWN" : (action == LEFT ? "LEFT" : "RIGHT")))) << std::endl << std::flush;;
    previous_state = m->_at();
    previous_action = action;
    return action;
  };
}

