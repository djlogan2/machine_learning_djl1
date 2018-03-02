#include "backtrack.h"

#include <algorithm>

void Backtrack::add_variables(std::array<std::string, 5> ops) {
  for(std::string v : ops) {
    if(variables.find(v) == variables.end())
      variables.emplace(v, Var(v, {0,1,2,3,4,5,6,7,8,9}));
  };
}

Backtrack::Backtrack(std::string s1, std::string s2, std::string s3) {
  int temp = 0;
  variables.emplace("ZERO", Var(0));
  std::vector<std::string> letters;
  std::reverse(s1.begin(), s1.end());
  std::reverse(s2.begin(), s2.end());
  std::reverse(s3.begin(), s3.end());
  int chars = std::max(std::max(s1.size(), s2.size()), s3.size());
  std::array<std::string, 5> ops;
  ops[3] = "ZERO";
  ops[4] = "C" + std::to_string(temp++);
  for(int x = 0 ; x < chars ; x++) {
    //
    // For every letter in each of the three words,
    //   we get the letter, or the constant value "ZERO" in each column
    //
    // The carry in and carry out variables are above
    //
    ops[0] = (x >= s1.size() ? "ZERO" : std::string(1, s1[x]));
    ops[1] = (x >= s2.size() ? "ZERO" : std::string(1, s2[x]));
    ops[2] = (x >= s3.size() ? "ZERO" : std::string(1, s3[x]));

    for(int x = 0 ; x < 3 ; x++)
      if(ops[x].size() == 1 && std::find(letters.begin(), letters.end(), ops[x]) == letters.end())
        letters.push_back(ops[x]);
   
    add_variables(ops); 
    consistency_map.push_back(new CalculationConsistency(ops));

    ops[3] = ops[4];
    ops[4] = "C" + std::to_string(temp++);
  };

  variables.at(ops[3]) == Var(0);

  variables.at(std::string(1,s1.back())) != Var(0);
  variables.at(std::string(1,s2.back())) != Var(0);
  variables.at(std::string(1,s3.back())) != Var(0);

  consistency_map.push_back(new NotEqualConsistency(letters));
}

bool Backtrack::solve(std::vector<std::string> &vnames, std::unordered_map<std::string, Var> solvemap) {
  if(vnames.empty()) {
    for(Consistency *c : consistency_map)
      if(!c->consistent(solvemap)) return false;
    std::cout << "--- Solution found ---" << std::endl <<std::flush;
    print_variables(solvemap);
    return true;
  };
  std::string vname = vnames.front();
  vnames.erase(vnames.begin());
  solvemap.emplace(vname, Var(0));
  std::vector<int> domain = variables.at(vname).domain();
  for(int i  : domain) {
    solvemap.at(vname) = Var(i);
    solve(vnames, solvemap);
  };
  vnames.push_back(vname);
  return true;
}

bool Backtrack::solve() {
  std::vector<std::string> vnames;
  std::unordered_map<std::string, Var> solvemap;
  solvemap.emplace("ZERO", Var(0));
  for(auto it = variables.begin() ; it != variables.end() ; ++it)
    if(it->first != "ZERO")
      vnames.push_back(it->first);
  variables.at("m") = Var(1);
  variables.at("o") = Var(0);
  variables.at("s") = Var(9);
  return solve(vnames, solvemap);
}

void Backtrack::print_variables(const std::unordered_map<std::string, Var> &vars) const {
  for(auto it = vars.begin() ; it != vars.end() ; ++it) {
    std::cout << it->second << std::endl << std::flush;
  };
}
