#pragma once

#include <type_traits>

#include "gmock/gmock.h"

namespace tests {

template <class Arg, class R = void>
struct AddMock {
  MOCK_METHOD(R, Addition, (const Arg&), (const));

  friend constexpr auto operator+(const AddMock& mock, const Arg& v)
      -> std::conditional_t<std::is_same_v<R, void>, const AddMock&, R> {
    if constexpr (std::is_same_v<R, void>) {
      mock.Addition(v);
      return mock;
    } else {
      return mock.Addition(v);
    }
  }

  friend constexpr auto operator+(const Arg& v, const AddMock& mock) {
    return mock + v;
  }
};

template <class Arg, class R = void>
struct SubMock {
  MOCK_METHOD(R, Substraction, (const Arg&), (const));
  friend constexpr auto operator-(const SubMock& mock, const Arg& v)
      -> std::conditional_t<std::is_same_v<R, void>, const SubMock&, R> {
    if constexpr (std::is_same_v<R, void>) {
      mock.Substraction(v);
      return mock;
    } else {
      return mock.Substraction(v);
    }
  }

  friend constexpr auto operator-(const Arg& v, const SubMock& mock) {
    return mock - v;
  }
};

template <class Arg, class R = void>
struct MulMock {
  MOCK_METHOD(R, Multiplication, (const Arg&), (const));
  friend constexpr auto operator*(const MulMock& mock, const Arg& v)
      -> std::conditional_t<std::is_same_v<R, void>, const MulMock&, R> {
    if constexpr (std::is_same_v<R, void>) {
      mock.Multiplication(v);
      return mock;
    } else {
      return mock.Multiplication(v);
    }
  }

  friend constexpr auto operator*(const Arg& v, const MulMock& mock) {
    return mock * v;
  }
};

template <class Arg, class R = void>
struct DivMock {
  MOCK_METHOD(R, Division, (const Arg&), (const));
  friend constexpr auto operator/(const DivMock& mock, const Arg& v)
      -> std::conditional_t<std::is_same_v<R, void>, const DivMock&, R> {
    if constexpr (std::is_same_v<R, void>) {
      mock.Division(v);
      return mock;
    } else {
      return mock.Division(v);
    }
  }

  friend constexpr auto operator/(const Arg& v, const DivMock& mock) {
    return mock / v;
  }
};

template <class Arg, class R = void>
struct ArithmeticMock : AddMock<Arg, R>,
                        SubMock<Arg, R>,
                        MulMock<Arg, R>,
                        DivMock<Arg, R> {};

}  // namespace tests
