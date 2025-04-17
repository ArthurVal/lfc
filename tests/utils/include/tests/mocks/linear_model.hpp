#pragma once

#include "gmock/gmock.h"

namespace tests {

template <class OffsetType>
struct MockIsValid {
  MOCK_METHOD(bool, IsValid, (const OffsetType&), ());
  friend constexpr auto IsValid(MockIsValid& m, const OffsetType& o) -> bool {
    return m.IsValid(o);
  }

  MOCK_METHOD(bool, IsValid, (const OffsetType&), (const));
  friend constexpr auto IsValid(const MockIsValid& m,
                                const OffsetType& o) -> bool {
    return m.IsValid(o);
  }

  MOCK_METHOD(bool, IsValidWithoutOffset, (), ());
  friend constexpr auto IsValid(MockIsValid& m) -> bool {
    return m.IsValidWithoutOffset();
  }

  MOCK_METHOD(bool, IsValidWithoutOffset, (), (const));
  friend constexpr auto IsValid(const MockIsValid& m) -> bool {
    return m.IsValidWithoutOffset();
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

}  // namespace tests
