#ifndef LRTASTAR_H
#define LRTASTAR_H

#include "ai.h"
#include "nodemap.h"
#include "map.h"

#include <ostream>
#include <unordered_map>
#include <vector>

class LRTAStar : public AI
{
public:
    LRTAStar(Map *m);
    virtual int nextaction(std::vector<int> legalmoves);
protected:
    int current_r = 0;
    int current_c = 0;
    NodeMap current_nodes;
    NodeMap known_nodes;
    Map *m;

    void dont_know_where_we_are() {
        current_nodes.clear();
        current_r = current_c = 0;
    }

    int current_state() { return current_r * current_nodes.width + current_c; }
    int minimum_cost(int whichstate);
    int minimum_cost_action(int whichstate);
    void update_scores(int previous_state, int state, int score);
};

#endif // LRTASTAR_H
