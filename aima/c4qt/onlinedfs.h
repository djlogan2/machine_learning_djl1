#ifndef ONLINEDFS_H
#define ONLINEDFS_H

#include "ai.h"
#include "map.h"
#include "mapfunctions.h"

#include <unordered_map>

class OnlineDFS : public AI
{
public:
    OnlineDFS(Map *m);
    virtual int nextaction();
protected:
    std::unordered_map<int, std::vector<int>> untried;
    std::unordered_map<std::array<int, 2>, int, hashing_func, key_equal_func> results;
    std::unordered_map<int, std::vector<int>> unbacktracked;
    Map *m;

    int previous_state = -1;
    int previous_action = -1;
};

#endif // ONLINEDFS_H
