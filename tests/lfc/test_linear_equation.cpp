#include <type_traits>

#include "gtest/gtest.h"
#include "lfc/linear_equation.hpp"
#include "tests/mocks/arithmetic.hpp"

namespace lfc {
namespace {

TEST(TestLinearEquation, Ignored) {
  using testing::StrictMock;
  using tests::ArgSide;
  using tests::ArithmeticMock;

  EXPECT_EQ(3, (2 * Ignored) + 3);
  EXPECT_EQ(4 + 3, 4 + Ignored + 3);

  auto mock = StrictMock<ArithmeticMock<int, int>>{};
  {
    using testing::Return;

    EXPECT_CALL(mock, Addition(3, ArgSide::Right))
        .Times(2)
        .WillOnce(Return(-1))
        .WillOnce(Return(-2))
        .RetiresOnSaturation();

    EXPECT_EQ(-1, (mock + Ignored) + 3);
    EXPECT_EQ(-2, mock + (Ignored + 3));

    // Mock ignored
    EXPECT_EQ(3, (mock * Ignored) + 3);
    EXPECT_EQ(3, (Ignored * mock) + 3);
  }
}

TEST(TestLinearEquation, Make) {
  static_assert(std::is_same_v<decltype(MakeLinearEquation(
                                   std::declval<int>(), std::declval<char>())),
                               LinearEquation<int, char>>);

  // lvalue ref
  static_assert(std::is_same_v<decltype(MakeLinearEquation(
                                   std::declval<int&>(), std::declval<char>())),
                               LinearEquation<int, char>>);

  static_assert(std::is_same_v<decltype(MakeLinearEquation(
                                   std::declval<int>(), std::declval<char&>())),
                               LinearEquation<int, char>>);

  // const lvalue ref
  static_assert(
      std::is_same_v<decltype(MakeLinearEquation(std::declval<const int&>(),
                                                 std::declval<char>())),
                     LinearEquation<int, char>>);

  static_assert(
      std::is_same_v<decltype(MakeLinearEquation(std::declval<int>(),
                                                 std::declval<const char&>())),
                     LinearEquation<int, char>>);

  // Using reference_wrapper
  static_assert(
      std::is_same_v<decltype(MakeLinearEquation(
                         std::declval<int>(), std::ref(std::declval<char&>()))),
                     LinearEquation<int, char&>>);

  static_assert(std::is_same_v<decltype(MakeLinearEquation(
                                   std::cref(std::declval<int&>()),
                                   std::declval<char>())),
                               LinearEquation<const int&, char>>);

  // rvalues
  static_assert(
      std::is_same_v<decltype(MakeLinearEquation(std::declval<int&&>(),
                                                 std::declval<char>())),
                     LinearEquation<int, char>>);

  static_assert(std::is_same_v<decltype(MakeLinearEquation(
                                   int{}, std::declval<char&&>())),
                               LinearEquation<int, char>>);

  {
    auto eq = MakeLinearEquation(1, 2);
    EXPECT_EQ(eq.Size(), 2);
    EXPECT_EQ(eq.k<0>(), 1);
    EXPECT_EQ(eq.k<1>(), 2);
  }

  {
    auto eq = MakeLinearEquation(1, 2., 3);
    EXPECT_EQ(eq.Size(), 3);

    EXPECT_EQ(eq.k<0>(), 1);
    EXPECT_DOUBLE_EQ(eq.k<1>(), 2.);
    EXPECT_EQ(eq.k<2>(), 3);
  }

  {
    // No constraints on the type (will need to define arithmetic operators with
    // the X when calling .Solve() ...)
    auto eq = MakeLinearEquation(1, "Coucou");
    EXPECT_EQ(eq.Size(), 2);

    EXPECT_EQ(eq.k<0>(), 1);
    EXPECT_STREQ(eq.k<1>(), "Coucou");
  }

  {
    // lvalue are copied by default...
    constexpr auto initial_value = 2;

    auto i = initial_value;
    auto d = 0.728182;
    auto eq = MakeLinearEquation(i, d);

    EXPECT_EQ(eq.Size(), 2);

    EXPECT_EQ(eq.k<0>(), i);
    EXPECT_EQ(eq.k<0>(), initial_value);

    EXPECT_DOUBLE_EQ(eq.k<1>(), d);

    i = 10;
    EXPECT_NE(eq.k<0>(), i);
    EXPECT_EQ(eq.k<0>(), initial_value);
  }

  {
    // ... unles we use std::ref
    constexpr auto initial_value = 2;

    auto i = initial_value;
    auto d = 0.728182;
    auto eq = MakeLinearEquation(std::ref(i), std::cref(d));

    EXPECT_EQ(eq.Size(), 2);

    // Ref
    EXPECT_EQ(std::addressof(eq.k<0>()), std::addressof(i));
    EXPECT_EQ(std::addressof(eq.k<1>()), std::addressof(d));

    EXPECT_EQ(eq.k<0>(), i);
    EXPECT_EQ(eq.k<0>(), initial_value);

    EXPECT_DOUBLE_EQ(eq.k<1>(), d);

    i = 10;
    EXPECT_EQ(eq.k<0>(), 10);
    EXPECT_NE(eq.k<0>(), initial_value);

    // Not using cref make it mutable from PD
    eq.k<0>() = 20;
    EXPECT_EQ(i, 20);

    // eq.k<1>() = 0.; // -> Doesn't compile
  }
}

TEST(TestLinearEquation, Tie) {
  static_assert(std::is_same_v<decltype(TieAsLinearEquation(
                                   std::declval<int&>(), std::declval<int&>(),
                                   std::declval<char&>())),
                               LinearEquation<int&, int&, char&>>);

  static_assert(
      std::is_same_v<decltype(TieAsLinearEquation(std::declval<int&>(),
                                                  std::declval<const int&>())),
                     LinearEquation<int&, const int&>>);

  static_assert(
      std::is_same_v<decltype(TieAsLinearEquation(std::declval<const char&>(),
                                                  std::declval<int&>())),
                     LinearEquation<const char&, int&>>);

  int k0_ref = 0;
  constexpr int k1_ref = 1;
  const short k2_ref = 2;
  char k3_ref = 3;

  auto tied_eq = TieAsLinearEquation(k0_ref, k1_ref, k2_ref, k3_ref);
  EXPECT_EQ(std::addressof(tied_eq.k<0>()), std::addressof(k0_ref));
  EXPECT_EQ(std::addressof(tied_eq.k<1>()), std::addressof(k1_ref));
  EXPECT_EQ(std::addressof(tied_eq.k<2>()), std::addressof(k2_ref));
  EXPECT_EQ(std::addressof(tied_eq.k<3>()), std::addressof(k3_ref));
}

TEST(TestLinearEquation, Forward) {
  static_assert(std::is_same_v<decltype(ForwardAsLinearEquation(
                                   std::declval<int>(), std::declval<char>())),
                               LinearEquation<int&&, char&&>>);

  // lvalue ref
  static_assert(std::is_same_v<decltype(ForwardAsLinearEquation(
                                   std::declval<int&>(), std::declval<char>())),
                               LinearEquation<int&, char&&>>);

  static_assert(std::is_same_v<decltype(ForwardAsLinearEquation(
                                   int{}, std::declval<char&>())),
                               LinearEquation<int&&, char&>>);

  // const lvalue ref
  static_assert(
      std::is_same_v<decltype(ForwardAsLinearEquation(
                         std::declval<const int&>(), std::declval<char>())),
                     LinearEquation<const int&, char&&>>);

  static_assert(std::is_same_v<decltype(ForwardAsLinearEquation(
                                   int{}, std::declval<const char&>())),
                               LinearEquation<int&&, const char&>>);

  // Using reference_wrapper does nothing here
  static_assert(
      std::is_same_v<decltype(ForwardAsLinearEquation(
                         int{}, std::ref(std::declval<char&>()))),
                     LinearEquation<int&&, std::reference_wrapper<char>&&>>);

  static_assert(std::is_same_v<
                decltype(ForwardAsLinearEquation(
                    std::cref(std::declval<const int&>()), char{})),
                LinearEquation<std::reference_wrapper<const int>&&, char&&>>);

  // rvalues
  static_assert(
      std::is_same_v<decltype(ForwardAsLinearEquation(std::declval<int&&>(),
                                                      std::declval<char>())),
                     LinearEquation<int&&, char&&>>);

  static_assert(
      std::is_same_v<decltype(ForwardAsLinearEquation(std::declval<int>(),
                                                      std::declval<char&&>())),
                     LinearEquation<int&&, char&&>>);
}

TEST(TestLinearEquation, Solve) {
  using testing::StrictMock;
  using tests::ArgSide;
  using tests::ArithmeticMock;

  auto k0 = StrictMock<ArithmeticMock<int, int>>{};
  auto k1 = StrictMock<ArithmeticMock<decltype(k0)&, int>>{};

  // Mocks can't be copied, we must pass by reference -> Tie
  auto eq = TieAsLinearEquation(k0, k1);

  int x0 = 123;
  int x1 = 321;

  using testing::Return;
  using testing::ReturnRef;

  testing::InSequence seq;

  // (k1 * x1) -> k0
  EXPECT_CALL(eq.k<1>(), Multiplication(x1, ArgSide::Right))
      .Times(1)
      .WillOnce(ReturnRef(k0))
      .RetiresOnSaturation();

  // (k0 * x0) -> 456
  EXPECT_CALL(eq.k<0>(), Multiplication(x0, ArgSide::Right))
      .Times(1)
      .WillOnce(Return(456))
      .RetiresOnSaturation();

  // ((k0 * x0) + (k1 * x1))
  // (  456    +     k0    ) -> 789
  EXPECT_CALL(eq.k<0>(), Addition(456, ArgSide::Left))
      .Times(1)
      .WillOnce(Return(789))
      .RetiresOnSaturation();

  EXPECT_EQ(789, eq.Solve(x0, x1));
}

TEST(TestLinearEquation, SolveReverseMultiplication) {
  using testing::StrictMock;
  using tests::ArgSide;
  using tests::ArithmeticMock;

  auto k0 = StrictMock<ArithmeticMock<int, int>>{};
  auto k1 = StrictMock<ArithmeticMock<decltype(k0)&, int>>{};

  // Mocks can't be copied, we must pass by reference -> Tie
  auto eq = TieAsLinearEquation(k0, k1);

  int x0 = 123;
  int x1 = 321;

  // We can reverse multiplication side using Solve<false>()
  using testing::Return;
  using testing::ReturnRef;

  testing::InSequence seq;
  // (x1 * k1) -> k0
  EXPECT_CALL(eq.k<1>(), Multiplication(x1, ArgSide::Left))
      .Times(1)
      .WillOnce(ReturnRef(k0))
      .RetiresOnSaturation();

  // (x0 * k0) -> 456
  EXPECT_CALL(eq.k<0>(), Multiplication(x0, ArgSide::Left))
      .Times(1)
      .WillOnce(Return(456))
      .RetiresOnSaturation();

  // Addition are unchanged
  // ((x0 * k0) + (x1 * k1))
  // (  456    +     k0    ) -> 789
  EXPECT_CALL(eq.k<0>(), Addition(456, ArgSide::Left))
      .Times(1)
      .WillOnce(Return(-1))
      .RetiresOnSaturation();

  EXPECT_EQ(-1, eq.Solve<false>(x0, x1));
}

TEST(TestLinearEquation, SolveIgnoredArgs) {
  using testing::StrictMock;
  using tests::ArgSide;
  using tests::ArithmeticMock;

  auto k0 = StrictMock<ArithmeticMock<int, int>>{};
  auto k1 = StrictMock<ArithmeticMock<int, int>>{};
  auto k2 = StrictMock<ArithmeticMock<int, int>>{};

  // Mocks can't be copied, we must pass by reference -> Tie
  auto eq = TieAsLinearEquation(k0, k1, k2);

  int x1 = 123;

  // We can use ignore to remove some x
  using testing::Return;

  // (k1 * x1) -> -1
  EXPECT_CALL(eq.k<1>(), Multiplication(x1, ArgSide::Right))
      .Times(1)
      .WillOnce(Return(111))
      .RetiresOnSaturation();

  // Ignored disable x2, hence k2 not used
  EXPECT_EQ(111, eq.Solve(Ignored, x1, Ignored));
}

TEST(TestLinearEquation, SolveIgnoredCoeffs) {
  using testing::StrictMock;
  using tests::ArgSide;
  using tests::ArithmeticMock;

  auto k1 = StrictMock<ArithmeticMock<int, int>>{};

  // Mocks can't be copied, we must pass by reference -> Tie
  auto eq = MakeLinearEquation(Ignored, std::ref(k1), Ignored);

  int x0 = 0;
  int x1 = 123;
  int x2 = 321;

  // We can use ignore to remove some x
  using testing::Return;

  // (k1 * x1) -> -100
  EXPECT_CALL(eq.k<1>(), Multiplication(x1, ArgSide::Right))
      .Times(1)
      .WillOnce(Return(-100))
      .RetiresOnSaturation();

  EXPECT_EQ(-100, eq.Solve(x0, x1, x2));
}

}  // namespace
}  // namespace lfc
