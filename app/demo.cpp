#include <iostream>

#include "pimpl/sentence.hpp"
#include "pimpl/parsing.hpp"

int main(int argc, char** argv)
{
    std::cout << "Hello Logic" << std::endl;
    auto vec_opt = pimpl::parseString("foo & bar\nfi | fie | fo | fum");
    for (const auto& sentence : vec_opt.value()) {
        std::cout << sentence.stringify() << std::endl;
    }
}
