#ifndef __BACKTRACK_H__
#define __BACKTRACK_H__

#include "var.h"

#include <array>
#include <string>
#include <unordered_map>

class Consistency {
public:
  virtual bool consistent(std::unordered_map<std::string, Var> &variables) = 0;
};

class CalculationConsistency : public Consistency {
public:
  CalculationConsistency(std::array<std::string, 5> &ops) { this->ops = ops; }
  bool consistent(std::unordered_map<std::string, Var> &variables) {
    Var left = variables.at(ops[0]) + variables.at(ops[1]) + variables.at(ops[3]);
    Var right = variables.at(ops[2]) + Var(10) * variables.at(ops[4]);
    left == right; // Changes the domain of 'left' based on the domain of 'right'
    //std::cout << "Checking calculation consistency: " << ops[0] << '+' << ops[1] << '+' << ops[3] << '=' << ops[2] << "+10*" << ops[3]
    //          << (left.domain_empty() ? " is not " : " is ") << "consistent" << std::endl << std::flush;
    return !left.domain_empty();
  }
protected:
  std::array<std::string, 5> ops;
};

class NotEqualConsistency : public Consistency {
public:
   NotEqualConsistency(std::vector<std::string> &unique) { this->unique = unique; }
  bool consistent(std::unordered_map<std::string, Var> &variables) {
    //std::cout << "Checking unique consistency: ";
    for(int x = 0 ; x < unique.size() ; x++)
      for(int y = 0 ; y < unique.size() ; y++)
        if(x != y) {
          if(!variables.at(unique[x]).isne(variables.at(unique[y]))) {
            //std::cout << unique[x] << " and " << unique[y] << " are not unique, inconsistent" << std::endl << std::flush;
            return false;
          }
        };
    //std::cout << " all variables are consistent" << std::endl << std::flush;
    return true;
  }
protected:
  std::vector<std::string> unique;
};

class Backtrack {
public:
  Backtrack(std::string s1, std::string s2, std::string s3);
  bool solve();
  void print_variables(const std::unordered_map<std::string, Var> &variables) const;
  void print_variables() { print_variables(variables); }

protected:
  bool solve(std::vector<std::string> &vnames, std::unordered_map<std::string, Var> solvemap);
  std::vector<Consistency *> consistency_map;
  std::unordered_map<std::string, Var> variables;

  void add_variables(std::array<std::string, 5> ops);
};

#endif // __BACKTRACK_H__
