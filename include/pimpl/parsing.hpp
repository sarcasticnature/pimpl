#ifndef __PIMPL__PARSING_HPP__
#define __PIMPL__PARSING_HPP__

#include <vector>
#include <string>

#include "pimpl/types.hpp"


/// A collection of functions for parsing text into a useable format
namespace pimpl::parsing
{

/// Parse a string and turn it into a useable Sentence class representation
/**
 * @param text A string of text which uses the predicate logic syntax specified
 *        in the README
 * @return The equivalent Sentence object
 */
Sentence parseString(std::string text);

/// 'private' functions for parsing
/**
 * Other files shouldn't need to worry about the dirty details of how the
 * parsing, lexing, etc. works, nor should they depend on the exact
 * implementation since I have no idea what I'm doing.
 * The functions in this namespace are subject to change once/if more
 * complicated text syntax is supported in the future, thus other code should
 * use the abstracted functions in the parent namespace.
 */
namespace impl
{

/// An Enum to represent possible tokens, with values for operator precedence
enum class Token
{
  LEFT_PAREN = 0,
  RIGHT_PAREN = 1,
  BICONDITIONAL = 2,
  IMPLICATION = 3,
  DISJUNCTION = 4,
  CONJUNCTION = 5,
  NOT = 6,
  SYMBOL = 7,
};

/// Turn a string into tokens
std::vector<Token> tokenize(std::string text);

/// Convert a string **in conjunctive normal form** into a Sentence object
Sentence parseConjunctiveNormal(std::string text);

} // namespace impl

} // namespace pimpl::parsing

#endif  // #ifndef __PIMPL__PARSING_HPP__

