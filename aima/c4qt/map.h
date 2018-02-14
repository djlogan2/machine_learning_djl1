#ifndef MAP_H
#define MAP_H

#include <array>
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
    int moves(int row, int col);
    bool won() const { return current_location == goal; }
    bool at(int row, int col) const { return row * _width + col == current_location; }
    bool isgoal(int row, int col) const { return row * _width + col == goal; }
    int at() const { return current_location; }
    int moves() const { return _moves; }
    int width() const { return _width; }
    std::vector<int> available_actions() const;
    Map();
    int reverse_move(int move);
    int move(int _move);
};

#endif // MAP_H
