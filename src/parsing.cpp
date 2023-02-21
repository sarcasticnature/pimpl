#include <unordered_set>

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
