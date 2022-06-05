#include "pimpl/parsing.hpp"

namespace pimpl::parsing
{

Sentence parseSentence(std::string text)
{
  return impl::parseConjunctiveNormal(text);
}

namespace impl
{

std::vector<Token> tokenize(std::string text)
{
  return {};
}

Sentence parseConjunctiveNormal(std::string text)
{
  return Sentence();
}

} // namespace impl

} // namespace pimpl::parsing

