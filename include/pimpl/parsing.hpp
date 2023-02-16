#include <memory>

#include "lexy/dsl.hpp"
#include "lexy/callback.hpp"

#include "pimpl/sentence.hpp"

namespace pimpl
{

namespace ast
{

struct AbstractBase
{
    //virtual std::string as_str() = 0;
};

using abstract_ptr = std::shared_ptr<AbstractBase>;

struct AbstractSymbol : public AbstractBase
{
    std::string name;

    explicit AbstractSymbol(std::string n) : name(std::move(n)) {}

    //std::string as_str() override { return name; }
};

struct AbstractBool : public AbstractBase
{
    bool value;

    explicit AbstractBool(bool v) : value(v) {}

    //std::string as_str() override { return value ? "T" : "F"; }
};

struct AbstractUnary : public AbstractBase
{
    enum Op {
        NOT,
    } op;

    abstract_ptr right;

    explicit AbstractUnary(Op o, abstract_ptr r) : op(o), right(std::move(r)) {}

    //std::string as_str() override { return "~" + right->as_str(); }
};

struct AbstractBinary : public AbstractBase
{
    enum Op {
        AND,
        OR,
        IMP,
        IFF,
    } op;

    abstract_ptr left;
    abstract_ptr right;

    explicit AbstractBinary(abstract_ptr l, Op o, abstract_ptr r)
        : op(o), left(std::move(l)), right(std::move(r)) {}

    //std::string as_str() override
    //{
    //    std::string op_str;
    //    switch (op) {
    //        using enum Op;
    //        case AND:
    //            op_str = " & ";
    //            break;
    //        case OR:
    //            op_str = " | ";
    //            break;
    //        case IMP:
    //            op_str = " => ";
    //            break;
    //        case IFF:
    //            op_str = " <=> ";
    //    }
    //    return "(" + left->as_str() + op_str + right->as_str() + ")";
    //}
};

}   // namespace ast

namespace grammar
{

namespace dsl = lexy::dsl;

struct Symbol : lexy::token_production
{
    static constexpr auto rule = dsl::identifier(dsl::ascii::alpha, dsl::ascii::word);
    static constexpr auto value = lexy::as_string<std::string, lexy::utf8_encoding>;
};

struct Bool : lexy::token_production
{
    struct true_ : lexy::transparent_production
    {
        static constexpr auto rule = LEXY_LIT("T");
        static constexpr auto value = lexy::constant(true);
    };

    struct false_ : lexy::transparent_production
    {
        static constexpr auto rule = LEXY_LIT("F");
        static constexpr auto value = lexy::constant(false);
    };

    static constexpr auto rule = dsl::p<true_> | dsl::p<false_>;
    static constexpr auto value = lexy::forward<bool>;
};

struct NestedExpr : lexy::transparent_production
{
    static constexpr auto whitespace = dsl::ascii::blank;
    static constexpr auto rule = dsl::recurse<struct Expr>;
    static constexpr auto value = lexy::forward<ast::abstract_ptr>;
};

struct Expr : lexy::expression_production
{
    struct ExpectedOperand
    {
        static constexpr auto name = "expected operand";
    };

    static constexpr auto atom = [] {
        auto paren_expr = dsl::parenthesized(dsl::p<NestedExpr>);
        return paren_expr | dsl::p<Bool> | dsl::p<Symbol> | dsl::error<ExpectedOperand>;
    }();

    struct Not : dsl::prefix_op
    {
        static constexpr auto op = dsl::op<ast::AbstractUnary::Op::NOT>(LEXY_LIT("~"));
        using operand = dsl::atom;
    };

    struct And : dsl::infix_op_left
    {
        static constexpr auto op = dsl::op<ast::AbstractBinary::Op::AND>(LEXY_LIT("&"));
        using operand = Not;
    };

    struct Or : dsl::infix_op_left
    {
        static constexpr auto op = dsl::op<ast::AbstractBinary::Op::OR>(LEXY_LIT("|"));
        using operand = And;
    };

    struct Imp : dsl::infix_op_left
    {
        static constexpr auto op = dsl::op<ast::AbstractBinary::Op::IMP>(LEXY_LIT("=>"));
        using operand = Or;
    };

    struct Iff : dsl::infix_op_left
    {
        static constexpr auto op = dsl::op<ast::AbstractBinary::Op::IFF>(LEXY_LIT("<=>"));
        using operand = Imp;
    };

    using operation = Iff;

    static constexpr auto value = lexy::callback(
        lexy::forward<ast::abstract_ptr>,
        lexy::new_<ast::AbstractSymbol, ast::abstract_ptr>,
        lexy::new_<ast::AbstractBool, ast::abstract_ptr>,
        lexy::new_<ast::AbstractUnary, ast::abstract_ptr>,
        lexy::new_<ast::AbstractBinary, ast::abstract_ptr>);
};

//struct GrammarSentence
//{
//    static constexpr auto max_recursion_depth = 19;
//
//    static constexpr auto whitespace = dsl::ascii::blank | dsl::ascii::other_space;
//
//    //static constexpr auto rule = [] {
//    //    auto at_eol = dsl::peek(dsl::eol);
//    //    return dsl::terminator(at_eol).opt_list(dsl::p<Expr>, dsl::sep(dsl::ascii::newline));
//    //}();
//    static constexpr auto rule = dsl::terminator(dsl::eol).opt_list(dsl::p<Expr>, dsl::sep(dsl::ascii::newline));
//    static constexpr auto value = lexy::as_list<std::vector<ast::abstract_ptr>>;
//};

struct GrammarSentence
{
    static constexpr auto max_recursion_depth = 19;

    static constexpr auto whitespace = dsl::ascii::space;

    static constexpr auto rule = dsl::p<Expr> + dsl::eof;
    static constexpr auto value = lexy::forward<ast::abstract_ptr>;
};

}   // namespace grammar

}   // namespace pimpl
