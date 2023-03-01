#ifndef __PIMPL__TYPES_HPP__
#define __PIMPL__TYPES_HPP__

#include <string>
#include <variant>
#include <memory>
#include <stack>
#include <unordered_map>

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

private:
    sentence_ptr_t data_;
    std::unordered_map<std::string, sentence_ptr_t> symbols_;
};

}   // namespace pimpl

#endif  // __PIMPL__TYPES_HPP__
