#include "ac3.h"
#include "backtrack.h"

int main(int argc, char **argv) {
  //AC3 ac3("send", "more", "money");
  //if(!ac3.run())
  //  std::cout << "No solution" << std::endl;
  //ac3.print_variables();
  //Backtrack bt("two", "two", "four"); //  Seven solutions
  Backtrack bt(argv[1], argv[2], argv[3]);
  //Backtrack bt("send", "more", "money");
  //Backtrack bt("abccdefc", "chgbicgd", "bfeaegeg");
  //Backtrack bt("davidd", "davidl", "carolyn");   //  davidd
                                              //  davidl
                                              // carolyn
  bt.solve();
  //bt.print_variables();
//  std::cout << "---- Final variables ----" << std::endl << std::flush;
//  for(auto it = variables.begin() ; it != variables.end() ; ++it) {
//    std::cout << it->second << std::endl << std::flush;
//  };
}
