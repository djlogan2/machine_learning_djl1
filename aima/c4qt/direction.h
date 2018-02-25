#ifndef __DIRECTION_H__
#define __DIRECTION_H__

enum DIRECTION {
  STOP     = 0,
  LEFT     = 1,
  UP       = 2,
  RIGHT    = 4,
  DOWN     = 8,
  TELEPORT = 16,
};

inline DIRECTION reverse_move(DIRECTION move) {
 switch(move) {
    default:
      throw "Invalid move";
    case UP:
      return DOWN;
    case DOWN:
      return UP;
    case LEFT:
      return RIGHT;
    case RIGHT:
      return LEFT;
  };
};

#endif // __DIRECTION_H__

