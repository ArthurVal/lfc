#pragma once

#include "gmock/gmock.h"

namespace tests {

template <class R, class... Args>
struct CallableMock {
  using return_t = R;
  using arg_t = std::tuple<Args...>;

  MOCK_METHOD(return_t, CallOperator, (const arg_t&), (const));

  template <class... T>
  constexpr auto operator()(T&&... v) const -> return_t {
    return CallOperator(arg_t{std::forward<T>(v)...});
  }
};

}  // namespace tests
