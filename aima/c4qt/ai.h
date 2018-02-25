#ifndef AI_H
#define AI_H

#include "direction.h"

#include <vector>

class AI {
public:
//    virtual int previous_state() { return 0; }
//    virtual int previous_action() { return 0; }
    virtual DIRECTION nextaction(std::vector<DIRECTION> legalmoves) = 0;
    virtual void atgoal(std::vector<DIRECTION> legalmoves) = 0;
};

#endif // AI_H
