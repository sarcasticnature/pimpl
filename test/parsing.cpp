#include <typeinfo>

#include "doctest/doctest.h"
#include "lexy/action/match.hpp"
#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"

#include "pimpl/parsing.hpp"

namespace
{

//void pass(const char* str)
//{
//    auto input = lexy::zstring_input<lexy::utf8_encoding>(str);
//    INFO(str);
//    REQUIRE(lexy::match<pimpl::grammar::GrammarSentence>(input));
//}
//
//void fail(const char* str)
//{
//    auto input = lexy::zstring_input<lexy::utf8_encoding>(str);
//    INFO(str);
//    REQUIRE(!lexy::match<pimpl::grammar::GrammarSentence>(input));
//}
//
void pass_check(const char* str)
{
    auto input = lexy::zstring_input<lexy::utf8_encoding>(str);
    INFO(str);
    CHECK(lexy::match<pimpl::grammar::GrammarSentence>(input));
}
//
//void fail_check(const char* str)
//{
//    auto input = lexy::zstring_input<lexy::utf8_encoding>(str);
//    INFO(str);
//    CHECK(!lexy::match<pimpl::grammar::GrammarSentence>(input));
//}

auto parse_str(const char* str)
{
    auto input = lexy::zstring_input<lexy::utf8_encoding>(str);
    auto result = lexy::parse<pimpl::grammar::GrammarSentence>(input, lexy::noop);
    INFO(str);
    REQUIRE(result);
    return result.value();
}

auto parse_first(const char* str)
{
    auto vec = parse_str(str);
    REQUIRE(vec.size() == 1);
    return vec.front();
}

}   // namespace

TEST_CASE("lexy::match every operator")
{
    pass_check("foo");
    pass_check("F");
    pass_check("~bar");
    pass_check("baz & T");
    pass_check("fi | fie");
    pass_check("fo => fum");
    pass_check("F <=> f");
}

TEST_CASE("Manual verification of AST structure")
{
    //NOLINTBEGIN(clang-diagnostic-potentially-evaluated-expression)
    using namespace pimpl::ast;

    abstract_ptr ptr;

    SUBCASE("Symbol") {
        ptr = parse_first("foo");
        CHECK(typeid(*ptr) == typeid(AbstractSymbol));
        CHECK(dynamic_cast<AbstractSymbol&>(*ptr).name == "foo");

        ptr = parse_first("t");
        CHECK(typeid(*ptr) == typeid(AbstractSymbol));
        CHECK(dynamic_cast<AbstractSymbol&>(*ptr).name == "t");
    }
    
    SUBCASE("Bool") {
        ptr = parse_first("T");
        CHECK(typeid(*ptr) == typeid(AbstractBool));
        CHECK(dynamic_cast<AbstractBool&>(*ptr).value == true);

        ptr = parse_first("F");
        CHECK(typeid(*ptr) == typeid(AbstractBool));
        CHECK(dynamic_cast<AbstractBool&>(*ptr).value == false);
    }

    SUBCASE("Not") {
        ptr = parse_first("~bar");
        CHECK(typeid(*ptr) == typeid(AbstractUnary));
        auto not_ = dynamic_cast<AbstractUnary&>(*ptr);
        CHECK(not_.op == AbstractUnary::NOT);

        CHECK(typeid(*not_.right) == typeid(AbstractSymbol));
        CHECK(dynamic_cast<AbstractSymbol&>(*not_.right).name == "bar");
    }

    SUBCASE("And") {
        ptr = parse_first("a & b");
        CHECK(typeid(*ptr) == typeid(AbstractBinary));
        auto and_ = dynamic_cast<AbstractBinary&>(*ptr);
        CHECK(and_.op == AbstractBinary::AND);

        CHECK(typeid(*and_.left) == typeid(AbstractSymbol));
        CHECK(dynamic_cast<AbstractSymbol&>(*and_.left).name == "a");
        CHECK(typeid(*and_.right) == typeid(AbstractSymbol));
        CHECK(dynamic_cast<AbstractSymbol&>(*and_.right).name == "b");
    }

    SUBCASE("Or") {
        ptr = parse_first("T | F");
        CHECK(typeid(*ptr) == typeid(AbstractBinary));
        auto or_ = dynamic_cast<AbstractBinary&>(*ptr);
        CHECK(or_.op == AbstractBinary::OR);

        CHECK(typeid(*or_.left) == typeid(AbstractBool));
        CHECK(dynamic_cast<AbstractBool&>(*or_.left).value == true);
        CHECK(typeid(*or_.right) == typeid(AbstractBool));
        CHECK(dynamic_cast<AbstractBool&>(*or_.right).value == false);
    }

    SUBCASE("Imp") {
        ptr = parse_first("c => T");
        CHECK(typeid(*ptr) == typeid(AbstractBinary));
        auto imp = dynamic_cast<AbstractBinary&>(*ptr);
        CHECK(imp.op == AbstractBinary::IMP);

        CHECK(typeid(*imp.left) == typeid(AbstractSymbol));
        CHECK(dynamic_cast<AbstractSymbol&>(*imp.left).name == "c");
        CHECK(typeid(*imp.right) == typeid(AbstractBool));
        CHECK(dynamic_cast<AbstractBool&>(*imp.right).value == true);
    }

    SUBCASE("Iff") {
        ptr = parse_first("F <=> d");
        CHECK(typeid(*ptr) == typeid(AbstractBinary));
        auto iff = dynamic_cast<AbstractBinary&>(*ptr);
        CHECK(iff.op == AbstractBinary::IFF);

        CHECK(typeid(*iff.left) == typeid(AbstractBool));
        CHECK(dynamic_cast<AbstractBool&>(*iff.left).value == false);
        CHECK(typeid(*iff.right) == typeid(AbstractSymbol));
        CHECK(dynamic_cast<AbstractSymbol&>(*iff.right).name == "d");
    }
    //NOLINTEND(clang-diagnostic-potentially-evaluated-expression)
}
