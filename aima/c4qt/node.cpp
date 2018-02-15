#include "node.h"

#include <algorithm>

int add_to_offset(int offset, DIRECTION where, int howmany, int width) {
    switch(where) {
    case UP:
        return offset+howmany*width;
    case LEFT:
        return offset+howmany*(offset/width+1);
    case RIGHT:
        return offset+howmany*(offset/width);
    default:
        return offset;
    };
}

void Node::add_to_states(DIRECTION where, int howmany, int width) {
    _state = add_to_offset(_state, where, howmany, width);
    std::transform(resultstates.begin(), resultstates.end(), resultstates.begin(),
       [where, howmany, width](NodeMove nm) -> NodeMove {
        return NodeMove(add_to_offset(nm.result_state, where, howmany, width), nm.move);
     });
}

DIRECTION Node::action(int towhichstate) {
    auto nodemoveptr = std::find_if(std::begin(resultstates), std::end(resultstates), [towhichstate](NodeMove nm) {return nm.result_state==towhichstate;});
    if(nodemoveptr != resultstates.end())
      return nodemoveptr->move;
    else {
      return STOP;
    };
}

Node::Node(int width, int state, int cost, std::vector<DIRECTION> legalmoves) {
    this->_state = state ;
    this->_cost = cost;
    for(auto move : legalmoves) {
        int newstate = state;
        switch(move) {
        case STOP:
        case TELEPORT:
            throw "We should not have this move!";
        case UP:
            newstate -= width;
            break;
        case DOWN:
            newstate += width;
            break;
        case LEFT:
            newstate--;
            break;
        case RIGHT:
            newstate++;
            break;
        }
        resultstates.push_back(NodeMove(newstate, move));
    }
}

std::vector<int> Node::availablestates()
{
    std::vector<int> ret;
    for(auto nm : resultstates)
        ret.push_back(nm.result_state);
    std::random_shuffle(ret.begin(), ret.end());
    return ret;
}

