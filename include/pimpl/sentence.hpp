#ifndef __PIMPL__SENTENCE_HPP__
#define __PIMPL__SENTENCE_HPP__

#include <string>
#include <variant>
#include <memory>
#include <stack>
#include <unordered_map>
#include <optional>

namespace pimpl
{

class Sentence
{
public:
    struct Not;
    struct And;
    struct Or;
    struct Imp;
    struct Iff;
    
    using sentence_t = std::variant<std::monostate, std::string, bool, Not, And, Or, Imp, Iff>;
    using sentence_ptr_t = std::shared_ptr<sentence_t>;
    
    static constexpr size_t INDEX_MONOSTATE = 0;
    static constexpr size_t INDEX_SYMBOL = 1;
    static constexpr size_t INDEX_BOOL = 2;
    static constexpr size_t INDEX_NOT = 3;
    static constexpr size_t INDEX_AND = 4;
    static constexpr size_t INDEX_OR = 5;
    static constexpr size_t INDEX_IMP = 6;
    static constexpr size_t INDEX_IFF = 7;

    struct Not
    {
        sentence_ptr_t right;

        explicit Not(sentence_ptr_t r) : right(r) {}

        bool operator==(const Not& rhs) const
        {
            return *right == *rhs.right;
        }
    };
    
    struct And
    {
        sentence_ptr_t left;
        sentence_ptr_t right;

        explicit And(sentence_ptr_t l, sentence_ptr_t r) : left(l), right(r) {}

        bool operator==(const And& rhs) const
        {
            return *right == *rhs.right && *left == *rhs.left;
        }
    };
    
    struct Or
    {
        sentence_ptr_t left;
        sentence_ptr_t right;

        explicit Or(sentence_ptr_t l, sentence_ptr_t r) : left(l), right(r) {}

        bool operator==(const Or& rhs)  const
        {
            return *right == *rhs.right && *left == *rhs.left;
        }
    };
    
    struct Imp
    {
        sentence_ptr_t left;
        sentence_ptr_t right;

        explicit Imp(sentence_ptr_t l, sentence_ptr_t r) : left(l), right(r) {}

        bool operator==(const Imp& rhs) const
        {
            return *right == *rhs.right && *left == *rhs.left;
        }
    };
    
    struct Iff
    {
        sentence_ptr_t left;
        sentence_ptr_t right;

        explicit Iff(sentence_ptr_t l, sentence_ptr_t r) : left(l), right(r) {}

        bool operator==(const Iff& rhs) const
        {
            return *right == *rhs.right && *left == *rhs.left;
        }
    };

    // On hold as an Iterator may be less useful than recursive functions
    //struct Sentinel;
    //class Iterator
    //{
    //public:
    //    using value_type = sentence_t;
    //    using difference_type = std::ptrdiff_t;

    //    Iterator() : data_(std::make_shared<sentence_t>(std::monostate())) {}
    //    Iterator(const Iterator& it) : data_(it.data_), stack_(it.stack_) {}
    //    Iterator operator=(const Iterator& rhs)
    //    {
    //        Iterator lhs;
    //        lhs.data_ = rhs.data_;
    //        lhs.stack_ = rhs.stack_;
    //        return lhs;
    //    }

    //    sentence_t& operator*() const { return *data_; }
    //    sentence_ptr_t operator->() const { return data_; }

    //    bool operator==(const Iterator& it) const
    //    {
    //        return *data_ == *it;
    //    }

    //    bool operator==(const Sentinel& it) const
    //    { 
    //        return data_->index() == INDEX_MONOSTATE;
    //    }
    //    Iterator& operator++()
    //    {
    //        switch (data_->index()) {
    //            case INDEX_MONOSTATE:
    //                if (!stack_.empty()) {
    //                    data_ = stack_.top();
    //                    stack_.pop();
    //                }
    //                break;
    //            case INDEX_SYMBOL:
    //            case INDEX_BOOL:
    //                if (!stack_.empty()) {
    //                    data_ = stack_.top();
    //                    stack_.pop();
    //                } else {
    //                    data_ = std::make_shared<sentence_t>(std::monostate());
    //                }
    //                break;
    //            case INDEX_NOT:
    //                data_ = data_->right;
    //                break;
    //            case INDEX_AND:
    //            case INDEX_OR:
    //            case INDEX_IMP:
    //            case INDEX_IFF:
    //            //default:
    //        }
    //        return *this;
    //    }
    //    Iterator operator++(int);
    //private:
    //    sentence_ptr_t data_;
    //    std::stack<sentence_ptr_t> stack_;
    //};

    //struct Sentinel
    //{
    //    //Sentinel() = default;
    //    bool operator==(Iterator it) const
    //    {
    //        return it->index() == INDEX_MONOSTATE;
    //    }
    //};

    Sentence() = default;
    Sentence(sentence_ptr_t d, std::unordered_map<std::string, sentence_ptr_t> v)
        : data_(d), symbols_(v)
    {
    }

    // Subtitute all symbols with a boolean value and determine if the sentence is true/false
    // Returns std::nullopt if a problem was encountered,
    // e.g. incorrect input symbols, issues with the sentence structure, etc
    std::optional<bool> evaluate(std::unordered_map<std::string, bool> symbol_values);

    // Check if the sentence evaluates to true or false
    // All symbols are expected to have been substituted for bools,
    // thus nullopt is returned if any are present
    std::optional<bool> truth();

private:
    sentence_ptr_t data_;
    std::unordered_map<std::string, sentence_ptr_t> symbols_;

    // TODO switch to const references?
    struct TruthVisitor
    {
        static bool negate (bool b) { return !b; };

        std::optional<bool> operator()(std::monostate _) { return std::nullopt; }
        std::optional<bool> operator()(std::string _) { return std::nullopt; }
        std::optional<bool> operator()(bool b) { return {b}; }
        std::optional<bool> operator()(Not n)
        {
            return std::visit(*this, *n.right).transform(negate);
        }

        std::optional<bool> operator()(And a)
        {
            return std::visit(*this, *a.left).and_then(
                [this, &a](bool b) -> std::optional<bool> {
                    if (b) return std::visit(*this, *a.right);
                    else return {false};
                }
            );
        }

        std::optional<bool> operator()(Or o)
        {
            return std::visit(*this, *o.left).and_then(
                [this, &o](bool b) -> std::optional<bool> {
                    if (b) return {true};
                    else return std::visit(*this, *o.right);
                }
            );
        }

        std::optional<bool> operator()(Imp i)
        {
            return std::visit(*this, *i.right).and_then(
                [this, &i](bool b) -> std::optional<bool> {
                    if (b) return {true};
                    else return std::visit(*this, *i.left).transform(negate);
                }
            );
        }

        std::optional<bool> operator()(Iff i)
        {
            return std::visit(*this, *i.left).and_then(
                [this, &i](bool b) -> std::optional<bool> {
                    if (b) return std::visit(*this, *i.right);
                    else return std::visit(*this, *i.right).transform(negate);
                }
            );
        }
    };

    // TODO switch to const references?
    struct NenfVisitor
    {
        static bool negate (bool b) { return !b; };

        // TODO: is monostate error worthy?
        sentence_t operator()(std::monostate _) { return std::monostate{}; }
        sentence_t operator()(std::string s) { return s; }
        sentence_t operator()(bool b) { return b; }

        sentence_t operator()(Not n)
        {
            sentence_ptr_t left, right;
            switch (n.right->index()) {
                case INDEX_NOT:
                    return *std::get<Not>(*n.right).right;
                case INDEX_AND:
                    left = std::make_shared<sentence_t>(Not(std::get<And>(*n.right).left));
                    right = std::make_shared<sentence_t>(Not(std::get<And>(*n.right).right));
                    *left = std::visit(*this, *left);
                    *right = std::visit(*this, *right);
                    return Or(left, right);
                case INDEX_OR:
                    left = std::make_shared<sentence_t>(Not(std::get<Or>(*n.right).left));
                    right = std::make_shared<sentence_t>(Not(std::get<Or>(*n.right).right));
                    *left = std::visit(*this, *left);
                    *right = std::visit(*this, *right);
                    return And(left, right);
                case INDEX_IMP:
                    right = std::make_shared<sentence_t>(Not(std::get<Imp>(*n.right).right));
                    *left = std::visit(*this, *std::get<Imp>(*n.right).left);
                    *right = std::visit(*this, *right);
                    return And(left, right);
                case INDEX_IFF:
                    right = std::make_shared<sentence_t>(Not(std::get<Iff>(*n.right).right));
                    *left = std::visit(*this, *std::get<Iff>(*n.right).left);
                    *right = std::visit(*this, *right);
                    return Iff(left, right);
            }

            return n;
        }

        sentence_t operator()(And a)
        {
            *a.left = std::visit(*this, *a.left);
            *a.right = std::visit(*this, *a.right);
            return a;
        }

        sentence_t operator()(Or o)
        {
            *o.left = std::visit(*this, *o.left);
            *o.right = std::visit(*this, *o.right);
            return o;
        }

        sentence_t operator()(Imp i)
        {
            sentence_ptr_t left = std::make_shared<sentence_t>(Not(i.left));
            return Or(left, i.right);
        }

        sentence_t operator()(Iff i)
        {
            *i.left = std::visit(*this, *i.left);
            *i.right = std::visit(*this, *i.right);
            return i;
        }
    };
};

}   // namespace pimpl

#endif  // __PIMPL__SENTENCE_HPP__
