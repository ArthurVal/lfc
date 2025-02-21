#include <string_view>

#include "gtest/gtest.h"
#include "lfc/utils/interger_sequence.hpp"
#include "lfc/utils/tuple.hpp"
#include "tests/overload.hpp"

using namespace std::literals::string_view_literals;

namespace lfc::utils {
namespace {

constexpr auto Add = [](auto&&... v) { return (... + v); };
constexpr auto Mul = [](auto&&... v) { return (... * v); };

TEST(TestTuple, Apply) {
  EXPECT_EQ(
      (1 + 5 + 4),
      (Apply<1, 2, 3>(Add, std::make_tuple(50, 1, 5, 4, 5452, "Coucou"))));

  EXPECT_EQ(50 + 1 + 5,
            (Apply(Add, std::make_tuple(50, 1, 5, 4, 5452, "Coucou"),
                   MakeIndexSequence<3>())));

  EXPECT_EQ((4 * 5452),
            (Apply(Mul, std::make_tuple(50, 1, 5, 4, 5452, "Coucou"),
                   MakeIndexSequence<2, 3>())));

  EXPECT_EQ((1 + 2 + 3), (Apply(Add, std::make_tuple(1, 2, 3))));
}

TEST(TestTuple, VisitTuple) {
  {
    int expected_v = -5;
    VisitTuples([&](int v) { EXPECT_EQ(v, expected_v++); },
                std::make_tuple(-5, -4, -3, -2, -1));
  }

  {
    int expected_v = -5;
    double expected_d = 5.;
    VisitTuples(
        [&](int v, double d) {
          EXPECT_EQ(v, expected_v++);
          EXPECT_EQ(d, expected_d);
          expected_d *= 2;
        },
        std::make_tuple(-5, -4, -3, -2, -1), std::make_tuple(5., 10.));
  }

  {
    using tests::Overload;
    VisitTuples(tests::Overload{
                    [](std::string_view str) { EXPECT_EQ(str, "Coucou"); },
                    [](int v) { EXPECT_EQ(v, 1); },
                    [](double v) { EXPECT_EQ(v, 3.14); },
                    [](auto) { FAIL(); },
                },
                std::make_tuple(1, 3.14, "Coucou"sv));
  }
}

template <class T>
struct Accumulator {
  T accumulated = 0;

  Accumulator& Add(T v) {
    accumulated += v;
    return *this;
  }
};

TEST(TestTuple, ReduceTuple) {
  EXPECT_EQ((5 + (1 + 1 + 1 + 1 + 1)),
            ReduceTuple(Add, std::make_tuple(1, 1, 1, 1, 1), 5));

  using tests::Overload;
  EXPECT_EQ((1 + 1 + "Coucou"sv.size() + 1 + 1),
            ReduceTuple<std::size_t>(
                Overload{
                    [](std::size_t init, std::string_view v) {
                      return init + v.size();
                    },
                    Add,
                },
                std::make_tuple(1, 1, "Coucou"sv, 1, 1)));

  // We can do some weird stuff ...
  auto acc = Accumulator<int>{3};

  EXPECT_EQ(ReduceTuple(&Accumulator<int>::Add,
                        std::make_tuple(5, 3, 0, 5, -100), acc)
                .accumulated,
            (3 + (5 + 3 + 0 + 5 - 100)));

  // init is passed by copy, therefore acc is not modified by default...
  EXPECT_EQ(acc.accumulated, 3);

  // ... But we can use std::ref
  ReduceTuple(&Accumulator<int>::Add, std::make_tuple(5, 3, 0, 5, -100),
              std::ref(acc));

  EXPECT_EQ(acc.accumulated, (3 + (5 + 3 + 0 + 5 - 100)));
}

TEST(TestTuple, TransformTuple) {
  EXPECT_EQ(
      std::make_tuple(3, 3, 3),
      TransformTuples(Add, std::make_tuple(1, 1, 1), std::make_tuple(2, 2, 2)));

  EXPECT_EQ(
      std::make_tuple(6, 7, 8),
      TransformTuples(Add, std::make_tuple(1, 2, 3), std::make_tuple(2, 2, 2),
                      std::make_tuple(3, 3, 3)));

  EXPECT_EQ(std::make_tuple(3, 3),
            TransformTuples(Add, std::make_tuple(1, 1, 1, 1, 1, 1, 1),
                            std::make_tuple(2, 2)));

  EXPECT_EQ(std::make_tuple(3),
            TransformTuples(Add, std::make_tuple(1),
                            std::make_tuple(2, 2, 2, 2, 2, 2)));

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
