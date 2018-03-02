#ifndef __AC3_H__
#define __AC3_H__

#include "constraint.h"
#include "var.h"

#include <array>
#include <string>
#include <unordered_map>


class AC3 {
public:
  AC3(std::string s1, std::string s2, std::string s3);
  bool run();
  void print_variables();

protected:
  std::unordered_map<std::string, Var> variables;
  int temp = 0;
  std::unordered_map<std::pair<std::string, std::string>, GeneralConstraint*> constraints;

  std::string add_new_equal_constraint(std::string op);
  void add_not_equal_constraint(std::string k1, std::string k2);
  void add_both_not_equal_constraints(std::string k1, std::string k2);
  void add_calculation_constraint(std::string k1, std::string k2, std::array<std::string, 5> ops);
  void add_both_calculation_constraints(std::string k1, std::string k2, std::array<std::string, 5> ops);
  std::string new_temporary(char c);
  void uniqify_ops(std::array<std::string, 5> &ops);
};

#endif // __AC3_H__
