#pragma once

namespace tests {

/// Implementation of the overloads pattern
///(see https://en.cppreference.com/w/cpp/utility/variant/visit)
template <typename... Matchers>
struct Overload : Matchers... {
  using Matchers::operator()...;
};

/// CTAD (Not needed as of c++20)
template <typename... Matchers>
Overload(Matchers...) -> Overload<Matchers...>;

}  // namespace tests
