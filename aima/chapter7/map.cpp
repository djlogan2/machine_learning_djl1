#include "map.h"

#include <algorithm>
#include <cstring>
#include <iostream>

void Map::startover() {
  has_arrow = true;
  has_gold  = false;
  wumpus_dead = false;
  std::memset(_map, 0, SIZE*SIZE);
  current_location = XY(0, SIZE-1);
  current_direction = XY(1, 0);
  XY wumpus;
  XY gold;
  do {
    wumpus = XY(std::rand() % SIZE, std::rand() % SIZE);
    gold   = XY(std::rand() % SIZE, std::rand() % SIZE);
  } while(wumpus == gold || at(wumpus) || at(gold));
  at(wumpus, WUMPUS);
  at(gold, GOLD);

  for(int x = 0 ; x < SIZE ; x++)
    for(int y = 0 ; y < SIZE ; y++)
    if(!at(XY(x,y)))
      if((double)std::rand() / (double)RAND_MAX <= 0.20)
        at(XY(x,y), PIT);
}

Sensor Map::move(const ACTION action) {
  switch(action) {
    case NONE:    return nothing();
    case FORWARD: return forward();
    case LEFT:
    case RIGHT:   return shift(action);
    case CLIMB:   return climb();
    case SHOOT:   return shoot();
    case GRAB:    return grab();
    default:      throw "Invalid action";
  };
}

Sensor Map::nothing() {
  XY cl = current_location;
  Sensor ret = (Sensor)0;

  if(at(cl) == GOLD)
    ret = GLITTER;

  for(int x = -1 ; x <= 1 ; x++)
    for(int y = -1 ; y <= 1 ; y++)
      if(x == 0 || y == 0) {
        XY sq = current_location + XY(x,y);
        if(sq.first >= 0 && sq.second >= 0 && sq.first < SIZE && sq.second < SIZE &&
             (sq.first == cl.first || sq.second == cl.second))
          if(at(sq) == PIT)
            ret |= BREEZE;
          if(at(sq) == WUMPUS)
            ret |= STENCH;
      };
  return ret;
}

Sensor Map::forward() {
  XY cl = current_location + current_direction;
  if(cl.first < 0 || cl.second < 0 ||
     cl.first >= SIZE || cl.second >= SIZE)
    return nothing() | BUMP;

  current_location = cl;

  if(at(cl) == PIT) return DIED | BREEZE;
  if(!wumpus_dead && at(cl) == WUMPUS) return DIED | STENCH;

  return nothing();
}

Sensor Map::shift(ACTION whichway) {
  if(current_direction.first)
    current_direction = XY(0, (whichway==LEFT?-1:1)*current_direction.first);
  else
    current_direction = XY((whichway==LEFT?1:-1)*current_direction.second, 0);
  return nothing();
}

Sensor Map::climb() {
  if(current_location == XY(0, SIZE-1) && has_gold) return WON;
  else return nothing();
}

Sensor Map::shoot() {
  if(wumpus_dead) return nothing();

  XY cl = current_location;

  do {
    if(at(cl) == WUMPUS) {
      wumpus_dead = true;
      return nothing() | SCREAM;
    };
    cl += current_direction;
  } while(cl.first >= 0 && cl.first < SIZE &&
          cl.second >= 0 && cl.second < SIZE);

  return nothing();
}

Sensor Map::grab() {
  if(at(current_location) & GOLD) {
    has_gold = true;
    at(current_location, at(current_location) & ~GOLD);
  };
  return nothing();
}

void Map::test_print() {
  for(int y = 0 ; y < SIZE ; y++) {
    for(int x = 0 ; x < SIZE ; x++) {
      if(current_location == XY(x,y))
        std::cout << '@';
      else switch(at(XY(x,y))) {
        case EMPTY: std::cout << '.'; break;
        case PIT: std::cout << 'P'; break;
        case WUMPUS: std::cout << 'W'; break;
        case GOLD: std::cout << 'G'; break;
      };
    };
    std::cout << std::endl;
  };
  std::cout << std::flush;
}
