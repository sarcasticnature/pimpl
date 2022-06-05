#ifndef __PIMPL__TYPES_HPP__
#define __PIMPL__TYPES_HPP__

#include <string>
#include <vector>

namespace pimpl
{

/// Type alias to abstract a symbol's type, largely in case of later changes
using Symbol = std::string;

/// Literal := Symbol | ~Symbol
struct Literal
{
  Symbol symbol;
  bool positive;
};

/// Clause := Literal | Literal_1 v ... v Literal_n
/**
 * A Clause of propositional logic (in conjunctive normal form).
 * The clause may or may not be a horn (etc.) clause, so convenience functions
 * are provided to check its form.
 */
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

/// Sentence := Clause | Clause_1 ^ ... ^ Clause_n
/**
 * A Sentence of propositional logic in conjunctive normal form.
 * The sentence may or may not contain purely horn clauses.
 */
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

/// A Knowledge Base containing propositional logic sentences
/**
 * A Knowledge Base of propositional logic, currently with no implementations
 * of inference or proving (to be added in the future)
 */
class KB
{
  KB() = default;
  KB(std::vector<Sentence> s) : sentences_(s) {}
  void addSentence(Sentence sentence);
private:
  std::vector<Sentence> sentences_;
};

} // namespace pimpl

#endif // #ifndef __PIMPL__TYPES_HPP__

