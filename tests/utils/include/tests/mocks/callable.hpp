#pragma once

#include "gmock/gmock.h"

namespace tests {

template <class R, class... Args>
struct CallableMock {
  using return_t = R;
  using arg_t = std::tuple<Args...>;

  MOCK_METHOD(return_t, Call, (std::tuple<Args...>), (const));
  constexpr auto operator()(Args... args) const -> return_t {
    return Call(std::make_tuple(std::move(args)...));
  }
};

template <class R, class Arg>
struct CallableMock<R, Arg> {
  using return_t = R;
  using arg_t = Arg;

  MOCK_METHOD(return_t, Call, (Arg), (const));
  constexpr auto operator()(Arg arg) const -> return_t {
    return Call(std::move(arg));
  }
};

}  // namespace tests
