#ifndef __CONSTRAINT_H__
#define __CONSTRAINT_H__

#include <algorithm>
#include <iostream>
#include <string>
#include <unordered_map>

#include "var.h"

namespace std {
  template<> inline bool operator==(const pair<string, string> &p1, const pair<string, string> &p2) { return p1.first == p2.first && p1.second == p2.second ; }
  template<> struct hash<pair<string, string>> {
    inline size_t operator()(const pair<string, string> &key) const {return hash<string>()(key.first + "." + key.second); }
  };
};

class GeneralConstraint {
public:
  virtual bool changed(std::unordered_map<std::string, Var> &variables) = 0;
};

class Equal : public GeneralConstraint {
public:
  Equal(std::string k1, std::string k2) { this->k1 = k1 ; this->k2 = k2; }
  virtual bool changed(std::unordered_map<std::string, Var> &variables) {
    bool c = (variables.at(k1) == variables.at(k2));
    //std::cout << "Equal[" << k1 << ',' << k2 << "]=" << c << ' ' << variables.at(k1) << ',' << variables.at(k2) << std::endl << std::flush;
    return c;
  };
protected:
  std::string k1;
  std::string k2;
};

class NotEqual : public GeneralConstraint {
public:
  NotEqual(std::string k1, std::string k2) { this->k1 = k1 ; this->k2 = k2; }
  virtual bool changed(std::unordered_map<std::string, Var> &variables) {
    bool c = variables.at(k1) != variables.at(k2);
    //std::cout << "NotEqual[" << k1 << ',' << k2 << "]=" << c << ' ' << variables.at(k1) << ',' << variables.at(k2) << std::endl << std::flush;
    return c;
  };
protected:
  std::string k1;
  std::string k2;
};

class Calculation : public GeneralConstraint {
public:
  Calculation(const std::string change, const std::array<std::string, 5> &ops) { this->change = change ; this->ops = ops; }
  virtual bool changed(std::unordered_map<std::string, Var> &variables) {
    int changewhich = std::distance(ops.begin(), std::find(ops.begin(), ops.end(), change));
    bool changed;
    std::string o1   = ops[0];
    std::string o2   = ops[1];
    std::string ans  = ops[2];
    std::string cin  = ops[3];
    std::string cout = ops[4];
    switch(changewhich) {
    case 0:
      changed = variables.at(o1) == variables.at(ans) + Var(10) * variables.at(cout) - variables.at(o2) - variables.at(cin);
      break;
    case 1:
      changed = variables.at(o2) == variables.at(ans) + Var(10) * variables.at(cout) - variables.at(o1) - variables.at(cin);
      break;
    case 2:
      changed = variables.at(ans) == variables.at(o1) + variables.at(o2) + variables.at(cin) - Var(10) * variables.at(cout);
      break;
    case 3:
      changed = variables.at(cin) == variables.at(ans) + Var(10) * variables.at(cout) - variables.at(o1) - variables.at(o2);
      break;
    case 4:
      changed = variables.at(cout) == (variables.at(o1) + variables.at(o2) + variables.at(cin) - variables.at(ans))/Var(10);
      break;
    };
    //std::cout << "Calculation[" << change << "]=" << changed;
    //for(std::string s : ops) std::cout << ',' << variables.at(s);
    //std::cout << std::endl << std::flush;
    return changed;
  };
protected:
  std::string change;
  std::array<std::string, 5> ops;
};

#endif // __CONSTRAINT_H__
