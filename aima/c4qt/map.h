#ifndef MAP_H
#define MAP_H

#include "direction.h"

#include <array>   // for array
#include <string>  // for string
#include <vector>

class Map {
  public:
    int moves(int row, int col) {return map[row*_width + col];}
    bool won() const { return current_location == goal; }
    bool isgoal(int row, int col) const { return row * _width + col == goal; }
    int width() const { return _width; }
    int height() const { return map.size() / _width; }
    int size() const { return map.size(); }
    std::vector<DIRECTION> available_actions() const { return available_actions(current_location); }
    Map();
    int move(DIRECTION _move);
    bool _at(int row, int col) const { return row * _width + col == current_location; }
    int _at() const { return current_location; }
    //int tostate(int fromstate, DIRECTION _move);

protected:
    //void generate_scores(std::vector<int> *visited, NodeMap *nm, int state, int score);
    std::vector<DIRECTION> available_actions(int atstate) const;
    std::array<int, 50*50> map;
    int current_location;
    int goal;
    //int _moves;
    int _width;

};

#endif // MAP_H
