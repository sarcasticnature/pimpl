#include "pimpl/types.hpp"

namespace pimpl
{

std::vector<Literal> Clause::get() const
{
  return literals_;
}

void Clause::set(std::vector<Literal> literals)
{
  literals_ = literals;
}

bool Clause::isUnit() const
{
  return literals_.size() == 1;
}

bool Clause::isHorn() const
{
  return countPositive() <= 1;
}

bool Clause::isDefinite() const
{
  return countPositive() == 1;
}

bool Clause::isGoal() const
{
  return countPositive() == 0;
}

inline int Clause::countPositive() const
{
  int res = 0;
  for (const auto& lit : literals_) {
    res += lit.positive ? 1 : 0;
  }
  return res;
}

std::vector<Clause> Sentence::get() const
{
  return clauses_;
}

void Sentence::set(std::vector<Clause> clauses)
{
  clauses_ = clauses;
}

bool Sentence::validHorn() const
{
  for (const auto& clause : clauses_) {
    if (!clause.isHorn()) {
      return false;
    }
  }
  return true;
}

void KB::addSentence(Sentence sentence)
{
  sentences_.push_back(sentence);
}

} // namespace pimpl

