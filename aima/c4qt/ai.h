#ifndef AI_H
#define AI_H

#include <vector>

class AI {
public:
    int previous_state;
    int previous_action;
    virtual int nextaction(std::vector<int> legalmoves) = 0;
};

#endif // AI_H
