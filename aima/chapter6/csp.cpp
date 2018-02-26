#include <string>
#include <unordered_map>
#include <vector>

enum CTYPE {
  NE, EQUAL
};

struct Constraint {
  Constraint(CTYPE type, std::string op1, std::string op2) { this->type = type; this->op1 = op1 ; this->op2 = op2; }
  Constraint(CTYPE type, std::string op1, std::string op2, std::string op3, std::string op4)
     { this->type = type; this->op1 = op1 ; this->op2 = op2; this->op3 = op3 ; this->op4 = op4; }
  CTYPE type;
  std::string op1;
  std::string op2;
  std::string op3;
  std::string op4;

  bool ne() {
    int o1    = (op1.empty() ? 0 : 9);
    int o2    = (op2.empty() ? 0 : 9);
    return o1 != o2;
  };

  bool equal() {
    int o1    = (op1.empty() ? 0 : 9);
    int o2    = (op2.empty() ? 0 : 9);
    int ans   = (op3.empty() ? 0 : 9);
    int carry = (op4.empty() ? 0 : 9);
    return (o1 + o2) == (ans + 10 * carry);
  };
};

//auto NE = [](int b1, int b2) -> bool {return b1 != b2;};

std::unordered_map<std::string, std::vector<int>> domains;
std::vector<Constraint> csp;

void cadd(std::vector<char> &letters, char c) {
  if(std::find(letters.begin(), letters.end(), c) == letters.end()) return;
  letters.push_back(c);
}

void setup(std::string op1, std::string op2, std::string answer) {
  std::vector<char> letters;

  for(char c : op1) cadd(letters, c);
  for(char c : op2) cadd(letters, c);
  for(char c : answer) cadd(letters, c);

  for(int x = 0 ; x < letters.size() ; x++)
    for(int y = x + 1 ; y < letters.size() ; y++)
      csp.push_back(Constraint(NE, std::string(1, letters[x]), std::string(1, letters[y])));

  std::string tempvar = "";  
  int tempid = 0;
  int max = std::max({op1.size(), op2.size(), answer.size()});
  for(int x = 0 ; x < max ; x++) {
    if(x < op1.size() && x < op2.size() && x < answer.size()) { // o1 + o2 + (10*t) = o3
      csp.push_back(Constraint(EQUAL, std::string(1, op1[x]), std::string(1, op2[x]), std::string(1, answer[x]), tempvar));
    } else if(x < op1.size() && x < answer.size()) {            // o1      + (10*t) = o3
      csp.push_back(Constraint(EQUAL, std::string(1, op1[x]), "", std::string(1, answer[x]), tempvar));
    } else if(x < op2.size() && x < answer.size()) {            //      o2 + (10*t) = o3
      csp.push_back(Constraint(EQUAL, std::string(1, op2[x]), "", std::string(1, answer[x]), tempvar));
    } else if(x < answer.size()) {                              //           (10*t) = o3
      csp.push_back(Constraint(EQUAL, "", "", std::string(1, answer[x]), tempvar));
    } else throw "Invalid";
    tempvar = "T" + std::to_string(tempid++);
  };
}

int main() {
  setup("two", "two", "four");
}
