#include <algorithm>
#include <array>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <sstream>
#include <vector>

#define STOP  0
#define LEFT  1
#define UP    2
#define RIGHT 4
#define DOWN  8

class Map {
  private:
    std::array<int, 18*18> map;
    int current_location;
    int goal;
    int _moves;
    int _width;

  public:
    bool won() { return current_location == goal; }
    int at() { return current_location; }
    int moves() { return _moves; }
    std::vector<int> available_actions() {
      std::vector<int> ret;
      for(int x = 8 ; x != 0 ; x >>= 1) {
        if(map[current_location] & x)
          ret.push_back(x);
      };
      return ret;
    }
    Map() {
      _moves = 0;
/*
      current_location = 2;
      goal = 3;
      _width = 3;
      map = {{
        RIGHT,    LEFT+DOWN,     DOWN,
        DOWN,     UP+DOWN,       UP+DOWN,
        UP+RIGHT, LEFT+UP+RIGHT, UP+LEFT
      }};
*/
      current_location = 0;
      goal = 18*18-1;
      _width = 18;
      map = {{
      RIGHT+DOWN,LEFT+RIGHT+DOWN,LEFT+RIGHT,LEFT+RIGHT,LEFT+RIGHT,LEFT+RIGHT,LEFT+DOWN,DOWN,DOWN,RIGHT+DOWN,LEFT+RIGHT,LEFT+RIGHT,LEFT+DOWN,RIGHT+DOWN,LEFT+DOWN,RIGHT+DOWN,LEFT+RIGHT+DOWN,LEFT+DOWN,
      UP+DOWN,UP+RIGHT,LEFT+RIGHT,LEFT+RIGHT+DOWN,LEFT+DOWN,DOWN,UP+DOWN,UP+DOWN,UP+RIGHT,LEFT+UP+RIGHT,LEFT+RIGHT,LEFT,UP+RIGHT,LEFT+UP,UP+DOWN,UP+DOWN,UP+DOWN,UP+DOWN,
      UP+RIGHT,LEFT+RIGHT,LEFT+DOWN,UP+DOWN,UP+DOWN,UP+DOWN,UP+DOWN,UP+RIGHT,LEFT+DOWN,RIGHT+DOWN,LEFT+RIGHT,LEFT+RIGHT,LEFT+RIGHT+DOWN,LEFT+RIGHT,LEFT+UP+RIGHT,LEFT+UP,UP+DOWN,UP+DOWN,
      RIGHT+DOWN,LEFT+RIGHT,LEFT+UP,UP,UP+DOWN,UP+RIGHT,LEFT+UP,RIGHT+DOWN,LEFT+UP,UP+DOWN,RIGHT+DOWN,LEFT+DOWN,UP+DOWN,DOWN,RIGHT+DOWN,LEFT+RIGHT,LEFT+UP,UP+DOWN,
      UP+RIGHT,LEFT+DOWN,DOWN,RIGHT+DOWN,LEFT+UP+RIGHT,LEFT+RIGHT,LEFT+DOWN,UP+DOWN,RIGHT+DOWN,LEFT+UP,UP+DOWN,UP+DOWN,UP+DOWN,UP+DOWN,UP+DOWN,RIGHT+DOWN,LEFT+RIGHT,LEFT+UP,
      RIGHT+DOWN,UP+LEFT+DOWN,UP+DOWN,UP+DOWN,RIGHT+DOWN,LEFT+DOWN,UP+DOWN,UP+DOWN,UP+DOWN,DOWN,UP+DOWN,UP+DOWN,UP+DOWN,UP+RIGHT,LEFT+UP,UP+RIGHT,LEFT+RIGHT,LEFT+DOWN,
      UP+DOWN,UP+DOWN,UP+RIGHT,LEFT+UP,UP+DOWN,UP+DOWN,UP+DOWN,UP+DOWN,UP+DOWN,UP+RIGHT,LEFT+UP,UP+DOWN,UP+DOWN,RIGHT+DOWN,LEFT+DOWN,RIGHT+DOWN,LEFT,UP+DOWN,
      UP+DOWN,UP+RIGHT,LEFT+RIGHT,LEFT+RIGHT,LEFT+UP,UP+DOWN,UP+RIGHT,LEFT+UP,UP+DOWN,RIGHT+DOWN,LEFT+RIGHT,LEFT+UP,UP+DOWN,UP+DOWN,UP+DOWN,UP+RIGHT,LEFT+RIGHT,LEFT+UP+DOWN,
      UP+DOWN,RIGHT+DOWN,LEFT+DOWN,RIGHT,LEFT+DOWN,UP+RIGHT,LEFT+RIGHT,LEFT+RIGHT,LEFT+UP,UP+RIGHT,LEFT+DOWN,RIGHT+DOWN,LEFT+UP,UP+DOWN,UP+DOWN,RIGHT+DOWN,LEFT+DOWN,UP+DOWN,
      UP+RIGHT,LEFT+UP,UP+RIGHT,LEFT+RIGHT,LEFT+UP,RIGHT+DOWN,LEFT+DOWN,RIGHT+DOWN,LEFT+RIGHT,LEFT+DOWN,UP+DOWN,UP+DOWN,RIGHT,LEFT+UP,UP+DOWN,UP+DOWN,UP+DOWN,UP,
      RIGHT+DOWN,LEFT+DOWN,RIGHT+DOWN,LEFT+RIGHT,LEFT+RIGHT,LEFT+UP,UP+RIGHT,LEFT+UP+RIGHT+DOWN,LEFT,UP+RIGHT,LEFT+UP,UP+DOWN,RIGHT+DOWN,LEFT+RIGHT,LEFT+UP,UP+DOWN,UP+RIGHT,LEFT+DOWN,
      UP+DOWN,UP+DOWN,UP+DOWN,RIGHT+DOWN,LEFT+RIGHT,LEFT+RIGHT,LEFT+DOWN,UP,RIGHT+DOWN,LEFT+RIGHT,LEFT+RIGHT,LEFT+UP,UP+RIGHT,LEFT+DOWN,RIGHT+DOWN,LEFT+UP,RIGHT+DOWN,LEFT+UP,
      UP+DOWN,UP+RIGHT,LEFT+UP,UP+DOWN,RIGHT+DOWN,LEFT+DOWN,UP+DOWN,RIGHT+DOWN,LEFT+UP,RIGHT+DOWN,LEFT+DOWN,RIGHT+DOWN,LEFT+RIGHT+DOWN,LEFT+UP,UP+DOWN,RIGHT+DOWN,LEFT+UP,DOWN,
      UP+RIGHT,LEFT+RIGHT,LEFT+DOWN,UP+DOWN,UP+DOWN,UP,UP+RIGHT,LEFT+UP,RIGHT+DOWN,LEFT+UP+DOWN,UP+RIGHT,LEFT+UP,UP,RIGHT+DOWN,LEFT+UP+DOWN,UP+RIGHT,LEFT+RIGHT,LEFT+UP+DOWN,
      RIGHT+DOWN,LEFT+DOWN,UP+DOWN,UP+DOWN,UP+RIGHT,LEFT+RIGHT+DOWN,LEFT+RIGHT,LEFT+RIGHT,LEFT+UP,UP,RIGHT+DOWN,LEFT+RIGHT,LEFT+RIGHT,LEFT+UP,UP+RIGHT,LEFT+RIGHT+DOWN,LEFT,UP+DOWN,
      UP+DOWN,UP+RIGHT+DOWN,LEFT+UP,UP+RIGHT,LEFT+DOWN,UP+RIGHT,LEFT+RIGHT,LEFT+RIGHT,LEFT+DOWN,RIGHT+DOWN,LEFT+UP,RIGHT+DOWN,LEFT+DOWN,RIGHT+DOWN,LEFT+RIGHT,LEFT+UP,RIGHT+DOWN,LEFT+UP,
      UP+DOWN,UP,RIGHT+DOWN,LEFT+DOWN,UP+RIGHT,LEFT+RIGHT,LEFT+RIGHT+DOWN,LEFT+DOWN,UP+RIGHT+DOWN,LEFT+UP,RIGHT+DOWN,LEFT+UP,UP+DOWN,UP+RIGHT,LEFT+DOWN,RIGHT+DOWN,LEFT+UP,DOWN,
      UP+RIGHT,LEFT+RIGHT,LEFT+UP,UP+RIGHT,LEFT+RIGHT,LEFT+RIGHT,LEFT+UP,UP+RIGHT,LEFT+UP,RIGHT,LEFT+UP+RIGHT,LEFT,UP+RIGHT,LEFT+RIGHT,LEFT+UP,UP+RIGHT,LEFT+RIGHT,LEFT+UP
      }};
    }

    int reverse_move(int move) {
      switch(move) {
        default:
          throw "Invalid move";
        case UP:
          return DOWN;
        case DOWN:
          return UP;
        case LEFT:
          return RIGHT;
        case RIGHT:
          return LEFT;
      };
      return current_location;
    };

    int move(int _move) {
      switch(map[current_location] & _move) {
        case STOP:
          throw "Invalid move";
        case UP:
          current_location -= _width;
          break;
        case DOWN:
          current_location += _width;
          break;
        case LEFT:
          current_location -= 1;
          break;
        case RIGHT:
          current_location += 1;
          break;
      };
      _moves++;
      return current_location;
    };
};

class hashing_func
{
  public:
    unsigned long operator()(const std::array<int, 2> &key) const {
      return key[0] * 100 + key[1];
    };
};
class key_equal_func
{
  public:
    bool operator()(const std::array<int, 2> &a, const std::array<int, 2> &b) const {
      return a[0] == b[0] && a[1] == b[1];
    };
};

Map m;
std::unordered_map<int, std::vector<int>> untried;
std::unordered_map<std::array<int, 2>, int, hashing_func, key_equal_func> results;
std::unordered_map<int, std::vector<int>> unbacktracked;
//std::vector<int> seen_states;

int previous_state = -1;
int previous_action = -1;

std::string dir(int action) {
  switch(action) {
    case STOP: return "stop";
    case UP: return "UP";
    case DOWN: return "DOWN";
    case LEFT: return "LEFT";
    case RIGHT: return "RIGHT";
  };
  return "";
}

std::string print_untried() {
  std::ostringstream ss;
  ss << "untried: [";
  for(auto iter : untried) {
    ss << '[' << iter.first << '=';
    for(auto action : iter.second) ss << dir(action) << ',';
    ss << ']';
  };
  ss << ']';
  return ss.str();
}

std::string print_results() {
  std::ostringstream ss;
  ss << "results: [";
  for(auto iter : results) {
    auto key = iter.first;
    auto action = iter.second;
    ss << "move " << dir(action) << " from " << key[0] << " to " << key[1] << ',';
  };
  ss << ']';
  return ss.str();
}

std::string print_unbacktracked() {
  std::ostringstream ss;
  ss << "unbacktracked: [";
  for(auto iter : unbacktracked) {
    ss << '[' << iter.first << '=';
    for(auto state : iter.second) ss << state << ',';
    ss << ']';
  };
  ss << ']';
  return ss.str();
}

int online_dfs_agent() {

  if(m.won()) {
    std::cout << "We have reached the goal" << std::endl << std::flush;
    return STOP;
  };

  auto available_actions = untried.find(m.at());
  if(available_actions == untried.end()) {
    untried[m.at()] = m.available_actions();
  };

  if(previous_state != -1) {
    //if(!std::any_of(seen_states.begin(), seen_states.end(), [](int s){return s == m.at();})) {
    //  seen_states.push_back(m.at());
      results[{{previous_state, m.at()}}] = previous_action;
      results[{{m.at(), previous_state}}] = m.reverse_move(previous_action);
      if(!std::any_of(unbacktracked[m.at()].begin(), unbacktracked[m.at()].end(), [](int s){return s == previous_state;}))
        // STOP the madness of oscillating back and forth?? unbacktracked[m.at()].push_back(previous_state);
        unbacktracked[m.at()].insert(unbacktracked[m.at()].begin(), previous_state); // Try to do a fifo queue instead of a lifo queue and see if that helps
    //};
  };

  std::cout << print_untried() << std::endl << std::flush;
  std::cout << print_unbacktracked() << std::endl << std::flush;
  std::cout << print_results() << std::endl << std::flush;

  if(!untried.size() || !untried[m.at()].size()) {
    if(!unbacktracked.size() || !unbacktracked[m.at()].size()) {
      std::cout << "No place to backtrack, sadly there is no solution" << std::endl << std::flush;
      return STOP;
    };
    int undo_state = unbacktracked[m.at()].back();
    unbacktracked[m.at()].pop_back();
    //if(!unbacktracked[m.at()].size()) unbacktracked.erase(m.at());
    int undo_action = results[{{m.at(), undo_state}}];
    std::cout << "Backtracking from " << m.at() << " to " << undo_state << std::endl << std::flush;
    return undo_action;
  } else {
    int action = untried[m.at()].front();
    untried[m.at()].erase(untried[m.at()].begin());
    // Don't do this! if(!untried[m.at()].size()) untried.erase(m.at());
    std::cout << "Executing new action " << dir(action) << std::endl << std::flush;
    return action;
  };
}

std::unordered_map<int, int> H;
std::unordered_map<int, std::vector<int>> action_list;

int lrta_cost(int orig_state, int action, int new_state) {
  return 0;
}

int lrta_star() {
  if(m.won()) {
    std::cout << "We have reached the goal" << std::endl << std::flush;
    return STOP;
  };

  if(H.find(m.at()) == H.end()) {
    H[m.at()] = 1; // An initial cost is always 1, because we always assume the next square is the goal!
  };

  action_list[m.at()] = m.available_actions();

  if(previous_state != -1) {
    int cost = std::numeric_limits<int>::max();
    results[{{previous_state, m.at()}}] = previous_action;
    for(auto action : action_list[previous_state]) {
      int cost_one = lrta_cost(previous_state, action, m.at());
      if(cost > cost_one) cost = cost_one;
    };
    //H[previous_state] = std::min(lrtacost);
  };
  return 0;
};

int main() {
  int action = online_dfs_agent();
  //int action = lrta_star();
  while(action != STOP) {
    previous_state = m.at();
    previous_action = action;
    std::cout << "Making move #" << m.moves() << ' ' << dir(action) << " from " << m.at();
    m.move(action);
    std::cout << " to " << m.at() << std::endl << std::endl << std::flush;
    action = online_dfs_agent();
    //action = lrta_star();
  };
  return 0;
}
