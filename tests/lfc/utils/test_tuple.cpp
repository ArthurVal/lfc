#include <string_view>

#include "gtest/gtest.h"
#include "lfc/utils/tuple.hpp"
#include "tests/overload.hpp"

using namespace std::literals::string_view_literals;

namespace lfc::utils {
namespace {

TEST(TestTuple, ForwardTuple) {
  int a = 2;
  double b = 3.;
  const short c = 4;
  {
    auto tpl = std::make_tuple(1, std::ref(a), std::cref(b), std::ref(c));

    static_assert(
        std::is_same_v<decltype(ForwardTuple(tpl)),
                       std::tuple<int&, int&, const double&, const short&>>);

    auto ref = ForwardTuple(tpl);
    EXPECT_EQ(ref, std::make_tuple(1, 2, 3., 4));

    std::get<0>(ref) = 5;
    EXPECT_EQ(std::get<0>(tpl), 5);

    std::get<1>(ref) = 6;
    EXPECT_EQ(std::get<1>(tpl), 6);
    EXPECT_EQ(a, 6);
  }

  {
    const auto tpl = std::make_tuple(1, std::ref(a), std::cref(b), std::ref(c));
    static_assert(std::is_same_v<
                  decltype(ForwardTuple(tpl)),
                  std::tuple<const int&, int&, const double&, const short&>>);
  }

  {
    static_assert(std::is_same_v<
                  decltype(ForwardTuple(std::forward_as_tuple(1, a, b, c))),
                  std::tuple<int&&, int&, double&, const short&>>);
  }
}

TEST(TestTuple, MakeSubTuple) {
  {
    // Work with lvalue tpl
    auto tpl = std::make_tuple(1, 2, "Coucou"sv);
    EXPECT_EQ((MakeSubTuple<0>(tpl)), std::make_tuple(1));
    EXPECT_EQ((MakeSubTuple<1, 2>(tpl)), std::make_tuple(2, "Coucou"sv));
  }

  {
    // Work with const lvalue tpl
    const auto tpl = std::make_tuple(1, 2, "Coucou"sv);
    EXPECT_EQ((MakeSubTuple<0>(tpl)), std::make_tuple(1));
    EXPECT_EQ((MakeSubTuple<1, 2>(tpl)), std::make_tuple(2, "Coucou"sv));

    auto sub_tuple = MakeSubTuple<0>(tpl);

    // Does a copy from tuple<T>
    EXPECT_NE(std::addressof(std::get<0>(sub_tuple)),
              std::addressof(std::get<0>(tpl)));
  }

  {
    // Work with rvalue tpl and forwarding
    EXPECT_EQ((MakeSubTuple<0>(std::forward_as_tuple(1, 2, "Coucou"sv))),
              std::make_tuple(1));
    EXPECT_EQ((MakeSubTuple<1, 2>(std::forward_as_tuple(1, 2, "Coucou"sv))),
              std::make_tuple(2, "Coucou"sv));

    int v = 3;
    const double d = 0.;
    static_assert(std::is_same_v<decltype(MakeSubTuple<0>(
                                     std::forward_as_tuple(1, v, d))),
                                 std::tuple<int&&>>);

    static_assert(std::is_same_v<decltype(MakeSubTuple<1>(
                                     std::forward_as_tuple(1, v, d))),
                                 std::tuple<int&>>);

    static_assert(std::is_same_v<decltype(MakeSubTuple<2>(
                                     std::forward_as_tuple(1, v, d))),
                                 std::tuple<const double&>>);
  }

  {
    // ref are kept and not copied
    auto i = 2;
    auto tpl = std::make_tuple(1, std::ref(i));
    auto sub_tuple = MakeSubTuple<1>(tpl);
    EXPECT_EQ(std::addressof(std::get<0>(sub_tuple)), std::addressof(i));
    EXPECT_EQ(std::get<0>(sub_tuple), 2);

    i = 3;
    EXPECT_EQ(std::get<0>(sub_tuple), 3);

    std::get<0>(sub_tuple) = 4;
    EXPECT_EQ(i, 4);
  }

  {
    // With index_sequence
    EXPECT_EQ(MakeSubTuple(std::make_index_sequence<2>{},
                           std::make_tuple(1, 2, 3, 4)),
              std::make_tuple(1, 2));
  }
}

TEST(TestTuple, SliceTuple) {
  EXPECT_EQ((SliceTuple<2, 1>(std::make_tuple(1, 2, 3, 4))),
            std::make_tuple(3));

  EXPECT_EQ((SliceTuple<1>(std::make_tuple(1, 2, 3, 4))),
            std::make_tuple(2, 3, 4));
}

TEST(TestTuple, ReduceTuple) {
  EXPECT_EQ(5, ReduceTuple(std::make_tuple(1, 1, 1, 1, 1), 0,
                           [](auto lhs, auto rhs) { return lhs + rhs; }));

  using tests::Overload;
  EXPECT_EQ(4 + 6, ReduceTuple<std::size_t>(
                       std::make_tuple(1, 1, "Coucou"sv, 1, 1), 0,
                       Overload{
                           [](std::size_t init, int v) { return init + v; },
                           [](std::size_t init, std::string_view v) {
                             return init + v.size();
                           },
                       }));
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

}  // namespace

}  // namespace lfc::utils
