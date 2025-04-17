#pragma once

#include <ostream>
#include <string_view>

#include "gmock/gmock.h"

namespace tests {

// ArgSide //////////////////////////////////////////////////////////////////
enum class ArgSide {
  Left,
  Right,
};

constexpr auto ToString(ArgSide side) noexcept -> std::string_view {
  switch (side) {
    case ArgSide::Left:
      return "Left";
    case ArgSide::Right:
      return "Right";
  }

  return "";
}

inline auto operator<<(std::ostream& os, ArgSide side) -> std::ostream& {
  os << ToString(side);
  return os;
}

// Addition /////////////////////////////////////////////////////////////////
template <class T, class R = T>
struct MockAddition {
  MOCK_METHOD(R, Addition, (T&, ArgSide), ());
  MOCK_METHOD(R, Addition, (T&, ArgSide), (const));
  MOCK_METHOD(R, Addition, (const T&, ArgSide), ());
  MOCK_METHOD(R, Addition, (const T&, ArgSide), (const));

  constexpr auto operator+=(const T& value) & -> MockAddition& {
    Addition(value, ArgSide::Right);
    return *this;
  }

  friend constexpr auto operator+=(T& value, MockAddition& m) -> T& {
    if constexpr (std::is_assignable_v<T, R>) {
      value = m.Addition(value, ArgSide::Left);
    } else {
      m.Addition(value, ArgSide::Left);
    }
    return value;
  }

  friend constexpr auto operator+=(T& value, const MockAddition& m) -> T& {
    if constexpr (std::is_assignable_v<T, R>) {
      value = m.Addition(value, ArgSide::Left);
    } else {
      m.Addition(value, ArgSide::Left);
    }
    return value;
  }

  friend constexpr auto operator+(MockAddition& mock, T value) -> R {
    return mock.Addition(value, ArgSide::Right);
  }

  friend constexpr auto operator+(const MockAddition& mock, T value) -> R {
    return mock.Addition(value, ArgSide::Right);
  }

  friend constexpr auto operator+(T value, MockAddition& mock) -> R {
    return mock.Addition(value, ArgSide::Left);
  }

  friend constexpr auto operator+(T value, const MockAddition& mock) -> R {
    return mock.Addition(value, ArgSide::Left);
  }
};

// Substraction /////////////////////////////////////////////////////////////
template <class T, class R = T>
struct MockSubstraction {
  MOCK_METHOD(R, Substraction, (T&, ArgSide), ());
  MOCK_METHOD(R, Substraction, (T&, ArgSide), (const));
  MOCK_METHOD(R, Substraction, (const T&, ArgSide), ());
  MOCK_METHOD(R, Substraction, (const T&, ArgSide), (const));

  constexpr auto operator-=(const T& value) & -> MockSubstraction& {
    Substraction(value, ArgSide::Right);
    return *this;
  }

  friend constexpr auto operator-=(T& value, MockSubstraction& m) -> T& {
    if constexpr (std::is_assignable_v<T&, R>) {
      value = m.Substraction(value, ArgSide::Left);
    } else {
      m.Substraction(value, ArgSide::Left);
    }
    return value;
  }

  friend constexpr auto operator-=(T& value, const MockSubstraction& m) -> T& {
    if constexpr (std::is_assignable_v<T&, R>) {
      value = m.Substraction(value, ArgSide::Left);
    } else {
      m.Substraction(value, ArgSide::Left);
    }
    return value;
  }

  friend constexpr auto operator-(MockSubstraction& mock, const T& value) -> R {
    return mock.Substraction(value, ArgSide::Right);
  }

  friend constexpr auto operator-(const MockSubstraction& mock,
                                  const T& value) -> R {
    return mock.Substraction(value, ArgSide::Right);
  }

  friend constexpr auto operator-(const T& value, MockSubstraction& mock) -> R {
    return mock.Substraction(value, ArgSide::Left);
  }

  friend constexpr auto operator-(const T& value,
                                  const MockSubstraction& mock) -> R {
    return mock.Substraction(value, ArgSide::Left);
  }
};

// Multiplication ///////////////////////////////////////////////////////////
template <class T, class R = T>
struct MockMultiplication {
  MOCK_METHOD(R, Multiplication, (T&, ArgSide), ());
  MOCK_METHOD(R, Multiplication, (T&, ArgSide), (const));
  MOCK_METHOD(R, Multiplication, (const T&, ArgSide), ());
  MOCK_METHOD(R, Multiplication, (const T&, ArgSide), (const));

  constexpr auto operator*=(const T& value) & -> MockMultiplication& {
    Multiplication(value, ArgSide::Right);
    return *this;
  }

  friend constexpr auto operator*=(T& value, MockMultiplication& m) -> T& {
    if constexpr (std::is_assignable_v<T&, R>) {
      value = m.Multiplication(value, ArgSide::Left);
    } else {
      m.Multiplication(value, ArgSide::Left);
    }
    return value;
  }

  friend constexpr auto operator*=(T& value,
                                   const MockMultiplication& m) -> T& {
    if constexpr (std::is_assignable_v<T&, R>) {
      value = m.Multiplication(value, ArgSide::Left);
    } else {
      m.Multiplication(value, ArgSide::Left);
    }
    return value;
  }

  friend constexpr auto operator*(const MockMultiplication& mock,
                                  const T& value) -> R {
    return mock.Multiplication(value, ArgSide::Right);
  }

  friend constexpr auto operator*(MockMultiplication& mock,
                                  const T& value) -> R {
    return mock.Multiplication(value, ArgSide::Right);
  }

  friend constexpr auto operator*(const T& value,
                                  const MockMultiplication& mock) -> R {
    return mock.Multiplication(value, ArgSide::Left);
  }

  friend constexpr auto operator*(const T& value,
                                  MockMultiplication& mock) -> R {
    return mock.Multiplication(value, ArgSide::Left);
  }
};

// Division /////////////////////////////////////////////////////////////////
template <class T, class R = T>
struct MockDivision {
  MOCK_METHOD(R, Division, (T&, ArgSide), ());
  MOCK_METHOD(R, Division, (T&, ArgSide), (const));
  MOCK_METHOD(R, Division, (const T&, ArgSide), ());
  MOCK_METHOD(R, Division, (const T&, ArgSide), (const));

  constexpr auto operator/=(const T& value) & -> MockDivision& {
    Division(value, ArgSide::Right);
    return *this;
  }

  friend constexpr auto operator/=(T& value, MockDivision& m) -> T& {
    if constexpr (std::is_assignable_v<T&, R>) {
      value = m.Division(value, ArgSide::Left);
    } else {
      m.Division(value, ArgSide::Left);
    }
    return value;
  }

  friend constexpr auto operator/=(T& value, const MockDivision& m) -> T& {
    if constexpr (std::is_assignable_v<T&, R>) {
      value = m.Division(value, ArgSide::Left);
    } else {
      m.Division(value, ArgSide::Left);
    }
    return value;
  }

  friend constexpr auto operator/(const MockDivision& mock,
                                  const T& value) -> R {
    return mock.Division(value, ArgSide::Right);
  }

  friend constexpr auto operator/(MockDivision& mock, const T& value) -> R {
    return mock.Division(value, ArgSide::Right);
  }

  friend constexpr auto operator/(const T& value,
                                  const MockDivision& mock) -> R {
    return mock.Division(value, ArgSide::Left);
  }

  friend constexpr auto operator/(const T& value, MockDivision& mock) -> R {
    return mock.Division(value, ArgSide::Left);
  }
};

// Full arithmetic //////////////////////////////////////////////////////////
template <class T, class R = T>
struct MockArithmetic : public MockAddition<T, R>,
                        public MockSubstraction<T, R>,
                        public MockMultiplication<T, R>,
                        public MockDivision<T, R> {};

}  // namespace tests
