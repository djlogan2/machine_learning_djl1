#ifndef __POSSIBLES_H__
#define __POSSIBLES_H__

#include "direction.h"
#include "nodemap.h"

#include <limits>
#include <vector>
#include <unordered_map>

class Possibles {
public:
  Possibles(NodeMap *known, NodeMap *unknown) { known_nodes = known; new_nodes = unknown; }
  DIRECTION minimum_cost_action(int current_state, std::vector<DIRECTION> legalmoves);
  void reset();
  void add_to_nodes(DIRECTION where, int howmany);

protected:
  int known_node_location = -1;
  int p_previous_state = -1;
  int p_state = -1;
  std::unordered_map<int, std::vector<std::pair<DIRECTION, int>>> p_untried;
  std::unordered_map<int, std::vector<std::pair<DIRECTION, int>>> p_unbacktracked;
//  std::vector<int> alreadysearched;
  NodeMap *known_nodes;
  NodeMap *new_nodes;
  DIRECTION previous_action;

  bool we_are_where_we_think_we_are(std::vector<DIRECTION> legalmoves);
  DIRECTION lowest_summed_cost_of_where_we_think_we_are(int current_state);
  DIRECTION find_out_where_we_are(int current_state, std::vector<DIRECTION> legalmoves);
};

#endif // __POSSIBLES_H__
