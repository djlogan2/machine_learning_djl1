#include "ac3.h"
#include "backtrack.h"

int main() {
  //setup("send", "more", "money"); // 9567+1085=10652
  //setup("two", "two", "four"); //  Seven solutions
  //setup("ab", "ab", "cbd");
  //AC3 ac3("send", "more", "money");
  //if(!ac3.run())
  //  std::cout << "No solution" << std::endl;
  //ac3.print_variables();
  Backtrack bt("two", "two", "four");  
  bt.solve();
  bt.print_variables();
//  std::cout << "---- Final variables ----" << std::endl << std::flush;
//  for(auto it = variables.begin() ; it != variables.end() ; ++it) {
//    std::cout << it->second << std::endl << std::flush;
//  };
}
