#include <string_view>

#include "gtest/gtest.h"
#include "lfc/utils/interger_sequence.hpp"
#include "lfc/utils/tuple.hpp"
#include "tests/overload.hpp"

using namespace std::literals::string_view_literals;

namespace lfc::utils {
namespace {

constexpr auto DoSum = [](auto&&... v) { return (... + v); };

TEST(TestTuple, ApplyOn) {
  EXPECT_EQ(
      1 + 5 + 4,
      (ApplyOn<1, 2, 3>(DoSum, std::make_tuple(50, 1, 5, 4, 5452, "Coucou"))));

  EXPECT_EQ(50 + 1 + 5,
            (ApplyOn(std::make_index_sequence<3>{}, DoSum,
                     std::make_tuple(50, 1, 5, 4, 5452, "Coucou"))));

  EXPECT_EQ((4 + 5452),
            (ApplyOn(SliceOfIndex<3, 2>(), DoSum,
                     std::make_tuple(50, 1, 5, 4, 5452, "Coucou"))));
}

TEST(TestTuple, ReduceTuple) {
  EXPECT_EQ(10, ReduceTuple([](auto lhs, auto rhs) { return lhs + rhs; },
                            std::make_tuple(1, 1, 1, 1, 1), 5));

  using tests::Overload;
  EXPECT_EQ(4 + 6, ReduceTuple<std::size_t>(
                       Overload{
                           [](std::size_t init, int v) { return init + v; },
                           [](std::size_t init, std::string_view v) {
                             return init + v.size();
                           },
                       },
                       std::make_tuple(1, 1, "Coucou"sv, 1, 1)));
}

TEST(TestTuple, TransformTuple) {
  EXPECT_EQ(
      std::make_tuple(3, 3, 3),
      TransformTuples([](auto lhs, auto rhs) { return lhs + rhs; },
                      std::make_tuple(1, 1, 1), std::make_tuple(2, 2, 2)));

  EXPECT_EQ(std::make_tuple(6, 7, 8),
            TransformTuples([](auto... v) { return (... + v); },
                            std::make_tuple(1, 2, 3), std::make_tuple(2, 2, 2),
                            std::make_tuple(3, 3, 3)));

  EXPECT_EQ(std::make_tuple(3, 3),
            TransformTuples([](auto lhs, auto rhs) { return lhs + rhs; },
                            std::make_tuple(1, 1, 1, 1, 1, 1, 1),
                            std::make_tuple(2, 2)));

  EXPECT_EQ(
      std::make_tuple(3),
      TransformTuples([](auto lhs, auto rhs) { return lhs + rhs; },
                      std::make_tuple(1), std::make_tuple(2, 2, 2, 2, 2, 2)));

  using tests::Overload;
  EXPECT_EQ(std::make_tuple("Foo Bar"sv, 0, 3.14),
            TransformTuples(Overload{
                                [](std::string_view lhs, std::string_view rhs) {
                                  std::string out;
                                  out.reserve(lhs.size() + 1 + rhs.size() + 1);

                                  out.append(lhs);
                                  out.append(" ");
                                  out.append(rhs);
                                  return out;
                                },
                                [](int lhs, int rhs) { return lhs - rhs; },
                                [](auto, auto) { return 3.14; },
                            },
                            std::make_tuple("Foo"sv, 1, ""),
                            std::make_tuple("Bar"sv, 1, 0.)));
}

TEST(TestTuple, TransformReduceTuples) {
  constexpr auto Add = [](auto... v) { return (... + v); };
  constexpr auto Mul = [](auto... v) { return (... * v); };

  EXPECT_EQ((10 + (2 * 1) + (4 * 3)),
            TransformReduceTuples(10, Add, Mul, std::make_tuple(2, 4, 6),
                                  std::make_tuple(1, 3)));

  EXPECT_EQ(
      (20 * (2 + 1 - 1) * (4 + 3 - 5)),
      TransformReduceTuples(20, Mul, Add, std::make_tuple(2, 4, 8),
                            std::make_tuple(1, 3), std::make_tuple(-1, -5)));
}

}  // namespace

}  // namespace lfc::utils
