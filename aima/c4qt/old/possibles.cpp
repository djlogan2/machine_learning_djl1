#include "node.h"
#include "possibles.h"

#include <algorithm>
DIRECTION Possibles::lowest_summed_cost_of_where_we_think_we_are(int current_state) {
  std::unordered_map<DIRECTION, int, std::hash<int>> costs;
  // It might be empty, but it should only be empty if it's our first time here.
  if(known_node_location != -1) {
     if(known_nodes->find(known_node_location) != known_nodes->end()) {
       if(known_nodes->at(known_node_location).legalmovesbyte() != new_nodes->at(current_state).legalmovesbyte())
         throw "How can this be?";
       //
       // Get all of the costs for the moves around the state we think we are in
       // If we have one, that is.
       //
       for(NodeMove nm : known_nodes->at(known_node_location).getResultStates()) {
         if(known_nodes->find(nm.result_state) != known_nodes->end())
           costs[nm.move] = known_nodes->at(nm.result_state).cost();
         else
           costs[nm.move] = 0; // If we don't have the node, assume it's the goal
       };
    };
  };

  //
  // Now add in all of the costs in our new node table
  //
  
  for(NodeMove nm : new_nodes->at(current_state).getResultStates())
    if(new_nodes->find(nm.result_state) != new_nodes->end())
      costs[nm.move] += new_nodes->at(nm.result_state).cost();
    else
      costs[nm.move] += 0; // Just to make sure it's actually in the map

  //
  // Now just find and return the lowest one
  //
  int mincost = std::numeric_limits<int>::max();
  DIRECTION retdir = STOP;
  for(auto it : costs) {
    if(it.second < mincost) {
      mincost = it.second;
      retdir = it.first;
    //} else if(it.second == mincost) {
    //  if(std::rand() / RAND_MAX <= 0.5) // Randomly select a move if we have duplicate costs
    //    retdir = it.first;
    };
  };
  return retdir;
}

int sumNodeMoves(int result, const NodeMove &nm) {
  return result | (int)nm.move;
}

bool Possibles::we_are_where_we_think_we_are(std::vector<DIRECTION> legalmoves) {
  if(known_nodes->empty())
    return true;
  if(known_node_location == -1)
    return false;
  if(known_nodes->find(known_node_location) == known_nodes->end()) return true;
  return known_nodes->at(known_node_location).moves_match(legalmoves);
}

//Possible returns are:
//  known_node_location YES   we know where we are
//  -1                  MAYBE make the recommended move and try me again
//  -2                  NO    we have run out of options. Don't call me again.
DIRECTION Possibles::find_out_where_we_are(int current_state, std::vector<DIRECTION> legalmoves) {
  int dir_state;

  if(p_state != -1 && known_nodes->pm_at(p_state).node_location != -1) {
    known_node_location = known_nodes->pm_at(p_state).node_location;
    new_nodes->zerocosts();
    p_untried.clear();
    p_unbacktracked.clear();
    p_state = p_previous_state = -1;
    return lowest_summed_cost_of_where_we_think_we_are(current_state);
  };

  if(p_previous_state == -1) {
    dir_state = known_nodes->pm_first(legalmoves);
    p_state = known_nodes->pmroot();
  } else {
    dir_state = known_nodes->pm_at(p_state).child(legalmoves);
    // .empty() makes sure the vector is created if it doesn't already exist
    if(p_unbacktracked[p_state].empty() || !std::any_of(p_unbacktracked[p_state].begin(), p_unbacktracked[p_state].end(),
            [this](std::pair<DIRECTION, int> n) -> bool {return n.second==p_previous_state; }))
      p_unbacktracked[p_state].push_back(std::pair<DIRECTION, int>(reverse_move(previous_action), p_previous_state));
  };

  // .empty() makes sure the vector is created if it doesn't already exist
  if(dir_state != -1 && (p_untried[dir_state].empty() || p_untried.find(dir_state) == p_untried.end())) {
    p_untried[dir_state] = known_nodes->pmchildren(dir_state);
    for(std::pair<DIRECTION, int> child : known_nodes->pmchildren(dir_state))
      p_untried[dir_state].push_back(std::pair<DIRECTION, int>(child.first, known_nodes->pmroot()));
  };

  if(p_untried.empty() || p_untried[dir_state].empty()) {
    if(p_unbacktracked[p_state].empty()) throw "We can't have a case where we ran out of options!";
    std::pair<DIRECTION, int> bt = p_unbacktracked[p_state].back();
    p_unbacktracked[p_state].pop_back();
    p_previous_state = p_state;
    p_state = bt.second;
    return bt.first;
  } else {
    std::pair<DIRECTION, int> bt = *(p_untried[dir_state].begin());
    p_untried[dir_state].erase(p_untried[dir_state].begin());
    p_previous_state = p_state;
    p_state = bt.second;
    return bt.first;
  };
}

DIRECTION Possibles::minimum_cost_action(int current_state, std::vector<DIRECTION> legalmoves) {
  DIRECTION dir;

  if(!we_are_where_we_think_we_are(legalmoves)) {
    //if(std::find(alreadysearched.begin(), alreadysearched.end(), current_state) != alreadysearched.end())
    //  dir = lowest_summed_cost_of_where_we_think_we_are(current_state);
    //else {
    //  alreadysearched.push_back(current_state);
      dir = find_out_where_we_are(current_state, legalmoves);
    //};
  } else if(legalmoves.size() == 1)
    dir = legalmoves[0];
  else
    dir = lowest_summed_cost_of_where_we_think_we_are(current_state);

  if(known_node_location != -1)
    switch(dir) {
      default:
        throw "We should not have this move";
      case UP:
        known_node_location -= known_nodes->width;
        break;
      case DOWN:
        known_node_location += known_nodes->width;
        break;
      case LEFT:
        known_node_location--;
        break;
      case RIGHT:
        known_node_location++;
        break;
    }
  previous_action = dir;
  return dir;
}

void Possibles::reset()
{
  known_node_location = -1;
  previous_action = STOP;
}

void Possibles::add_to_nodes(DIRECTION where, int howmany) {
//  std::transform(alreadysearched.begin(), alreadysearched.end(), alreadysearched.begin(),
//                 [where, howmany, this](int i) -> int { return add_to_offset(i, where, howmany, new_nodes->width); });
}
