#ifndef __VAR_H__
#define __VAR_H__

#include <iostream>
#include <string>
#include <vector>

class Var {
public:
//  Var() {name="default constructor";}
  Var(const std::string &name, const std::vector<int> &domain);
  Var(int constant);
  Var operator+(const Var &other);
  Var operator-(const Var &other);
  Var operator*(const Var &other);
  Var operator/(const Var &other);
  bool operator==(const Var &other);
  bool operator!=(const Var &other);
  Var &operator=(const Var &other);
  bool isne(const Var &other);
  friend std::ostream &operator<<(std::ostream &os, const Var &var);
  bool domain_empty() { return _domain.empty(); }
  const std::vector<int> &domain() const { return _domain; }
protected:
  std::string name;
  std::vector<int> _domain;
};

#endif // __VAR_H__
