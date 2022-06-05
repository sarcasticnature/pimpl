#include "pimpl/parsing.hpp"

namespace pimpl::parsing
{

Sentence parseSentence(std::string text)
{
  return impl::stringToSentence(text);
}

namespace impl
{

std::vector<Token> tokenize(std::string text)
{
  return {};
}

Sentence stringToSentence(std::string text)
{
  return Sentence();
}

} // namespace impl

} // namespace pimpl::parsing

