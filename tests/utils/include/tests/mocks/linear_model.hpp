#pragma once

#include "arithmetic.hpp"
#include "gmock/gmock.h"

namespace tests {

template <class OffsetType = void>
struct MockIsValid {
  MOCK_METHOD(bool, IsValid, (const OffsetType&), ());
  friend inline auto IsValid(MockIsValid& m, const OffsetType& o) -> bool {
    return m.IsValid(o);
  }

  MOCK_METHOD(bool, IsValid, (const OffsetType&), (const));
  friend constexpr auto IsValid(const MockIsValid& m,
                                const OffsetType& o) -> bool {
    return m.IsValid(o);
  }

  MOCK_METHOD(bool, IsValid, (), ());
  friend inline auto IsValid(MockIsValid& m) -> bool { return m.IsValid(); }

  MOCK_METHOD(bool, IsValid, (), (const));
  friend inline auto IsValid(const MockIsValid& m) -> bool {
    return m.IsValid();
  }
};

template <>
struct MockIsValid<void> {
  MOCK_METHOD(bool, IsValid, (), ());
  friend inline auto IsValid(MockIsValid& m) -> bool { return m.IsValid(); }

  MOCK_METHOD(bool, IsValid, (), (const));
  friend inline auto IsValid(const MockIsValid& m) -> bool {
    return m.IsValid();
  }
};

template <class T>
struct MockAccepts {
  MOCK_METHOD(bool, Accepts, (const T&), ());
  friend constexpr auto Accepts(MockAccepts& m, const T& t) -> bool {
    return m.Accepts(t);
  }

  MOCK_METHOD(bool, Accepts, (const T&), (const));
  friend constexpr auto Accepts(const MockAccepts& m, const T& t) -> bool {
    return m.Accepts(t);
  }
};

template <class T, class OffsetType = void>
struct MockCoeffs : public MockIsValid<OffsetType>,
                    public MockAccepts<T>,
                    public MockMultiplication<T> {};

}  // namespace tests
