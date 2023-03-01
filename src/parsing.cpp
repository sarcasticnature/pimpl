#include <unordered_set>

#include "doctest/doctest.h"

#include "pimpl/parsing.hpp"

namespace pimpl::ast
{

inline Sentence::sentence_ptr_t sentenceBuilder(
    const abstract_ptr ast_ptr,
    std::unordered_set<Sentence::sentence_ptr_t>& symbol_set)
{
    if (!ast_ptr) {
        return std::make_shared<Sentence::sentence_t>(std::monostate());
    }

    if (auto ast_symbol = std::dynamic_pointer_cast<AbstractSymbol>(ast_ptr)) {
        auto symbol = std::make_shared<Sentence::sentence_t>(ast_symbol->name);
        symbol_set.insert(symbol);
        return symbol;
    }

    if (auto ast_bool = std::dynamic_pointer_cast<AbstractBool>(ast_ptr)) {
        return std::make_shared<Sentence::sentence_t>(ast_bool->value);
    }

    if (auto ast_unary = std::dynamic_pointer_cast<AbstractUnary>(ast_ptr)) {
        return std::make_shared<Sentence::sentence_t>(
            Sentence::Not(sentenceBuilder(ast_unary->right, symbol_set)));
    }

    if (auto ast_binary = std::dynamic_pointer_cast<AbstractBinary>(ast_ptr)) {
        Sentence::sentence_ptr_t sentence_ptr;
        switch (ast_binary->op) {
            case AbstractBinary::AND:
                sentence_ptr = std::make_shared<Sentence::sentence_t>(Sentence::And(
                    sentenceBuilder(ast_binary->left, symbol_set),
                    sentenceBuilder(ast_binary->right, symbol_set)));
                break;
            case AbstractBinary::OR:
                sentence_ptr = std::make_shared<Sentence::sentence_t>(Sentence::Or(
                    sentenceBuilder(ast_binary->left, symbol_set),
                    sentenceBuilder(ast_binary->right, symbol_set)));
                break;
            case AbstractBinary::IMP:
                sentence_ptr = std::make_shared<Sentence::sentence_t>(Sentence::Imp(
                    sentenceBuilder(ast_binary->left, symbol_set),
                    sentenceBuilder(ast_binary->right, symbol_set)));
                break;
            case AbstractBinary::IFF:
                sentence_ptr = std::make_shared<Sentence::sentence_t>(Sentence::Iff(
                    sentenceBuilder(ast_binary->left, symbol_set),
                    sentenceBuilder(ast_binary->right, symbol_set)));
                break;
        }
        return sentence_ptr;
    }

    // _should_ never hit this
    return nullptr;
}

Sentence toSentence(abstract_ptr ast_ptr)
{
    Sentence::sentence_ptr_t sentence_ptr;

    std::unordered_set<Sentence::sentence_ptr_t> symbol_set;

    if (ast_ptr == nullptr) {
        return Sentence(sentence_ptr, symbol_set);
    }

    sentence_ptr = sentenceBuilder(ast_ptr, symbol_set);
    return Sentence(sentence_ptr, symbol_set);
}

}   // namespace pimpl::ast

////////////////////////////////////////////////////////////////////////////////

#ifdef PIMPL_ENABLE_TESTS
#include <typeinfo>

#include "doctest/doctest.h"
#include "lexy/action/match.hpp"
#include "lexy/action/parse.hpp"
#include "lexy/input/string_input.hpp"

namespace
{

//void pass(const char* str)
//{
//    auto input = lexy::zstring_input<lexy::utf8_encoding>(str);
//    INFO(std::string(str));
//    REQUIRE(lexy::match<pimpl::grammar::GrammarSentence>(input));
//}
//
//void fail(const char* str)
//{
//    auto input = lexy::zstring_input<lexy::utf8_encoding>(str);
//    INFO(std::string(str));
//    REQUIRE(!lexy::match<pimpl::grammar::GrammarSentence>(input));
//}
//
void pass_check(const char* str)
{
    auto input = lexy::zstring_input<lexy::utf8_encoding>(str);
    INFO(std::string(str));
    CHECK(lexy::match<pimpl::grammar::GrammarSentence>(input));
}
//
//void fail_check(const char* str)
//{
//    auto input = lexy::zstring_input<lexy::utf8_encoding>(str);
//    INFO(std::string(str));
//    CHECK(!lexy::match<pimpl::grammar::GrammarSentence>(input));
//}

auto parse_str(const char* str)
{
    auto input = lexy::zstring_input<lexy::utf8_encoding>(str);
    auto result = lexy::parse<pimpl::grammar::GrammarSentence>(input, lexy::noop);
    INFO(std::string(str));
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

        ptr = parse_first("True");
        CHECK(typeid(*ptr) == typeid(AbstractSymbol));
        CHECK(dynamic_cast<AbstractSymbol&>(*ptr).name == "True");

        ptr = parse_first("eslaF");
        CHECK(typeid(*ptr) == typeid(AbstractSymbol));
        CHECK(dynamic_cast<AbstractSymbol&>(*ptr).name == "eslaF");

        ptr = parse_first("tTfFTTff");
        CHECK(typeid(*ptr) == typeid(AbstractSymbol));
        CHECK(dynamic_cast<AbstractSymbol&>(*ptr).name == "tTfFTTff");

        ptr = parse_first("my_42");
        CHECK(typeid(*ptr) == typeid(AbstractSymbol));
        CHECK(dynamic_cast<AbstractSymbol&>(*ptr).name == "my_42");
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

// TODO: add checks for symbol_set in subcases
TEST_CASE("sentenceBuilder")
{
    using namespace pimpl;

    ast::abstract_ptr ast_ptr = nullptr;
    std::unordered_set<Sentence::sentence_ptr_t> symbol_set;
    Sentence::sentence_ptr_t sentence_ptr = nullptr;
    Sentence::sentence_ptr_t left_ptr = nullptr;
    Sentence::sentence_ptr_t right_ptr = nullptr;
    
    SUBCASE("monostate") {
        sentence_ptr = sentenceBuilder(ast_ptr, symbol_set);
        REQUIRE(sentence_ptr->index() == Sentence::INDEX_MONOSTATE);
        REQUIRE(std::holds_alternative<std::monostate>(*sentence_ptr));
        REQUIRE(std::get<Sentence::INDEX_MONOSTATE>(*sentence_ptr) == std::monostate());
        REQUIRE(symbol_set.empty());
    }

    SUBCASE("Symbol") {
        ast_ptr = parse_first("FOO");
        sentence_ptr = sentenceBuilder(ast_ptr, symbol_set);
        REQUIRE(sentence_ptr->index() == Sentence::INDEX_SYMBOL);
        REQUIRE(std::holds_alternative<std::string>(*sentence_ptr));
        REQUIRE(std::get<Sentence::INDEX_SYMBOL>(*sentence_ptr) == "FOO");

        CHECK(symbol_set.size() == 1);
        CHECK(symbol_set.find(sentence_ptr) != symbol_set.end());
    }

    SUBCASE("Bool") {
        ast_ptr = parse_first("T");
        sentence_ptr = sentenceBuilder(ast_ptr, symbol_set);
        REQUIRE(sentence_ptr->index() == Sentence::INDEX_BOOL);
        REQUIRE(std::holds_alternative<bool>(*sentence_ptr));
        REQUIRE(std::get<Sentence::INDEX_BOOL>(*sentence_ptr) == true);

        ast_ptr = parse_first("F");
        sentence_ptr = sentenceBuilder(ast_ptr, symbol_set);
        REQUIRE(sentence_ptr->index() == Sentence::INDEX_BOOL);
        REQUIRE(std::holds_alternative<bool>(*sentence_ptr));
        REQUIRE(std::get<Sentence::INDEX_BOOL>(*sentence_ptr) == false);

        CHECK(symbol_set.empty());
    }

    SUBCASE("Or") {
        ast_ptr = parse_first("~L33t");
        sentence_ptr = sentenceBuilder(ast_ptr, symbol_set);
        REQUIRE(sentence_ptr->index() == Sentence::INDEX_NOT);
        REQUIRE(std::holds_alternative<Sentence::Not>(*sentence_ptr));
        REQUIRE_NOTHROW(right_ptr = std::get<Sentence::INDEX_NOT>(*sentence_ptr).right);

        REQUIRE(right_ptr->index() == Sentence::INDEX_SYMBOL);
        REQUIRE(std::get<Sentence::INDEX_SYMBOL>(*right_ptr) == "L33t");

        CHECK(symbol_set.size() == 1);
        CHECK(symbol_set.find(right_ptr) != symbol_set.end());
    }

    SUBCASE("And") {
        ast_ptr = parse_first("a & b");
        sentence_ptr = sentenceBuilder(ast_ptr, symbol_set);
        REQUIRE(sentence_ptr->index() == Sentence::INDEX_AND);
        REQUIRE(std::holds_alternative<Sentence::And>(*sentence_ptr));
        REQUIRE_NOTHROW(left_ptr = std::get<Sentence::INDEX_AND>(*sentence_ptr).left);
        REQUIRE_NOTHROW(right_ptr = std::get<Sentence::INDEX_AND>(*sentence_ptr).right);

        REQUIRE(left_ptr->index() == Sentence::INDEX_SYMBOL);
        REQUIRE(std::get<Sentence::INDEX_SYMBOL>(*left_ptr) == "a");

        REQUIRE(right_ptr->index() == Sentence::INDEX_SYMBOL);
        REQUIRE(std::get<Sentence::INDEX_SYMBOL>(*right_ptr) == "b");

        CHECK(symbol_set.size() == 2);
        CHECK(symbol_set.find(right_ptr) != symbol_set.end());
        CHECK(symbol_set.find(left_ptr) != symbol_set.end());
    }

    SUBCASE("Or") {
        ast_ptr = parse_first("T | F");
        sentence_ptr = sentenceBuilder(ast_ptr, symbol_set);
        REQUIRE(sentence_ptr->index() == Sentence::INDEX_OR);
        REQUIRE(std::holds_alternative<Sentence::Or>(*sentence_ptr));
        REQUIRE_NOTHROW(left_ptr = std::get<Sentence::INDEX_OR>(*sentence_ptr).left);
        REQUIRE_NOTHROW(right_ptr = std::get<Sentence::INDEX_OR>(*sentence_ptr).right);

        REQUIRE(left_ptr->index() == Sentence::INDEX_BOOL);
        REQUIRE(std::get<Sentence::INDEX_BOOL>(*left_ptr) == true);

        REQUIRE(right_ptr->index() == Sentence::INDEX_BOOL);
        REQUIRE(std::get<Sentence::INDEX_BOOL>(*right_ptr) == false);

        CHECK(symbol_set.size() == 0);
    }

    SUBCASE("Imp") {
        ast_ptr = parse_first("c => T");
        sentence_ptr = sentenceBuilder(ast_ptr, symbol_set);
        REQUIRE(sentence_ptr->index() == Sentence::INDEX_IMP);
        REQUIRE(std::holds_alternative<Sentence::Imp>(*sentence_ptr));
        REQUIRE_NOTHROW(left_ptr = std::get<Sentence::INDEX_IMP>(*sentence_ptr).left);
        REQUIRE_NOTHROW(right_ptr = std::get<Sentence::INDEX_IMP>(*sentence_ptr).right);

        REQUIRE(left_ptr->index() == Sentence::INDEX_SYMBOL);
        REQUIRE(std::get<Sentence::INDEX_SYMBOL>(*left_ptr) == "c");

        REQUIRE(right_ptr->index() == Sentence::INDEX_BOOL);
        REQUIRE(std::get<Sentence::INDEX_BOOL>(*right_ptr) == true);

        CHECK(symbol_set.size() == 1);
        CHECK(symbol_set.find(left_ptr) != symbol_set.end());
    }

    SUBCASE("Iff") {
        ast_ptr = parse_first("F <=> d");
        sentence_ptr = sentenceBuilder(ast_ptr, symbol_set);
        REQUIRE(sentence_ptr->index() == Sentence::INDEX_IFF);
        REQUIRE(std::holds_alternative<Sentence::Iff>(*sentence_ptr));
        REQUIRE_NOTHROW(left_ptr = std::get<Sentence::INDEX_IFF>(*sentence_ptr).left);
        REQUIRE_NOTHROW(right_ptr = std::get<Sentence::INDEX_IFF>(*sentence_ptr).right);

        REQUIRE(left_ptr->index() == Sentence::INDEX_BOOL);
        REQUIRE(std::get<Sentence::INDEX_BOOL>(*left_ptr) == false);

        REQUIRE(right_ptr->index() == Sentence::INDEX_SYMBOL);
        REQUIRE(std::get<Sentence::INDEX_SYMBOL>(*right_ptr) == "d");

        CHECK(symbol_set.size() == 1);
        CHECK(symbol_set.find(right_ptr) != symbol_set.end());
    }
}

// TODO: add basic toSentence test(s)

#endif  // PIMPL_ENABLE_TESTS
