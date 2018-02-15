#ifndef MAP_H
#define MAP_H

#include <array>   // for array
#include <string>  // for string
#include <vector>

enum DIRECTION {
  STOP     = 0,
  LEFT     = 1,
  UP       = 2,
  RIGHT    = 4,
  DOWN     = 8,
  TELEPORT = 16
};
//#define STOP  0
//#define LEFT  1
//#define UP    2
//#define RIGHT 4
//#define DOWN  8
//#define TELEPORT 16

namespace Debug {
inline std::string d(int dir) {
    switch(dir) {
    case STOP:     return "STOP";
    case LEFT:     return "LEFT";
    case UP:       return "UP";
    case RIGHT:    return "RIGHT";
    case DOWN:     return "DOWN";
    case TELEPORT: return "TELEPORT";
    default:       return "??ACTION??";
    }
}
}

class Map {
  private:
    std::array<int, 50*50> map;
    int current_location;
    int goal;
    int _moves;
    int _width;

  public:
    int moves(int row, int col);
    bool won() const { return current_location == goal; }
    // We don't have an "at" if we are doing lrta_star
    bool isgoal(int row, int col) const { return row * _width + col == goal; }
    //int moves() const { return _moves; }
    int width() const { return _width; }
    std::vector<DIRECTION> available_actions() const;
    Map();
    DIRECTION reverse_move(DIRECTION move);
    int move(DIRECTION _move);
    bool _at(int row, int col) const { return row * _width + col == current_location; }
    int _at() const { return current_location; }
protected:
};

#endif // MAP_H
