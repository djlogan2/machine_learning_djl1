#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <exception>
#include <iostream>
#include <list>
#include <queue>
#include <sstream>
#include <stack>
#include <unordered_map>
#include <vector>

#define BOARD_TYPE unsigned char

typedef class _square {
  public:
    int x;
    int y;
    _square() { x = 0 ; y = 0; }
    _square(int _x, int _y) {
      if(_x < 0 || _x > 3 || _y < 0 || _y > 3)
        throw std::logic_error("WTF???");
      x = _x; y = _y;
    }

    _square(const _square &other) {
      x = other.x;
      y = other.y;
    }

    _square(_square *other) {
      if(other == NULL) {
        x = -1; y = -1;
      } else {
        x = other->x;
        y = other->y;
      };
    }

    std::string tostring() {
      std::ostringstream ss;
      ss << "Square(x=" << x << ",y=" << y << ")";
      return ss.str();
    };

} Square;

typedef class _board {
  public:
    _board(int side);
    _board(int side, BOARD_TYPE *board_array);
    ~_board();
    
    _board *newBoard(Square from);
    BOARD_TYPE *board() { return board_array; }
    bool solvable();
    bool won();
    Square zero();
    void print();
    inline bool same(_board *other) { return memcmp(board_array, other->board_array, _side * _side) == 0; }
    double manhattan_distance();
    int side() { return _side; }
    std::string dump() {
      std::ostringstream ss;
      for(int y = 0 ; y < _side ; y++) {
        for(int x = 0 ; x < _side ; x++) {
          ss << (int)board_array[y * _side + x] << ',';
        };
        ss << std::endl;
      };
      ss << "side=" << _side << ", zero=" << zero().tostring();
      return ss.str();
    }
    std::string tostring() {std::string r((const char *)board_array, _side*_side*sizeof(BOARD_TYPE)); return r; }

  protected:
  private:
    BOARD_TYPE *board_array;
    int _side;
} Board;

typedef class _state {
  public:
    _state(Board *board, Square action, double current_cost, _state *parent);
    ~_state();
    double cost();
    int depth();
    _state *parent() { return _parent; }
    Square *action() { return _action; }
    bool already_in_chain() {
      if(!_parent) return false;
      return _parent->already_in_chain(this);
    };
    Board *board();
    std::vector<_state *> children();
    std::string tostring(bool printchildren=false) {
      std::ostringstream ss;
      ss << "State: " << this << std::endl;
      ss << _board->dump() << std::endl;
      ss << "   Action=" << (_action ? _action->tostring() : "No action");
      ss << ", parent=" << _parent;
      ss << ", current_cost=" << _current_cost;
      ss << ", cost=" << cost() << '|' << std::endl;
      if(printchildren) {
        std::vector<_state *> children = this->children();
        for(std::vector<_state *>::iterator child = children.begin() ; child != children.end(); ++child) {
          ss << "      child=" << (*child)->tostring(false) << std::endl;
        };
      };
      return ss.str();
    };

  protected:
  private:
    Board *_board;
    Square *_action;
    _state *_parent;
    std::vector<_state *> *_children;
    double _current_cost;
    double _cost;

    bool already_in_chain(_state *other) {
      if(_board->same(other->_board))
        return true;
      if(!_parent) return false;
      return _parent->already_in_chain(other);
    }
} State;

_board::_board(int side) {
  _side = side;
  board_array = new BOARD_TYPE[_side*_side];

  for(int x = 0 ; x < _side*_side ; x++)
    board_array[x] = x;

  do {
    for(int x = 0 ; x < _side * _side ; x++) {
      int r = std::rand() % (_side * _side);
      BOARD_TYPE temp = board_array[x];
      board_array[x] = board_array[r];
      board_array[r] = temp;
    };
  } while(!solvable());
}

_board::_board(int side, BOARD_TYPE *board_array) {
  this->_side = side;
  this->board_array = board_array;
}

_board::~_board() {
  delete board_array;
}

bool _board::solvable() {
  int inversions = 0;
  for(int x = 0 ; x < _side * _side ; x++) {
    for(int y = x + 1 ; y < _side * _side ; y++) {
      if(board_array[x] && board_array[y] && board_array[x] > board_array[y]) inversions++;
    };
  };
  if(_side % 2 == 1 && inversions % 2 == 0) return true;
  if(_side % 2 == 1) return false;
  return ((_side - zero().x) % 2 == inversions %2);
}

bool _board::won() {
  int n = 1;
  for(int x = 0 ; x < _side * _side ; x++) {
    if(board_array[x] != 0) {
      if(board_array[x] != n) return false;
      n++;
    };
  };
  return true;
}

Square _board::zero() {
  int x = 0;
  Square s;
  while(board_array[x]) x++;
  if(x >= _side * _side) throw std::logic_error("Could not find zero");
  s.y = x / _side;
  s.x = x % _side;
  return s;
}

double _board::manhattan_distance() {
  double dist = 0.0;
  for(int x = 0 ; x < _side * _side ; x++) {
    if(board_array[x] != 0) {
      int n = board_array[x];
      Square final((n-1)/_side, (n-1)%_side);
      Square current(x / _side, x % _side);
      dist += abs(final.x - current.x) + abs(final.y - current.y);
    };
  };
  return dist;
}

_board *_board::newBoard(Square from) {
  Square _z = zero();
  if(from.x < 0 || from.x >= _side || from.y < 0 || from.y >= _side) throw std::out_of_range("hmmm...1");
  if(_z.x < 0 || _z.x >= _side || _z.y < 0 || _z.y >= _side) throw std::out_of_range("hmmm...2");
  BOARD_TYPE *new_array = new BOARD_TYPE[_side*_side];
  std::memcpy(new_array, board_array, _side * _side * sizeof(BOARD_TYPE));
  new_array[_z.y * _side + _z.x] = new_array[from.y * _side + from.x];
  new_array[from.y * _side + from.x] = 0;
  return new Board(_side, new_array);
}

_state::_state(Board *board, Square action, double current_cost, _state *parent) {
  this->_board = board;
  this->_parent = parent;
  this->_action = (action.x == -1 ? NULL : new Square(action)); //(action ? new Square(action) : NULL);
  this->_current_cost = current_cost;
  this->_cost = 0;
  this->_children = NULL;
}

_state::~_state() {
  delete _board;
  if(_action != NULL) delete _action;
  if(_children != NULL) {
    for(int x = 0 ; x < _children->size() ; x++)
      delete (*_children)[x];
    delete _children;
  };
}

double _state::cost() {
  if(_cost > _current_cost || _action == NULL) return _cost;
  _cost = _current_cost;
  Board *b = this->_board->newBoard(*_action);
  _cost += b->manhattan_distance();
  delete b;
  return _cost;
}

int _state::depth() {
  if(!_parent) return 0;
  else return _parent->depth() + 1;
}

Board *_state::board() {
  return _board;
}

std::vector<_state *> _state::children() {
  std::vector<Square> transitions;
  if(_children != NULL) return *_children;

  _children = new std::vector<State *>();
  Square z = _board->zero();

  if(z.x != 0) //transitions.push_back(Square(z.x-1, z.y));
    _children->push_back(new State(_board->newBoard(Square(z.x-1, z.y)), Square(z.x-1, z.y), this->cost(), this));
  if(z.x < _board->side() - 1) //transitions.push_back(Square(z.x+1, z.y));
    _children->push_back(new State(_board->newBoard(Square(z.x+1, z.y)), Square(z.x+1, z.y), this->cost(), this));
  if(z.y != 0) //transitions.push_back(Square(z.x, z.y-1));
    _children->push_back(new State(_board->newBoard(Square(z.x, z.y-1)), Square(z.x, z.y-1), this->cost(), this));
  if(z.y < _board->side() - 1) //transitions.push_back(Square(z.x, z.y+1));
    _children->push_back(new State(_board->newBoard(Square(z.x, z.y+1)), Square(z.x, z.y+1), this->cost(), this));

//  for(std::vector<Square>::iterator it = transitions.begin() ; it != transitions.end(); ++it)
//    _children->push_back(new State(_board->newBoard(*it), &(*it), this->cost(), this));
  return *_children;
}

std::stack<State *> lifo_frontier;
std::priority_queue<State *> sorted_frontier;

std::vector<State *> solutions;

void show_solution(State *node) {
  if(!node->parent()) {
    std::cout << "----- SOLUTION -----" << std::endl;
    std::cout << "Starting board:" << std::endl;
    std::cout << node->board()->dump() << std::endl;
  } else {
    show_solution(node->parent());
    std::cout << "Make move " << node->action()->tostring() << std::endl;
  };
}

void breadth_first() {
  std::unordered_map<std::string, bool> explored;
  std::unordered_map<std::string, bool> frontier_dict;
  std::queue<State *> fifo_frontier;
  BOARD_TYPE static_array[3*3] = {8,4,7,1,6,3,5,0,2};
  Board *b = new Board(3, static_array);
  std::cout << b->dump();
  State *s = new State(b, NULL, 0, NULL);
  fifo_frontier.push(s);
  frontier_dict[s->board()->tostring()] = true;

  int max_depth = 0;
  int node_count = 0;

  while(!fifo_frontier.empty()) {
    State *s = fifo_frontier.front();
//    std::cout << s->tostring(false) << std::endl << std::flush;
    fifo_frontier.pop();
    frontier_dict.erase(s->board()->tostring());
    if(s->depth() > max_depth) {
      max_depth = s->depth();
      std::cout << "Evaluated " << node_count << " nodes, starting to evaluate depth " 
                << max_depth << ", and there are " << fifo_frontier.size() 
                << " nodes on the frontier" << std::endl << std::flush;
    };
    node_count++;
    explored.insert({s->board()->tostring(), true});
    std::vector<State *> children = s->children();
    for(std::vector<State *>::iterator child = children.begin() ; child != children.end(); ++child) {
      std::string str = (*child)->board()->tostring();
      if((*child)->board()->won()) {
        show_solution(*child);
        return;
      } else if(!explored[(*child)->board()->tostring()] && !frontier_dict[(*child)->board()->tostring()]) {
        fifo_frontier.push(*child);
        frontier_dict[s->board()->tostring()] = true;
      }
    }
  };
}

bool depth_first(State *s) {
  static int max_depth = 0;
  std::vector<State *> children = s->children();

  if(max_depth < s->depth()) {
    max_depth = s->depth();
    if(max_depth == 101) return false; // Force a stop at 100 moves
   std::cout << "Reached a depth of " << max_depth << std::endl;
  };
  //
  // For speed, check all of the children for a solution before we traverse entire subtrees
  // Just to find out a long time later that the solution was a child of this node!
  //
  for(std::vector<State *>::iterator child = children.begin() ; child != children.end(); ++child)
    if((*child)->board()->won()) {
      show_solution(*child);
      return true;
    };

  //
  // OK, here we go into the depths!
  //
  for(std::vector<State *>::iterator child = children.begin() ; child != children.end(); ++child)
    if(!(*child)->already_in_chain())
      if(depth_first((*child))) return true;

  std::cout << "Ran out of solutions at depth " << s->depth() << std::endl;
  return false;
}

void depth_first() {
  std::queue<State *> fifo_frontier;
  BOARD_TYPE static_array[3*3] = {8,4,7,1,6,3,5,0,2};
  Board *b = new Board(3, static_array);
  std::cout << b->dump();
  State *s = new State(b, NULL, 0, NULL);
  depth_first(s);
}

class {
  public:
    bool operator() (State *s1, State *s2) { return s1->cost() < s2->cost(); }
} costsort;
void a_star() {
  std::unordered_map<std::string, bool> explored;
  std::unordered_map<std::string, bool> frontier_dict;
  std::queue<State *> fifo_frontier;
  BOARD_TYPE static_array[3*3] = {8,4,7,1,6,3,5,0,2};
  Board *b = new Board(3); //3, static_array);
  std::cout << b->dump();
  State *s = new State(b, NULL, 0, NULL);
  fifo_frontier.push(s);
  frontier_dict[s->board()->tostring()] = true;

  int max_depth = 0;
  int node_count = 0;

  while(!fifo_frontier.empty()) {
    State *s = fifo_frontier.front();
    fifo_frontier.pop();
    frontier_dict.erase(s->board()->tostring());
    if(s->depth() > max_depth) {
      max_depth = s->depth();
      std::cout << "Evaluated " << node_count << " nodes, starting to evaluate depth " 
                << max_depth << ", and there are " << fifo_frontier.size() 
                << " nodes on the frontier" << std::endl << std::flush;
    };
    node_count++;
    explored.insert({s->board()->tostring(), true});
    std::vector<State *> children = s->children();
    std::sort(children.begin(), children.end(), costsort);
    for(std::vector<State *>::iterator child = children.begin() ; child != children.end(); ++child) {
      std::string str = (*child)->board()->tostring();
      if((*child)->board()->won()) {
        show_solution(*child);
        return;
      } else if(!explored[(*child)->board()->tostring()] && !frontier_dict[(*child)->board()->tostring()]) {
        fifo_frontier.push(*child);
        frontier_dict[s->board()->tostring()] = true;
      }
    }
  };
}

int main() {
  std::srand(std::time(NULL));
  //std::cout << "Breadth first" << std::endl;
  //breadth_first();
  //std::cout << "Depth first" << std::endl;
  //depth_first();
  std::cout << "A star" << std::endl;
  a_star();
  return 0;
}
