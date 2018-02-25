#ifndef __DEBUG__H__
#define __DEBUG__H__

#include "direction.h"

#include <string>

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

#endif // __DEBUG__H__
