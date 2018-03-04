#include "backtrack.h"

#include <algorithm>

void Backtrack::add_variables(std::array<std::string, 5> ops) {
  for(std::string v : ops) {
    if(_variables.find(v) == _variables.end())
      _variables.emplace(v, Var(v, {0,1,2,3,4,5,6,7,8,9}));
  };
}

Backtrack::Backtrack(std::string s1, std::string s2, std::string s3) {
  std::vector<std::array<std::string, 5>> opslist;
  int temp = 0;
  _variables.emplace("ZERO", Var(0));
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
    //consistency_map.push_back(new CalculationConsistency(ops));
    opslist.push_back(ops);

    ops[3] = ops[4];
    ops[4] = "C" + std::to_string(temp++);
  };

  _variables.at(ops[3]) == Var(0);

  _variables.at(std::string(1,s1.back())) != Var(0);
  _variables.at(std::string(1,s2.back())) != Var(0);
  _variables.at(std::string(1,s3.back())) != Var(0);

  //consistency_map.push_back(new NotEqualConsistency(letters));
  ac3 = new AC3(_variables, opslist);
}

std::string Backtrack::select_unassigned_variable(const AC3 &ac3, std::vector<std::string> &vnames) {
  int count = std::numeric_limits<int>::max();
  int which;

  for(int i = 0 ; i < vnames.size() ; i++) {
    if(/*ac3.variable(vnames[i]).domain().size() > 1 && */ count > ac3.variable(vnames[i]).domain().size()) {
      which = i;
      count = ac3.variable(vnames[i]).domain().size();
    };
  };
  std::string vname = vnames[which];
  vnames.erase(vnames.begin()+which);
  return vname;
}

bool Backtrack::solve(const AC3 &curac3, std::vector<std::string> &vnames) {
  if(vnames.empty()) {
//    for(Consistency *c : consistency_map)
//      if(!c->consistent(solvemap)) return false;
    std::cout << "--- Solution found ---" << std::endl <<std::flush;
    for(std::string var : letters)
      std::cout << var << ": " << curac3.variable(var) << std::endl;
    //curac3.print_variables();
    return true;
  };
  std::string vname = select_unassigned_variable(curac3, vnames);
  std::vector<int> domain = curac3.variable(vname).domain();
  //std::cout << "Backtrack trying " << vname << ", " << curac3.variable(vname) << std::endl << std::flush;
  for(int i  : domain) {
    AC3 nextac3(curac3);
    nextac3.set_variable(vname, Var(i));
    //std::cout << "Backtrack trying " << vname << " value of " << i << std::endl << std::flush;
    if(nextac3.run()) //{
      //std::cout << "Backtrack " << vname << " worked! Nesting!" << std::endl << std::flush;
      solve(nextac3, vnames);
    //} else
    //  std::cout << "Backtrack " << vname << " failed, trying next value!" << std::endl << std::flush;
  };
  vnames.push_back(vname);
  //std::cout << "Backtrack " << vname << " failed, un-nesting!" << std::endl << std::flush;
  return false;
}

bool Backtrack::solve() {
  ac3->run(); // Do the initial set of consistency checks
              // It may solve it, disprove it, or redue the
              // domains of the variables0

  std::vector<std::string> vnames;
  std::unordered_map<std::string, Var> solvemap;
  solvemap.emplace("ZERO", Var(0));
  for(auto it = _variables.begin() ; it != _variables.end() ; ++it)
    if(it->first != "ZERO")
      vnames.push_back(it->first);
  return solve(*ac3, vnames);
}

void Backtrack::print_variables(const std::unordered_map<std::string, Var> &vars) const {
  for(auto it = vars.begin() ; it != vars.end() ; ++it) {
    std::cout << it->first << ": " << it->second << std::endl << std::flush;
  };
}
