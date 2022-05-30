#include <string>
#include <vector>

namespace pimpl
{

using Symbol = std::string;

struct Literal
{
  Symbol symbol;
  bool positive;
};

class Clause
{
public:
  Clause() = default;
  Clause(std::vector<Literal> l) : literals_(l) {}

  std::vector<Literal> get() const;
  void set(std::vector<Literal> literals);
  bool isUnit() const;
  bool isHorn() const;
  bool isDefinite() const;
  bool isGoal() const;
private:
  int countPositive() const;
  std::vector<Literal> literals_;
};

class Sentence
{
public:
  Sentence() = default;
  Sentence(std::vector<Clause> c) : clauses_(c) {}
  std::vector<Clause> get() const;
  void set(std::vector<Clause> clauses);
  bool validHorn() const;
private:
  std::vector<Clause> clauses_;
};

class KB
{
  KB() = default;
  KB(std::vector<Sentence> s) : sentences_(s) {}
  void addSentence(Sentence sentence);
private:
  std::vector<Sentence> sentences_;
};

} // namespace pimpl

