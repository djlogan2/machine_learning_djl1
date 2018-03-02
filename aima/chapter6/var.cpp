#include "var.h"

#include <algorithm>

Var::Var(const std::string &name, const std::vector<int> &_domain) {
  this->name = name;
  this->_domain = _domain;
}

Var::Var(int constant) {
  name = std::to_string(constant);
  _domain.push_back(constant);
}

bool Var::isne(const Var &other) {
  if(other._domain.size() > 1 || _domain.size() > 1) return true;
  return _domain[0] != other._domain[0];
}

bool Var::operator!=(const Var &other) {
 // TODO: other only has one element. Just remove it from our _domain
 // Also: Look into algorithm options instead of manual operations
  //std::cout << "   vcalc: " << *this << " != " << other <<  " = ";
  if(other._domain.size() != 1) {
    //std::cout << *this << std::endl << std::flush;
    return false;
  };
  auto it = std::find(_domain.begin(), _domain.end(), other._domain[0]);
  if(it == _domain.end()) {
    //std::cout << *this << std::endl << std::flush;
    return false;
  };
  _domain.erase(it);
  //std::cout << *this << std::endl << std::flush;
  return true;
}

Var Var::operator+(const Var &other) {
  std::vector<int> results;
  for(int ours : _domain)
    for(int theirs : other._domain)
      if(std::find(results.begin(), results.end(), ours + theirs) == results.end()) {
        results.push_back(ours + theirs);
      };
  //std::cout << "   vcalc: " << *this << " + " << other << " = " << Var(name + "." + other.name, results) << std::endl << std::flush;
  return Var(name + "." + other.name, results);
}

Var Var::operator-(const Var &other) {
  std::vector<int> results;
  for(int ours : _domain)
    for(int theirs : other._domain)
      if(std::find(results.begin(), results.end(), ours - theirs) == results.end()) {
        results.push_back(ours - theirs);
      };
  //std::cout << "   vcalc: " << *this << " - " << other << " = " << Var(name + "." + other.name, results) << std::endl << std::flush;
  return Var(name + "." + other.name, results);
}

Var Var::operator*(const Var &other) {
  std::vector<int> results;
  for(int ours : _domain)
    for(int theirs : other._domain)
      if(std::find(results.begin(), results.end(), ours * theirs) == results.end()) {
        results.push_back(ours * theirs);
      };
  //std::cout << "   vcalc: " << *this << " * " << other << " = " << Var(name + "." + other.name, results) << std::endl << std::flush;
  return Var(name + "." + other.name, results);
}

Var Var::operator/(const Var &other) {
  std::vector<int> results;
  for(int ours : _domain)
    for(int theirs : other._domain)
      if(!(ours % theirs)) {
        if(theirs && std::find(results.begin(), results.end(), ours/theirs) == results.end()) {
          results.push_back(ours / theirs);
        };
      };
  //std::cout << "   vcalc: " << *this << " / " << other << " = " << Var(name + "." + other.name, results) << std::endl << std::flush;
  return Var(name + "." + other.name, results);
}

bool Var::operator==(const Var &other) {
  std::vector<int> new_domain;
  for(int ours : _domain)
    for(int theirs : other._domain)
      if(ours == theirs) {
        if(std::find(new_domain.begin(), new_domain.end(), ours) == new_domain.end())
          new_domain.push_back(ours);
      };
  //std::cout << "   vcalc: " << *this << " == " << other << " = ";
  bool changed = new_domain.size() != _domain.size();
  _domain = new_domain;
  //std::cout << *this << std::endl << std::flush;
  return changed;
}

Var &Var::operator=(const Var &other) {
  name = other.name;
  _domain = other._domain;
  //std::cout << "  Var operator=" << other << std::endl << std::flush;
  return *this;
}

std::ostream &operator<<(std::ostream &os, const Var &var) {
  std::string comma = "";
  os << "Var(" << var.name << ",[";
  for(int i : var._domain) {
    os << comma << i;
    comma = ",";
  };
  os << "])";
  return os;
}
