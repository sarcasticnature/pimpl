#include <iostream>
#include <string>
#include <cstdio>

#include "pimpl/parsing.hpp"
#include "lexy/action/parse.hpp"
#include "lexy/action/parse_as_tree.hpp"
#include "lexy/action/trace.hpp"
#include "lexy/input/file.hpp"
#include "lexy_ext/report_error.hpp"
#include "lexy/visualize.hpp"

int main(int argc, char** argv)
{
    if (argc < 2)
    {
        std::fprintf(stderr, "usage: %s <filename>", argv[0]);
        return 1;
    }

    // We're requiring UTF-8 input.
    auto file = lexy::read_file<lexy::utf8_encoding>(argv[1]);
    if (!file)
    {
        std::fprintf(stderr, "file '%s' not found", argv[1]);
        return 1;
    }

    //lexy::parse_tree_for<decltype(file.buffer())> tree{};
    //auto result = lexy::parse_as_tree<pimpl::grammar::GrammarSentence>(tree, file.buffer(), lexy_ext::report_error);
    //if (!result) return 2;
    //lexy::visualize(stdout, tree, {lexy::visualize_fancy});
    //lexy::trace<pimpl::grammar::GrammarSentence>(stdout, file.buffer(), {lexy::visualize_fancy});

    //auto sentence = lexy::parse<pimpl::grammar::GrammarSentence>(file.buffer(), lexy_ext::report_error);
    //if (sentence.has_value()) {
    //    std::cout << sentence.value()->as_str() << std::endl;
    //}

    auto sentence = lexy::parse<pimpl::grammar::GrammarSentence>(file.buffer(), lexy_ext::report_error);
    if (sentence.has_value()) {
        for (const auto& s : sentence.value()) {
            std::cout << s->as_str() << std::endl;
        }
    }

    if (!sentence)
        return 2;
}
