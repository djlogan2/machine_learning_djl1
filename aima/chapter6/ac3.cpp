#include "ac3.h"

#include <algorithm>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>


std::string AC3::add_new_equal_constraint(std::string op) {
  if(op == "ZERO") return "ZERO";
  std::string tvar1 = "T" + std::to_string(temp++);
  std::string tvar2 = "T" + std::to_string(temp++);
  std::cout << "Adding equal constraint [" << op << ',' << tvar1 << ']' << std::endl;
  std::cout << "Adding equal constraint [" << tvar1 << ',' << tvar2 << ']' << std::endl;
  std::cout << "Adding equal constraint [" << tvar1 << ',' << op << ']' << std::endl;
  std::cout << "Adding equal constraint [" << tvar2 << ',' << tvar1 << ']' << std::endl;
  variables.emplace(tvar1, Var(tvar1, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
  variables.emplace(tvar2, Var(tvar2, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
  constraints.emplace(make_pair(op, tvar1), new Equal(op, tvar1));
  constraints.emplace(make_pair(tvar1, op), new Equal(tvar1, op));
  constraints.emplace(make_pair(tvar1, tvar2), new Equal(tvar1, tvar2));
  constraints.emplace(make_pair(tvar2, tvar1), new Equal(tvar2, tvar1));
  return tvar2;
}

void AC3::add_not_equal_constraint(std::string k1, std::string k2) {
  if(constraints.find(make_pair(k1, k2)) != constraints.end())
    k2 = add_new_equal_constraint(k2);
  std::cout << "Adding not equal constraint [" << k1 << ',' << k2 << ']' << std::endl;
  constraints.emplace(make_pair(k1, k2), new NotEqual(k1, k2));
}

void AC3::add_both_not_equal_constraints(std::string k1, std::string k2) {
  add_not_equal_constraint(k1, k2);
  add_not_equal_constraint(k2, k1);
}

void AC3::add_calculation_constraint(std::string k1, std::string k2, std::array<std::string, 5> ops) {
  std::pair<std::string, std::string> key(k1, k2);
  if(k1 == "ZERO" || k2 == "ZERO") return;
  if(k1 == k2) return;
  if(constraints.find(key) != constraints.end()) {
    std::string newk2 = add_new_equal_constraint(k2);
    replace_if(ops.begin(), ops.end(), [k2](const std::string &s){return s == k2;}, newk2);
    k2 = key.second = newk2;
  };
  constraints.emplace(key, new Calculation(k1, ops));
  std::cout << "Adding calculation constraint [" << k1 << ',' << k2 << "]=";
  std::cout << ops[0] << '+' << ops[1] << '+' << ops[3] << '=' << ops[2] << "+10*" << ops[4] << std::endl;
}

void AC3::add_both_calculation_constraints(std::string k1, std::string k2, std::array<std::string, 5> ops) {
  add_calculation_constraint(k1, k2, ops);
  add_calculation_constraint(k2, k1, ops);
}

std::string AC3::new_temporary(char c) {
  std::string tvar = std::string(1, c) + std::to_string(temp++);
  variables.emplace(tvar, Var(tvar, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}));
  return tvar;
}

void AC3::uniqify_ops(std::array<std::string, 5> &ops) {
  for(int x = 0 ; x < 5 ; x++)
    for(int y = x + 1 ; y < 5 ; y++)
      if(ops[x] == ops[y])
        ops[y] = add_new_equal_constraint(ops[y]);
}

AC3::AC3(std::string s1, std::string s2, std::string s3) {
  variables.emplace("ZERO", Var(0));
  std::vector<std::string> letters;
  std::reverse(s1.begin(), s1.end());
  std::reverse(s2.begin(), s2.end());
  std::reverse(s3.begin(), s3.end());
  int chars = std::max(std::max(s1.size(), s2.size()), s3.size());
  std::array<std::string, 5> ops;
  ops[3] = "ZERO";
  ops[4] = new_temporary('C'); // First carry out is our first temporary.
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
    //
    // For each of the three letters, we store them in the letters vector if we don't already
    //   have them.
    //
    for(int x = 0 ; x < 3 ; x++)
      if(ops[x].size() == 1 && std::find(letters.begin(), letters.end(), ops[x]) == letters.end())
        letters.push_back(ops[x]);
    //
    // Now add calculation constraints for every combination of variables both ways
    //
    uniqify_ops(ops);
    for(int y = 0 ; y < 5 ; y++)
      for(int z = y+1 ; z < 5 ; z++)
        if(y != z) add_both_calculation_constraints(ops[y], ops[z], ops);
    ops[3] = ops[4];
    ops[4] = new_temporary('C');
  };

  variables.at(ops[3]) == Var(0);

  //
  // Lastly, we now have the vector of unique letters. Add them all in as unique
  //
  for(int x = 0 ; x < letters.size() ; x++) {
    variables.emplace(letters[x], Var(letters[x], {0,1,2,3,4,5,6,7,8,9}));
    for(int y = x + 1 ; y < letters.size() ; y++) {
      add_both_not_equal_constraints(letters[x], letters[y]);
    };
  };

  variables.at(std::string(1,s1.back())) != Var(0);
  variables.at(std::string(1,s2.back())) != Var(0);
  variables.at(std::string(1,s3.back())) != Var(0);
}

bool AC3::run() {
  std::vector<std::pair<std::string, std::string>> queue;

  for(auto it = constraints.begin() ; it != constraints.end() ; ++it)
    queue.push_back(it->first);

  while(!queue.empty()) {
    //
    // Get the first constraint on the queue
    //
    std::pair<std::string, std::string> key = queue.front();
    queue.erase(queue.begin());
    std::cout << "Retrieved [" << key.first << ',' << key.second << "] from the queue, size=" << queue.size() << std::endl << std::flush;

    //
    // If the constraint changes the domain of the variable
    //    If the new domain is empty, no solution
    //    If it's not empty, then re-add all constraints
    //     for which a variable is dependent upon the variable
    //     that changed, excluding the dependency of the
    //     original two (i.e., if a depends on b, and a changes
    //     due to the constraint, add all constraints that depend
    //     upon a except for b -> a, as we already know that
    //     constraint will not change.)
    //
    if(constraints.at(key)->changed(variables)) {
      if(variables.at(key.first).domain_empty()) {
        return false;
      };
      for(auto it = constraints.begin() ; it != constraints.end() ; ++it) {
        //std::cout << "WTF? : [" << (it->first).first << ',' << (it->first).second << "],[" << key.first << ',' << key.second << ']' << std::endl << std::flush;
        if((it->first).second == key.first && (it->first).first != key.second) {
          std::cout << "Adding constraint [" << (it->first).first << ',' << (it->first).second << "] back into the queue, size=" << queue.size() << std::endl << std::flush;
          queue.push_back(it->first);
        };
      };
    };
  };
  return true;
}

void AC3::print_variables() {
  for(auto it = variables.begin() ; it != variables.end() ; ++it) {
    std::cout << it->second << std::endl << std::flush;
  };
}
