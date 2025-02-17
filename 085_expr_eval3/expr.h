#include <string.h>

#include <cstdlib>
#include <iostream>
#include <sstream>
class Expression {
 public:
  virtual std::string toString() const = 0;
  virtual ~Expression() {}
  virtual long evaluate() const = 0;
};

class OpExpression : public Expression {
 private:
  std::stringstream ss;
  long answer;

 public:
  OpExpression(char op, Expression * lhs, Expression * rhs) {
    ss << '(' << lhs->toString() << ' ' << op << ' ' << rhs->toString() << ')';
    switch (op) {
      case '+': {
        answer = lhs->evaluate() + rhs->evaluate();
        break;
      }
      case '-': {
        answer = lhs->evaluate() - rhs->evaluate();
        break;
      }
      case '*': {
        answer = lhs->evaluate() * rhs->evaluate();
        break;
      }
      case '/': {
        answer = lhs->evaluate() / rhs->evaluate();
        break;
      }
      default:
        break;
    }
  }
  virtual std::string toString() const {
    std::string answer(ss.str());
    return answer;
  }
  virtual long evaluate() const { return answer; }
  virtual ~OpExpression() {}
};

class NumExpression : public Expression {
 private:
  long number;

 public:
  NumExpression(long num) : number(num) {}
  virtual std::string toString() const {
    std::stringstream num;
    num << number;
    std::string numString(num.str());
    return numString;
  }
  long evaluate() const { return number; }
  virtual ~NumExpression() {}
};

class PlusExpression : public OpExpression {
 public:
  PlusExpression(Expression * lhs, Expression * rhs) : OpExpression('+', lhs, rhs) {
    delete lhs;
    delete rhs;
  }
};

class MinusExpression : public OpExpression {
 public:
  MinusExpression(Expression * lhs, Expression * rhs) : OpExpression('-', lhs, rhs) {
    delete lhs;
    delete rhs;
  }
};

class TimesExpression : public OpExpression {
 public:
  TimesExpression(Expression * lhs, Expression * rhs) : OpExpression('*', lhs, rhs) {
    delete lhs;
    delete rhs;
  }
};

class DivExpression : public OpExpression {
 public:
  DivExpression(Expression * lhs, Expression * rhs) : OpExpression('/', lhs, rhs) {
    delete lhs;
    delete rhs;
  }
};
