#ifndef LRTASTAR_H
#define LRTASTAR_H

#include "ai.h"       // for AI
#include "nodemap.h"
#include "map.h"      // for DIRECTION, Map (ptr only)

#include <vector>

class LRTAStar : public AI
{
public:
    LRTAStar(Map *m);
    virtual DIRECTION nextaction(std::vector<DIRECTION> legalmoves);
protected:
    int current_r = 0;
    int current_c = 0;
    NodeMap current_nodes;
    NodeMap known_nodes;
    Map *m;

    DIRECTION previous_action;
    int current_state() { return current_r * current_nodes.width + current_c; }
    int minimum_cost(int whichstate);
    DIRECTION minimum_cost_action(int whichstate);
    void update_scores(int previous_state, int state, int score);
};

#endif // LRTASTAR_H
