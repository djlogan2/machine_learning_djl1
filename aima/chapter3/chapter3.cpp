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

#define DELETE_CHILDREN

#define BOARD_TYPE unsigned char
#define max(a, b) (a > b ? a : b)
#define min(a, b) (a < b ? a : b)

//
// Branching factor:
//    0=−b*(N+1)+N+b^(d+1)
//    b = The branching factor we are looking for
//    N = The number of nodes evaluated to get to the solution
//    d = The depth of the tree (i.e. the number of moves to get to the solution)
//
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

typedef class _state {
  public:
    _state(int side);
    _state(int side, BOARD_TYPE *board_array);
    ~_state();
    
    _state *newState(Square from);
    bool solvable();
    bool won();
    Square zero();
    void print();
    int something();
    int manhattan_distance();

    inline bool same(_state *other) { return memcmp(board_array, other->board_array, _side * _side) == 0; }
    BOARD_TYPE *board() { return board_array; }
    int side() { return _side; }
    std::string tostring() {std::string r((const char *)board_array, _side*_side*sizeof(BOARD_TYPE)); return r; }
    std::string dump() {
      std::ostringstream ss;
      for(int y = 0 ; y < _side ; y++) {
        ss << '[';
        for(int x = 0 ; x < _side ; x++) {
          ss << (int)board_array[y * _side + x] << ',';
        };
        ss << ']';
      };
      ss << ",side=" << _side << ", zero=" << zero().tostring();
      return ss.str();
    }

  private:
    BOARD_TYPE *board_array;
    int _side;
} State;

typedef class _node {
  public:
    _node(State *board, Square action, _node *parent);
    ~_node();
    int cost();
    int depth();
    int f;
    _node *parent() { return _parent; }
    Square *action() { return _action; }
    bool already_in_chain() {
      if(!_parent) return false;
      return _parent->already_in_chain(this);
    };
    State *board();
    std::vector<_node *> children();
#ifdef DELETE_CHILDREN
    void delete_children();
#endif
    std::string tostring(bool printchildren=false) {
      std::ostringstream ss;
      ss << "Node: " << this << std::endl;
      ss << _state->dump() << std::endl;
      ss << "   Action=" << (_action ? _action->tostring() : "No action");
      ss << ", parent=" << _parent;
      ss << ", cost=" << cost();
      ss << ", f=" << f << '|' << std::endl;
      if(printchildren) {
        std::vector<_node *> children = this->children();
        for(std::vector<_node *>::iterator child = children.begin() ; child != children.end(); ++child) {
          ss << "      child=" << (*child)->tostring(false) << std::endl;
        };
      };
      return ss.str();
    };

  private:
    State *_state;
    Square *_action;
    _node *_parent;
    std::vector<_node *> *_children;
    double _cost;

    bool already_in_chain(_node *other) {
      if(_state->same(other->_state))
        return true;
      if(!_parent) return false;
      return _parent->already_in_chain(other);
    }
} Node;

_state::_state(int side) {
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

_state::_state(int side, BOARD_TYPE *board_array) {
  this->_side = side;
  this->board_array = board_array;
}

_state::~_state() {
  delete[] board_array;
}

bool _state::solvable() {
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

bool _state::won() {
  int n = 1;
  for(int x = 0 ; x < _side * _side ; x++) {
    if(board_array[x] != 0) {
      if(board_array[x] != n) return false;
      n++;
    };
  };
  return true;
}

Square _state::zero() {
  int x = 0;
  Square s;
  while(board_array[x]) x++;
  if(x >= _side * _side) throw std::logic_error("Could not find zero");
  s.y = x / _side;
  s.x = x % _side;
  return s;
}

int _state::manhattan_distance() {
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

int _state::something() {
  static int row[] = {3,0,0,0,0,1,1,1,1,2,2,2,2,3,3,3};
  static int col[] = {3,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2};

  if(_side != 4)
    return manhattan_distance();

  int r=0;
  for(int i=0;i<4;i++)
    for(int j=0;j<4;j++)
      if(board_array[i+j*4]!=0)
        r+=abs(row[board_array[i+j*4]]-i)+abs(col[board_array[i+j*4]]-j);
  return r;
}

_state *_state::newState(Square from) {
  Square _z = zero();
  if(from.x < 0 || from.x >= _side || from.y < 0 || from.y >= _side) throw std::out_of_range("hmmm...1");
  if(_z.x < 0 || _z.x >= _side || _z.y < 0 || _z.y >= _side) throw std::out_of_range("hmmm...2");
  BOARD_TYPE *new_array = new BOARD_TYPE[_side*_side];
  std::memcpy(new_array, board_array, _side * _side * sizeof(BOARD_TYPE));
  new_array[_z.y * _side + _z.x] = new_array[from.y * _side + from.x];
  new_array[from.y * _side + from.x] = 0;
  return new State(_side, new_array);
}

_node::_node(State *board, Square action, _node *parent) {
  this->_state = board;
  this->_parent = parent;
  this->_action = (action.x == -1 ? NULL : new Square(action));
  this->_cost = 0;
  this->_children = NULL;
  this->f = 0;
}

_node::~_node() {
  delete _state;
  if(_action != NULL) delete _action;
#ifdef DELETE_CHILDREN
  delete_children();
}

void _node::delete_children() {
#endif
  if(_children != NULL) {
    for(int x = 0 ; x < _children->size() ; x++)
      delete (*_children)[x];
    delete _children;
    _children = NULL;
  };
}

int _node::cost() {
  if(_cost > depth() || _action == NULL) return _cost;
  _cost = depth();
  State *b = this->_state->newState(*_action);
  //_cost += b->something();
  _cost += b->manhattan_distance();
  //std::cout << "::cost returning " << _cost << ", manhattan_distance=" << b->manhattan_distance() << std::endl << std::flush;
  delete b;
  return _cost;
}

int _node::depth() {
  if(!_parent) return 0;
  else return _parent->depth() + 1;
}

State *_node::board() {
  return _state;
}

std::vector<_node *> _node::children() {
  std::vector<Square> transitions;
  if(_children != NULL) return *_children;

  _children = new std::vector<Node *>();
  Square z = _state->zero();

  if(z.x != 0)
    _children->push_back(new Node(_state->newState(Square(z.x-1, z.y)), Square(z.x-1, z.y), this));
  if(z.x < _state->side() - 1)
    _children->push_back(new Node(_state->newState(Square(z.x+1, z.y)), Square(z.x+1, z.y), this));
  if(z.y != 0)
    _children->push_back(new Node(_state->newState(Square(z.x, z.y-1)), Square(z.x, z.y-1), this));
  if(z.y < _state->side() - 1)
    _children->push_back(new Node(_state->newState(Square(z.x, z.y+1)), Square(z.x, z.y+1), this));

  return *_children;
}

std::stack<Node *> lifo_frontier;
std::priority_queue<Node *> sorted_frontier;

std::vector<Node *> solutions;

void show_solution(Node *node) {
  if(!node->parent()) {
    std::cout << "----- SOLUTION -----" << std::endl;
    std::cout << "Starting board:" << std::endl;
    std::cout << node->board()->dump() << std::endl;
  } else {
    show_solution(node->parent());
    std::cout << "Make move " << node->depth() << ' ' << node->action()->tostring() << " : " << node->board()->dump() << std::endl;
  };
}

void breadth_first() {
  std::unordered_map<std::string, bool> explored;
  std::unordered_map<std::string, bool> frontier_dict;
  std::queue<Node *> fifo_frontier;
  BOARD_TYPE static_array[3*3] = {8,4,7,1,6,3,5,0,2};
  State *b = new State(3, static_array);
  std::cout << b->dump();
  Node *s = new Node(b, NULL, /*0, */ NULL);
  fifo_frontier.push(s);
  frontier_dict[s->board()->tostring()] = true;

  int max_depth = 0;
  int node_count = 0;

  while(!fifo_frontier.empty()) {
    Node *s = fifo_frontier.front();
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
    std::vector<Node *> children = s->children();
    for(std::vector<Node *>::iterator child = children.begin() ; child != children.end(); ++child) {
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

bool depth_first(Node *s) {
  static int max_depth = 0;
  std::vector<Node *> children = s->children();

  if(max_depth < s->depth()) {
    max_depth = s->depth();
    if(max_depth == 101) return false; // Force a stop at 100 moves
   std::cout << "Reached a depth of " << max_depth << std::endl;
  };
  //
  // For speed, check all of the children for a solution before we traverse entire subtrees
  // Just to find out a long time later that the solution was a child of this node!
  //
  for(std::vector<Node *>::iterator child = children.begin() ; child != children.end(); ++child)
    if((*child)->board()->won()) {
      show_solution(*child);
      return true;
    };

  //
  // OK, here we go into the depths!
  //
  for(std::vector<Node *>::iterator child = children.begin() ; child != children.end(); ++child)
    if(!(*child)->already_in_chain())
      if(depth_first((*child))) return true;

  std::cout << "Ran out of solutions at depth " << s->depth() << std::endl;
  return false;
}

void depth_first() {
  std::queue<Node *> fifo_frontier;
  BOARD_TYPE static_array[3*3] = {8,4,7,1,6,3,5,0,2};
  State *b = new State(3, static_array);
  std::cout << b->dump();
  Node *s = new Node(b, NULL, NULL);
  depth_first(s);
}

class {
  public:
    bool operator() (Node *s1, Node *s2) { return s1->cost() < s2->cost(); }
} costsort;
class {
  public:
    bool operator() (Node *s1, Node *s2) { return s1->f < s2->f; }
} fsort;

void a_star() {
  std::unordered_map<std::string, bool> explored;
  std::unordered_map<std::string, bool> frontier_dict;
  std::queue<Node *> fifo_frontier;
  // 10 moves BOARD_TYPE static_array[4*4] = {1,3,6,4,5,2,8,11,9,10,7,12,13,14,15,0}; //{2,0,4,13,9,12,11,1,8,6,7,3,15,10,14,5};
  BOARD_TYPE static_array[4*4] = {1,3,0,4,2,10,6,11,5,7,8,12,9,13,14,15};
  State *b = new State(4, static_array);
  std::cout << b->dump();
  Node *s = new Node(b, NULL, NULL);
  fifo_frontier.push(s);
  frontier_dict[s->board()->tostring()] = true;

  int max_depth = 0;
  int node_count = 0;

  while(!fifo_frontier.empty()) {
    Node *s = fifo_frontier.front();
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
    std::vector<Node *> children = s->children();
    std::sort(children.begin(), children.end(), costsort);
    for(std::vector<Node *>::iterator child = children.begin() ; child != children.end(); ++child) {
      std::string str = (*child)->board()->tostring();
      if((*child)->board()->won()) {
        show_solution(*child);
        std::cout << "Evaluated " << node_count << " nodes, starting to evaluate depth " 
                  << max_depth << ", and there are " << fifo_frontier.size() 
                  << " nodes on the frontier" << std::endl << std::flush;
        return;
      } else if(!explored[(*child)->board()->tostring()] && !frontier_dict[(*child)->board()->tostring()]) {
        fifo_frontier.push(*child);
        frontier_dict[s->board()->tostring()] = true;
      }
    }
  };
}

int rbfs(Node *s, int f_limit) {
  static int max_depth = 0;
  static unsigned long long nodes_checked = 0;

  if(s->depth() > max_depth) {
    max_depth = s->depth();
    std::cout << "Checking depth " << max_depth << " with limit " << f_limit << std::endl << std::flush;
  };
  if(++nodes_checked % 10000000 == 0)
    std::cout << "Checking " << nodes_checked << " nodes, current limit " << f_limit << ", current_depth " << s->depth() << ", max depth " << max_depth << std::endl << std::flush;
  
  if(s->board()->won()) {
    show_solution(s);
    return -2;
  };

  std::vector<Node *> children = s->children();
  if(children.empty()) return -1;


  for(std::vector<Node *>::iterator child = children.begin() ; child != children.end(); ++child) {
    (*child)->f = max((*child)->cost(), s->f);
  };

  Node *best = NULL;

  while(true) {
    std::vector<Node *>::iterator child = children.begin();
    std::sort(children.begin(), children.end(), fsort);

    best = (*child);
    int alternative = (*child)->f;

    for(std::vector<Node *>::iterator c = (child + 1) ; c != children.end(); ++c) {
      if((*c)->f > best->f) {
        alternative = ((*c)->f);
        break;
      };
    };

    if(best->f > f_limit) {
      int v = best->f;
#ifdef DELETE_CHILDREN
      s->delete_children();
#endif
      return v;
    };
    best->f = rbfs(best, min(f_limit, alternative));
    if(best->f < 0) {
      int v = best->f;
#ifdef DELETE_CHILDREN
      s->delete_children();
#endif
      return v;
    };
  };
}

void rbfs() {
  BOARD_TYPE static_array[3*3] = {7,3,2,8,5,4,1,0,6};
  State *b = new State(4); //3, static_array);
  std::cout << b->dump();
  Node *s = new Node(b, NULL, /* 0, */ NULL);
  rbfs(s, 999999999);
  delete s;
}

int main() {
  std::srand(std::time(NULL));
  //std::cout << "Breadth first" << std::endl;
  //breadth_first();
  //std::cout << "Depth first" << std::endl;
  //depth_first();
  //std::cout << "A star" << std::endl;
  a_star();
  //rbfs();
  return 0;
}
