#include <algorithm>

#include "doctest/doctest.h"

#include "pimpl/sentence.hpp"

namespace pimpl
{

std::optional<bool> Sentence::evaluate(std::unordered_map<std::string, bool> symbol_values)
{
    if (symbols_.size() != symbol_values.size()) {
        return std::nullopt;
    }

    // substitute the provided bools into each symbol
    for (const auto& [key, val] : symbol_values) {
        if (!symbols_.contains(key)) {
            return std::nullopt;
        }
        *symbols_.at(key) = val;
    }

    // evaluate the sentence
    auto res = truth();

    // reset the sentence
    std::ranges::for_each(symbols_, [](const auto& s) { *s.second = s.first; });

    return res;
}

std::optional<bool> Sentence::truth()
{
    if (data_ == nullptr) {
        return std::nullopt;
    }
    return std::visit(TruthVisitor{}, *data_);
}

}   // namespace pimpl

TEST_CASE("truth()")
{
    using namespace pimpl;
    using s_t = Sentence::sentence_t;

    Sentence::sentence_ptr_t data;
    std::unordered_map<std::string, Sentence::sentence_ptr_t> symbols;

    SUBCASE("monostate") {
        data = std::make_shared<s_t>(std::monostate());
        Sentence sentence(data, symbols);
        CHECK(sentence.truth() == std::nullopt);
    }

    SUBCASE("symbol") {
        data = std::make_shared<s_t>("foo");
        symbols.insert({"foo", data});
        Sentence sentence(data, symbols);
        CHECK(sentence.truth() == std::nullopt);
    }

    SUBCASE("bool") {
        SUBCASE("true") {
            data = std::make_shared<s_t>(true);
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == true);
        }

        SUBCASE("false") {
            data = std::make_shared<s_t>(false);
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == false);
        }
    }

    SUBCASE("not") {
        SUBCASE("~true") {
            data = std::make_shared<s_t>(Sentence::Not(std::make_shared<s_t>(true)));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == false);
        }

        SUBCASE("~false") {
            data = std::make_shared<s_t>(Sentence::Not(std::make_shared<s_t>(false)));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == true);
        }
    }

    SUBCASE("and") {
        SUBCASE("true & true") {
            data = std::make_shared<s_t>(Sentence::And(
                std::make_shared<s_t>(true),
                std::make_shared<s_t>(true)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == true);
        }

        SUBCASE("true & false") {
            data = std::make_shared<s_t>(Sentence::And(
                std::make_shared<s_t>(true),
                std::make_shared<s_t>(false)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == false);
        }

        SUBCASE("false & true") {
            data = std::make_shared<s_t>(Sentence::And(
                std::make_shared<s_t>(false),
                std::make_shared<s_t>(true)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == false);
        }

        SUBCASE("false & false") {
            data = std::make_shared<s_t>(Sentence::And(
                std::make_shared<s_t>(false),
                std::make_shared<s_t>(false)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == false);
        }
    }

    SUBCASE("or") {
        SUBCASE("true | true") {
            data = std::make_shared<s_t>(Sentence::Or(
                std::make_shared<s_t>(true),
                std::make_shared<s_t>(true)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == true);
        }

        SUBCASE("true | false") {
            data = std::make_shared<s_t>(Sentence::Or(
                std::make_shared<s_t>(true),
                std::make_shared<s_t>(false)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == true);
        }

        SUBCASE("false | true") {
            data = std::make_shared<s_t>(Sentence::Or(
                std::make_shared<s_t>(false),
                std::make_shared<s_t>(true)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == true);
        }

        SUBCASE("false | false") {
            data = std::make_shared<s_t>(Sentence::Or(
                std::make_shared<s_t>(false),
                std::make_shared<s_t>(false)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == false);
        }
    }

    SUBCASE("imp") {
        SUBCASE("true => true") {
            data = std::make_shared<s_t>(Sentence::Imp(
                std::make_shared<s_t>(true),
                std::make_shared<s_t>(true)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == true);
        }

        SUBCASE("true => false") {
            data = std::make_shared<s_t>(Sentence::Imp(
                std::make_shared<s_t>(true),
                std::make_shared<s_t>(false)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == false);
        }

        SUBCASE("false => true") {
            data = std::make_shared<s_t>(Sentence::Imp(
                std::make_shared<s_t>(false),
                std::make_shared<s_t>(true)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == true);
        }

        SUBCASE("false => false") {
            data = std::make_shared<s_t>(Sentence::Imp(
                std::make_shared<s_t>(false),
                std::make_shared<s_t>(false)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == true);
        }
    }

    SUBCASE("iff") {
        SUBCASE("true => true") {
            data = std::make_shared<s_t>(Sentence::Iff(
                std::make_shared<s_t>(true),
                std::make_shared<s_t>(true)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == true);
        }

        SUBCASE("true => false") {
            data = std::make_shared<s_t>(Sentence::Iff(
                std::make_shared<s_t>(true),
                std::make_shared<s_t>(false)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == false);
        }

        SUBCASE("false => true") {
            data = std::make_shared<s_t>(Sentence::Iff(
                std::make_shared<s_t>(false),
                std::make_shared<s_t>(true)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == false);
        }

        SUBCASE("false => false") {
            data = std::make_shared<s_t>(Sentence::Iff(
                std::make_shared<s_t>(false),
                std::make_shared<s_t>(false)
            ));
            Sentence sentence(data, symbols);
            CHECK(sentence.truth() == true);
        }
    }
}

TEST_CASE("evaluate()")
{
    using namespace pimpl;
    using s_t = Sentence::sentence_t;

    Sentence::sentence_ptr_t data;
    std::unordered_map<std::string, Sentence::sentence_ptr_t> symbols;
    std::unordered_map<std::string, bool> symbol_values;

    // Tests for malformed inputs/sentence structure
    SUBCASE("empty sentence") {
        Sentence sentence;
        REQUIRE(sentence.evaluate(symbol_values) == std::nullopt);
    }
    SUBCASE("missing/mismatched symbol values") {
        data = std::make_shared<s_t>(Sentence::And(
            std::make_shared<s_t>("foo"),
            std::make_shared<s_t>("bar")
        ));
        symbols.insert({"foo", std::get<Sentence::And>(*data).left});
        symbols.insert({"bar", std::get<Sentence::And>(*data).right});
        Sentence sentence(data, symbols);

        REQUIRE_MESSAGE(sentence.evaluate(symbol_values) == std::nullopt,
                        "empty symbol_values");
        symbol_values.insert({"foo", false});
        REQUIRE_MESSAGE(sentence.evaluate(symbol_values) == std::nullopt,
                        "too few symbol_values");
        symbol_values.insert({"bar", true});
        symbol_values.insert({"baz", true});
        REQUIRE_MESSAGE(sentence.evaluate(symbol_values) == std::nullopt,
                        "too many symbol_values");
    }

    SUBCASE("unexpected monostate") {
        data = std::make_shared<s_t>(Sentence::Or(
            std::make_shared<s_t>(std::monostate()),
            std::make_shared<s_t>(true)
        ));
        Sentence sentence(data, symbols);
        REQUIRE(sentence.evaluate(symbol_values) == std::nullopt);
    }

    SUBCASE("unexpected symbol") {
        data = std::make_shared<s_t>(Sentence::Imp(
            std::make_shared<s_t>("foo"),
            std::make_shared<s_t>("bar")
        ));
        symbols.insert({"foo", std::get<Sentence::Imp>(*data).left});
        Sentence sentence(data, symbols);
        symbol_values.insert({"foo", true});
        REQUIRE(sentence.evaluate(symbol_values) == std::nullopt);
    }

    // Tests for successful evaluation
    SUBCASE("single true") {
        data = std::make_shared<s_t>(true);
        Sentence sentence(data, symbols);
        REQUIRE(sentence.evaluate(symbol_values) == true);
    }

    SUBCASE("single false") {
        data = std::make_shared<s_t>(false);
        Sentence sentence(data, symbols);
        REQUIRE(sentence.evaluate(symbol_values) == false);
    }

    SUBCASE("symbol-free tautology (F => T)") {
        data = std::make_shared<s_t>(Sentence::Imp(
            std::make_shared<s_t>(false),
            std::make_shared<s_t>(true)
        ));
        Sentence sentence(data, symbols);
        REQUIRE(sentence.evaluate(symbol_values) == true);
    }

    SUBCASE("single symbol") {
        data = std::make_shared<s_t>("foo");
        symbols.insert({"foo", data});
        Sentence sentence(data, symbols);
        symbol_values.insert({"foo", true});
        REQUIRE(sentence.evaluate(symbol_values) == true);
    }

    SUBCASE("symbol substitution tautology (T & T)") {
        data = std::make_shared<s_t>(Sentence::And(
            std::make_shared<s_t>("foo"),
            std::make_shared<s_t>("bar")
        ));
        symbols.insert({"foo", std::get<Sentence::And>(*data).left});
        symbols.insert({"bar", std::get<Sentence::And>(*data).right});
        Sentence sentence(data, symbols);
        symbol_values.insert({"foo", true});
        symbol_values.insert({"bar", true});
        REQUIRE(sentence.evaluate(symbol_values) == true);
    }
}
