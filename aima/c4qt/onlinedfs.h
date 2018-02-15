#ifndef ONLINEDFS_H
#define ONLINEDFS_H

#include "ai.h"   // for AI
#include "map.h"  // for Map (ptr only), DIRECTION
#include "mapfunctions.h"

#include <unordered_map>

class OnlineDFS : public AI
{
public:
    OnlineDFS(Map *m);
    virtual int nextaction();
protected:
    std::unordered_map<int, std::vector<DIRECTION>> untried;
    std::unordered_map<std::array<int, 2>, DIRECTION, hashing_func, key_equal_func> results;
    std::unordered_map<int, std::vector<int>> unbacktracked;
    Map *m;

    int previous_state = -1;
    DIRECTION previous_action;
};

#endif // ONLINEDFS_H
