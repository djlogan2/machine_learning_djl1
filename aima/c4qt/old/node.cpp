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

int Node::state_from_action(DIRECTION dir) {
    auto nodemoveptr = std::find_if(std::begin(resultstates), std::end(resultstates),
                       [dir](NodeMove nm) {return nm.move==dir;});
    if(nodemoveptr != resultstates.end())
      return nodemoveptr->result_state;
    else
      throw "We can't return anything valid here";
}

DIRECTION Node::action(int towhichstate) {
    auto nodemoveptr = std::find_if(std::begin(resultstates), std::end(resultstates),
                       [towhichstate](NodeMove nm) {return nm.result_state==towhichstate;});
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
  std::vector<int> return_states;
  std::transform(resultstates.begin(), resultstates.end(), std::back_inserter(return_states),
            [](NodeMove nm) -> int { return nm.result_state; });
  return return_states;
}

bool Node::moves_match(std::vector<DIRECTION> legalmoves) {
  if(legalmoves.size() != resultstates.size()) return false;
  for(auto nm : resultstates)
    if(std::find(legalmoves.begin(), legalmoves.end(), nm.move) == legalmoves.end())
      return false;
  return true;
}

int Node::legalmovesbyte() {
  int lmb = 0;
  for(NodeMove nm : resultstates) lmb |= (int)nm.move;
  return lmb;
}
