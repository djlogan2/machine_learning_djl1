#include "agent.h"
#include "map.h"

#include <iostream>
#include <ctime>

void manual() {
  std::srand(std::time(NULL));
  Map m;
  Sensor s = m.move(NONE);
  do {
    if(s & STENCH) std::cout << "You smell a stench" << std::endl << std::flush; 
    if(s & BREEZE) std::cout << "You feel a breeze" << std::endl << std::flush; 
    if(s & GLITTER) std::cout << "You see a glitter" << std::endl << std::flush; 
    if(s & BUMP) std::cout << "You bumped into a wall" << std::endl << std::flush; 
    if(s & SCREAM) std::cout << "You heard a scream" << std::endl << std::flush; 
    std::cout << "Move nflrcsg: " << std::endl << std::flush;
    if(s != WON && !(s & DIED)) {
redo:
      int c = std::getchar();
      switch(c) {
        case 10: goto redo; // Ignore the CR/LF
        case '?': m.test_print(); goto redo;
        case 'n': s = m.move(NONE);  break;
        case 'f': s = m.move(FORWARD); break;
        case 'l': s = m.move(LEFT); break;
        case 'r': s = m.move(RIGHT); break;
        case 'c': s = m.move(CLIMB); break;
        case 's': s = m.move(SHOOT); break;
        case 'g': s = m.move(GRAB); break;
        default: std::cout << "Illegal option. You can: nflrcsg" << std::endl << std::flush;
                 goto redo;
      };
    };
  } while(s != WON && !(s & DIED));
  if(s == WON) std::cout << "You won!" << std::flush << std::endl;
  else if(s & BREEZE) std::cout << "You fell into a pit and died, sorry" << std::flush << std::endl;
  else if(s & STENCH) std::cout << "You were captured by the wumpus and died, sorry" << std::flush << std::endl;
}

void doauto() {
  std::srand(std::time(NULL));
  Map m({0,0,0,3,1,2,1,1,0,0,0,0,0,0,0,0});
  Wumpus w;
  Sensor s = m.move(NONE);
  do {
    if(s & STENCH) std::cout << "You smell a stench" << std::endl << std::flush; 
    if(s & BREEZE) std::cout << "You feel a breeze" << std::endl << std::flush; 
    if(s & GLITTER) std::cout << "You see a glitter" << std::endl << std::flush; 
    if(s & BUMP) std::cout << "You bumped into a wall" << std::endl << std::flush; 
    if(s & SCREAM) std::cout << "You heard a scream" << std::endl << std::flush; 
    m.test_print();
    w.tell(s);
    w.print_map();
    w.print_node(0);
    s = m.move(w.ask());
  } while(s != WON && s != DIED);
  if(s == WON) std::cout << "You won!" << std::flush << std::endl;
  else if(s & BREEZE) std::cout << "You fell into a pit and died, sorry" << std::flush << std::endl;
  else if(s & STENCH) std::cout << "You were captured by the wumpus and died, sorry" << std::flush << std::endl;
}

int main() {
  //manual();
  doauto();
}
