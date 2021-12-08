#include "calc.h"
#include <map>
#include <vector>
#include <sstream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <iostream>

struct Calc {
private:
  // fields
  std::map<std::string, int> variableMap;
  pthread_mutex_t lock;
  
public:
  // public member functions
  Calc() {
    pthread_mutex_init(&this->lock, NULL);
    //variableMap = new std::map<std::string, int>;
    //isOperator('+');
  }
  ~Calc(){
    variableMap.clear();
    pthread_mutex_destroy(&this->lock);
  }
  
  int evalExpr(const char *expr, int *result) {
    std::vector<std::string> tokens = tokenize(expr);
    size_t tokenSize = tokens.size();
    int res;
    pthread_mutex_lock(&this->lock);
    switch(tokenSize) {
    case 1:
      pthread_mutex_unlock(&this->lock);
      return eval1(tokens, result);
    case 3:
      
      if(isOperator(tokens[1]) == 3 && tokens[2] == "0") {
	//error
	pthread_mutex_unlock(&this->lock);
	return 0;
      }
      res = eval3(tokens, result);
      pthread_mutex_unlock(&this->lock);
      return eval3(tokens, result);
    case 5:
      
      if(isOperator(tokens[3]) == 3 && tokens[4] == "0") {
	//error
	//std::cout <<"DIVIDE BY ZERO \n" << std::endl;
	pthread_mutex_unlock(&this->lock);
	return 0;
      }
      res = eval5(tokens, result);
      pthread_mutex_unlock(&this->lock);
      return res;
    default:
      //pthread_mutex_unlock(&this->lock);
      return 0;
    }
  }
  
private:
  
  std::vector<std::string> tokenize(const std::string &expr);
  int eval1(std::vector<std::string> tokens, int* result);
  int eval3(std::vector<std::string> tokens, int* result);
  int eval5(std::vector<std::string> tokens, int* result);
  int operation(int op, std::string left, std::string right);
  int isOperator (std::string expr);
  bool isValidOperand(std::string token);
  bool isValidVariable(std::string token);
  bool isValidNumber(std::string token);
  bool hasVariable(std::string var);
  void updateVariable(std::string var, std::string update);
  
  //if token size is one
  
};

int Calc::eval1(std::vector<std::string> tokens, int* result) {
  //checks if is a valid number
  //if it is, return that number
  if(isValidNumber(tokens[0])){
    *result = std::stoi(tokens[0]);
    return 1;
  }
  //check if is a valid variable
  if(isValidVariable(tokens[0])) {
    //check if map has our variable
    if(hasVariable(tokens[0])) {
      *result = variableMap.find(tokens[0])->second;
      return 1;
    }
    else{
      return 0;
    }
  }
  return 0;
}

int Calc::eval3(std::vector<std::string> tokens, int* result) {
  if(isOperator(tokens[1]) != 5) {
    if(isValidVariable(tokens[0]) && !hasVariable(tokens[0])) {
      return 0;
    }
    
    
    //std::cout << "WORKING: " << isOperator(tokens[1]) << tokens[0] << tokens[2] << std::endl;
    *result = operation(isOperator(tokens[1]), tokens[0], tokens[2]);
    return 1;
  }
  else if(isValidNumber(tokens[0])) {
    if(isOperator((tokens[1])) == 5) {
      return 0;
    }
  }
  else if(isValidVariable(tokens[0]) && (isOperator(tokens[1]) == 5)) {
    //std::cout << tokens.size() << std::endl;
    //std::cout << tokens[2] << std::endl;
    updateVariable(tokens[0], tokens[2]);
    if(isValidVariable(tokens[2])) {
      *result = variableMap[tokens[2]];
    }
    else {
      *result = std::stoi(tokens[2]);
    }
    return 1;
  }
  return 0;
}


int Calc::eval5(std::vector<std::string> tokens, int* result) {
  if(!isValidVariable(tokens[0])){
    
    return 0;
  }
  if(tokens[1][0] != '=') {
    
    return 0;
  }
  
  //check if operands are valid
  if(!(isValidOperand(tokens[2]) && isValidOperand(tokens[4]))) {
    
    return 0;
  }
  
  int oper = isOperator(tokens[3]);
  if(oper == -1) {
    
    return 0;
  }
  
  int value = operation(oper, tokens[2], tokens[4]);
  //std::cout << "DOING: " << oper << tokens[2] << tokens[4] << std::endl;
  //std::cout << "Result of operator: " << value << std::endl;
  variableMap[tokens[0]] = value;
  *result = value;
  return 1;
  
}

int Calc::operation(int op, std::string left, std::string right) {
  int ileft;
  int iright;
  //might need to check if the variable is in the map first
  if(isValidVariable(left)) {
    ileft = variableMap[left];
  }
  else if(isValidNumber(left)) {
    ileft = std::stoi(left);
  }
  else {
    //error
  }
  if(isValidVariable(right)){
    iright = variableMap[right];
  }
  else if(isValidNumber(right)) {
    iright = std::stoi(right);
  }
  else {
    //error
  }
  switch(op){
  case 1:
    return ileft + iright;
  case 2:
    return ileft - iright;
  case 3:
    if(iright == 0) {
      //error
    }
    return ileft / iright;
  case 4:
    return ileft * iright;
  default:
    return -1;
  }
}
// private member functions
int Calc::isOperator (std::string expr) {
  if(expr.size() != 1) {
    return -1;
  }
  char current = expr[0];
  switch(current) {
  case '+': 
    return 1; 
  case '-':
    return 2; 
  case '/':
    return 3;
  case '*':
    return 4; 
  case '=':
    return 5; 
  default: 
    return -1;
  }
}

bool Calc::isValidOperand(std::string token) {
  if (isValidNumber(token)) {
    return true; 
  }
  if (isValidVariable(token)) {
    if (hasVariable(token)){
      return true;
    }
  }
  return false;
}

bool Calc::isValidVariable(std::string token) {
  int i = 0;
  while(token[i] != '\0') {
    if(!(token[i] >= 'a' && token[i] <= 'z') && !(token[i] >= 'A' && token[i] <= 'Z')) {
      return false;
    }
    i++;
  }
  return true;
}

bool Calc::isValidNumber(std::string token) {
  int i = 0;
  if (token[0] == '-') {
    i++;
  }
  while(token[i] != '\0') {
    if(token[i] < '0' || token[i] > '9') {
      return false;
    }
    i++;
  }
  return true;
}

std::vector<std::string> Calc::tokenize(const std::string &expr) {
  std::vector<std::string> vec;
  std::stringstream s(expr);
  
  std::string tok;
  while (s >> tok) {
    vec.push_back(tok);
  }
  
    return vec;
}

bool Calc::hasVariable(std::string var) {
  int check = variableMap.count(var);
  if (check == 0) {
    return false;
      }
  return true;
}

void Calc::updateVariable(std::string var, std::string update) {
  int num;
  if(isValidVariable(update) && isValidOperand(update)) {
    num = variableMap[update];
  }
  if(isValidNumber(update)) {
    num = std::stoi(update);
  }
  variableMap[var] = num;
}

extern "C" struct Calc *calc_create(void) {
  return new Calc();
}

extern "C" void calc_destroy(struct Calc *calc) {
  delete calc;
}

extern "C" int calc_eval(struct Calc *calc, const char *expr, int *result) {
  return calc->evalExpr(expr, result);
}





