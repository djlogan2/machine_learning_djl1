#ifndef __MAP_H__
#define __MAP_H__

#include <utility>

class XY : public std::pair<int, int> {
public:
 XY() { first = second = 0; }
 XY(int x, int y) { first=x;second=y; }
 const XY &operator+=(const XY &other) {
    first += other.first;
    second += other.second;
    return *this;
  };
 const XY operator+(const XY &other) {
    return XY(first+other.first, second+other.second);
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

enum DIRECTION {
  DUP, DRIGHT, DDOWN, DLEFT //  specifically ordered for rotating!
};

#define SIZE 4

class Map {
public:
  Map() { startover(); }
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

