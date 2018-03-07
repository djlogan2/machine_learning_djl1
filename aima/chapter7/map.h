#ifndef __MAP_H__
#define __MAP_H__

#include <iostream>
#include <utility>
#include <vector>

enum DIRECTION {
  DUP, DRIGHT, DDOWN, DLEFT //  specifically ordered for rotating!
};

inline DIRECTION direction_right(const DIRECTION &d) {
  if(d == DLEFT) return DUP;
  else return (DIRECTION)((int)d + 1);
}

inline DIRECTION direction_left(const DIRECTION &d) {
  if(d == DUP) return DLEFT;
  else return (DIRECTION)((int)d - 1);
}

#define SIZE 4
#define DJL //{if(first < -1000 || second < -1000 || first >= 1000 || second >= 1000) throw "here"; }

class XY : public std::pair<int, int> {
public:
 XY() { first = second = 0; }
 XY(int x, int y) { first=x;second=y; DJL; }
 friend std::ostream &operator<<(std::ostream &os, const XY &xy) {
    os << "XY[" << xy.first << ',' << xy.second << ']';
    return os;
 }
 const XY &operator+=(const XY &other) {
    first += other.first;
    second += other.second;
    DJL;
    return *this;
  };
 bool operator==(const XY &other) {
    return first==other.first && second == other.second;
  };
 const XY operator+(const XY &other) {
    return XY(first+other.first, second+other.second);
 };
 const XY adjacent(DIRECTION d) const {
    switch(d) {
      case DUP: return XY(first, second-1);
      case DDOWN: return XY(first, second+1);
      case DLEFT: return XY(first-1, second);
      case DRIGHT: return XY(first+1, second);
      default: throw "What?";
    };
 };
 const XY &move(DIRECTION d) {
    switch(d) {
      case DUP: second -= 1; break;
      case DDOWN: second += 1; break;
      case DLEFT: first -= 1; break;
      case DRIGHT: first += 1; break;
    };
    DJL;
    return *this;
 };
 const XY &reverse(DIRECTION d) {
    switch(d) {
      case DUP: second += 1; break;
      case DDOWN: second -= 1; break;
      case DLEFT: first += 1; break;
      case DRIGHT: first -= 1; break;
    };
    DJL;
    return *this;
 };
 const DIRECTION which_direction(const XY &other) {
    if(first != other.first) {
      if(second != other.second) return (DIRECTION)-1;
      return (first < other.first ? DRIGHT : DLEFT);
    } else if(second != other.second) {
      if(first != other.first) return (DIRECTION)-1;
      return (second < other.second ? DDOWN : DUP);
    } else
      return (DIRECTION)-1;
 };
};

enum Sensor {
STENCH  = 1,
BREEZE  = 2,
GLITTER = 4,
BUMP    = 8,
SCREAM  = 16,
WON     = 32,
DIED    = 64
};
inline Sensor operator|(const Sensor &a, const Sensor &b)
{ return (Sensor)((int)a | (int)b); }
inline Sensor &operator|=(Sensor &a, const int b)
{ return (Sensor&)((int&)a |= (int)b); }

enum ACTION {
  NONE,
  FORWARD,
  LEFT,  // 90deg
  RIGHT, // 90deg
  CLIMB,
  SHOOT,
  GRAB
};

enum SQUARE {
  EMPTY,
  PIT,
  WUMPUS,
  GOLD
};
inline SQUARE operator&(const SQUARE &a, const SQUARE &b)
{ return (SQUARE)((int)a & (int)b); }
inline SQUARE operator~(const SQUARE &a)
{ return (SQUARE)(~(int)a); }

class Map {
public:
  Map() { startover(); }
  Map(const std::vector<int> bytes);
  void startover();
  Sensor move(const ACTION action);
  void test_print();

protected:
  unsigned char _map[SIZE*SIZE];
  bool has_arrow;
  bool has_gold;
  bool wumpus_dead;
  XY current_location;
  XY current_direction;
  void at(XY loc, SQUARE s) { _map[loc.second*SIZE+loc.first] = (unsigned char)s; }
  SQUARE at(XY loc) { return (SQUARE)_map[loc.second*SIZE+loc.first]; }
  Sensor nothing();
  Sensor forward();
  Sensor shift(ACTION whichway);
  Sensor climb();
  Sensor shoot();
  Sensor grab();
};

#endif // __MAP_H__

