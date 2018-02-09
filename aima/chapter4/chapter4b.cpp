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
    std::array<int, 9> map;
    int current_location;
    int goal;
    int _moves;

  public:
    bool won() { return current_location == goal; }
    int at() { return current_location; }
    int moves() { return _moves; }
    std::queue<int> available_actions() {
      std::queue<int> ret;
      for(int x = 8 ; x != 0 ; x >>= 1) {
        if(map[current_location] & x)
          ret.push(x);
      };
      return ret;
    }
    Map() {
      current_location = 2;
      goal = 3;
      _moves = 0;
      map = {{
        RIGHT,    LEFT+DOWN,     DOWN,
        DOWN,     UP+DOWN,       UP+DOWN,
        UP+RIGHT, LEFT+UP+RIGHT, UP+LEFT
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
          current_location -= 3;
          break;
        case DOWN:
          current_location += 3;
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
std::unordered_map<int, std::queue<int>> untried;
std::unordered_map<std::array<int, 2>, int, hashing_func, key_equal_func> results;
std::unordered_map<int, std::vector<int>> unbacktracked;

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
}

std::string print_untried() {
  std::ostringstream ss;
  ss << "untried: [";
  for(auto iter : untried) {
    ss << '[' << iter.first << '=';
    std::queue<int> temp = iter.second;
    while(!temp.empty()) { ss << dir(temp.front()) << ','; temp.pop(); }
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
    results[{{previous_state, m.at()}}] = previous_action;
    results[{{m.at(), previous_state}}] = m.reverse_move(previous_action);
    if(!std::any_of(unbacktracked[m.at()].begin(), unbacktracked[m.at()].end(), [](int s){return s == previous_state;}))
      unbacktracked[m.at()].push_back(previous_state);
//    std::queue<int>::iterator it = find(unbacktracked[m.at()].begin(), unbacktracked[m.at()].end(), previous_action);
//    if(it == unbacktracked[m.at()].end())
//      unbacktracked[m.at()].push(previous_state);
  };

  std::cout << print_untried() << std::endl << std::flush;
  std::cout << print_unbacktracked() << std::endl << std::flush;
  std::cout << print_results() << std::endl << std::flush;

  if(!untried.size() || !untried[m.at()].size()) {
    if(!unbacktracked.size() || !unbacktracked[m.at()].size()) return STOP;
    int undo_state = unbacktracked[m.at()].back();
    unbacktracked[m.at()].pop_back();
    //if(!unbacktracked[m.at()].size()) unbacktracked.erase(m.at());
    int undo_action = results[{{m.at(), undo_state}}];
    std::cout << "Backtracking from " << m.at() << " to " << undo_state << std::endl << std::flush;
    return undo_action;
  } else {
    int action = untried[m.at()].front();
    untried[m.at()].pop();
    // Don't do this! if(!untried[m.at()].size()) untried.erase(m.at());
    std::cout << "Executing new action " << dir(action) << std::endl << std::flush;
    return action;
  };
}

int main() {
  int action = online_dfs_agent();
  while(action != STOP) {
    previous_state = m.at();
    previous_action = action;
    std::cout << "Making move #" << m.moves() << ' ' << dir(action) << " from " << m.at();
    m.move(action);
    std::cout << " to " << m.at() << std::endl << std::endl << std::flush;
    action = online_dfs_agent();
  };
  return 0;
}
