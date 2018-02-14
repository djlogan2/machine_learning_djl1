#include <iostream>
#include <vector>

int width = 100;
int height = 100;

#define LEFT  0
#define UP    1
#define RIGHT 2
#define DOWN  3
#define GENERATING 0x10
#define ENDPATH    0x20

unsigned char **board;

int reverse(int action) {
  switch(action) {
    case UP:    return DOWN;
    case DOWN:  return UP;
    case LEFT:  return RIGHT;
    case RIGHT: return LEFT;
  };
  return 0;
}

bool move(int previous_action, int current_state) {
  int r = current_state/width;
  int c = current_state%width;

  if(board[r][c] | ENDPATH) {
    board[r][c] = reverse(previous_action) | ENDPATH;
    return true;
  };

  std::vector<int> options = {{LEFT, UP, RIGHT, DOWN}};

  while(options.size()) {
    int i = std::rand() % options.size();
    int next_action = options[i];
    options.erase(options.begin() + i);
    int next_state = current_state;
    switch(next_action) {
      case LEFT:
        next_state--; break;
      case UP:
        next_state -= width; break;
      case RIGHT:
        next_state ++; break;
      case DOWN:
        next_state += width; break;
    };
    int nr = next_state/width;
    int nc = next_state%width;
    if(next_state >= 0 && next_state < width * height && !board[nr][nc]) {
      board[r][c] = GENERATING;
      if(move(next_action, next_state, goal)) {
        board[r][c] |= (next_action | reverse(previous_action) | ENDPATH);
        board[r][c] &= (0xFF-GENERATING);
        return true;
      } else
        board[r][c] = 0;
    };
  };
  return false; // We ran out of moves
}

int main() {
  std::memset(board, 0, width * height);
  int start = (std::rand() % height) * width + (width - 1);
  int end   = (std::rand() % height) * width;
  board[end/width][end%width] = ENDPATH;
  if(!move(0, start)) {
    std::cout << "We could not generate an initial path for some reason" << std::endl << std::flush;
    exit(0);
  };
}
