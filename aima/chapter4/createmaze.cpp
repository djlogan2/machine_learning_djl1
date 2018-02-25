#include <cstring>
#include <ctime>
#include <iostream>
#include <vector>

#define WIDTH 50
#define HEIGHT 50

#define LEFT  1
#define UP    2
#define RIGHT 4
#define DOWN  8
#define GENERATING 0x10
#define ENDPATH    0x20

unsigned char board[HEIGHT][WIDTH];

int reverse(int action) {
  switch(action) {
    case UP:    return DOWN;
    case DOWN:  return UP;
    case LEFT:  return RIGHT;
    case RIGHT: return LEFT;
  };
  return 0;
}

int depth = 0;
int maxdepth = 0;
bool move(int previous_action, int current_state) {
  if(++depth > maxdepth) {
    std::cout << "Depth " << depth << std::endl << std::flush;
    maxdepth = depth;
  };
  int r = current_state/WIDTH;
  int c = current_state%WIDTH;

  if(board[r][c] & GENERATING) {
    std::cout << depth << ":  " << "Square " << current_state << " is already in the generated path, return" << std::endl << std::flush;
    depth--;
    return false;
  };

  if(board[r][c] & ENDPATH) {
    std::cout << depth << ":  " << "Square " << current_state << " is a path to the end, so great!" << std::endl << std::flush;
    board[r][c] |= reverse(previous_action);
    depth--;
    return true;
  };

  std::vector<int> options = {{DOWN, UP, RIGHT, LEFT}};
  while(options.size()) {
    int i = std::rand() % options.size();
    int next_action = options[i];
    options.erase(options.begin() + i);
    int nr = r;
    int nc = c;
    switch(next_action) {
      case LEFT:
        nc--; break;
      case UP:
        nr--; break;
      case RIGHT:
        nc++; break;
      case DOWN:
        nr++; break;
    };
    if(nr >= 0 && nr < HEIGHT && nc >=0 && nc < WIDTH) {
      if(board[r][c]) throw "It should be zero";
      board[r][c] = (next_action | reverse(previous_action) | GENERATING);
      std::cout << depth << ":  " << "Trying to move " << next_action << " from " << current_state << " to " << (nr*WIDTH+nc) << std::endl << std::flush;
      if(move(next_action, nr*WIDTH+nc)) {
        std::cout << depth << ":  " << "Succeeded in moving " << next_action << " from " << current_state << " to " << (nr*WIDTH+nc) << std::endl << std::flush;
        board[r][c] &= 0xFF - GENERATING;
        board[r][c] |= ENDPATH;
        depth--;
        return true;
      } else {
        std::cout << depth << ":  " << "Failed to move " << next_action << " from " << current_state << " to " << (nr*WIDTH+nc) << std::endl << std::flush;
        board[r][c] = 0;
      };
    };
  };
  depth--;
  return false; // We ran out of moves
}

void print_board(int height, int width, int start, int end, unsigned char *board)
{
  std::cout << "current_location = " << start << ';' << std::endl;
  std::cout << "goal = " << end << ';' << std::endl;
  std::cout << "_width = " << width << ';' << std::endl;
  std::cout << "map = {{";
  for(int h = 0 ; h < HEIGHT ; h++) {
    std::string comma = "";
    for(int w = 0 ; w < WIDTH ; w++) {
      std::cout << comma;
      comma = ",";
      std::string plus = "";
      if(board[h*WIDTH+w] & LEFT) { std::cout << plus << "LEFT"; plus = "+"; }
      if(board[h*WIDTH+w] & UP) { std::cout << plus << "UP"; plus = "+"; }
      if(board[h*WIDTH+w] & RIGHT) { std::cout << plus << "RIGHT"; plus = "+"; }
      if(board[h*WIDTH+w] & DOWN) { std::cout << plus << "DOWN"; plus = "+"; }
      if(plus != "+") std::cout << "0";
    };
    std::cout << ',' << std::endl;
  };
  std::cout << "}};" << std::endl << std::flush;
}

std::vector<int> getunmazed(unsigned char *board) {
  std::vector<int> unmazed;
  for(int x = 0 ; x < HEIGHT * WIDTH ; x++)
    if(!(board[x] & ENDPATH))
      unmazed.push_back(x);
  return unmazed;
}

int main() {
  std::srand(std::time(NULL));
  std::memset(board, 0, WIDTH * HEIGHT);
  int end   = (std::rand() % HEIGHT) * WIDTH;
  board[end/WIDTH][end%WIDTH] = ENDPATH;
  int next;
  int start = next = (std::rand() % HEIGHT) * WIDTH + (WIDTH - 1);
  while(true) {
    if(!move(0, next)) {
      std::cout << "We could not generate an initial path for some reason" << std::endl << std::flush;
      exit(0);
    };
    std::vector<int> unmazed = getunmazed((unsigned char *)board);
    if(!unmazed.size()) {
      print_board(HEIGHT, WIDTH, start, end, (unsigned char *)board);
      return 0;
    };
    next = unmazed[std::rand() % unmazed.size()];
    std::cout << unmazed.size() << " squares remain" << std::endl << std::flush;
  };
}
