#include <type_traits>

#include "gtest/gtest.h"
#include "lfc/linear_equation.hpp"
#include "tests/mocks/arithmetic.hpp"
#include "tests/mocks/callable.hpp"

namespace lfc {
namespace {

TEST(TestLinearEquation, Ignored) {
  using testing::StrictMock;
  using tests::ArgSide;
  using tests::ArithmeticMock;

  EXPECT_EQ(3, (2 * Ignored{}) + 3);
  EXPECT_EQ(4 + 3, 4 + Ignored{} + 3);

  auto mock = StrictMock<ArithmeticMock<int, int>>{};
  {
    using testing::Return;

    EXPECT_CALL(mock, Addition(3, ArgSide::Right))
        .Times(2)
        .WillOnce(Return(-1))
        .WillOnce(Return(-2))
        .RetiresOnSaturation();

    EXPECT_EQ(-1, (mock + Ignored{}) + 3);
    EXPECT_EQ(-2, mock + (Ignored{} + 3));

    // Mock ignored
    EXPECT_EQ(3, (mock * Ignored{}) + 3);
    EXPECT_EQ(3, (Ignored{} * mock) + 3);
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

    EXPECT_EQ(AsTuple(eq), std::make_tuple(1, 2));
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

TEST(TestLinearEquation, ApplyToCoefs) {
  using testing::StrictMock;
  using tests::CallableMock;

  auto mock = StrictMock<CallableMock<int, int, double, const char*>>{};
  using testing::Return;

  EXPECT_CALL(mock, Call(std::make_tuple(1, 3.14, "Coucou")))
      .Times(1)
      .WillRepeatedly(Return(-1))
      .RetiresOnSaturation();

  EXPECT_EQ(ForwardAsLinearEquation(1, 3.14, "Coucou").ApplyToCoeffs(mock), -1);

  {
    // More realistic example without mocks, for documentation purposes
    auto eq = MakeLinearEquation(1, 1, 1, 1, 1);
    EXPECT_EQ(eq.ApplyToCoeffs([](auto&... k) -> std::string {
      ((++k), ...);
      return "Coucou";
    }),
              "Coucou");

    EXPECT_EQ(AsTuple(eq), std::make_tuple(2, 2, 2, 2, 2));

    // Checks that all coeffs are equals to 2
    EXPECT_TRUE(
        eq.ApplyToCoeffs([](auto... k) -> bool { return ((k == 2) and ...); }));

    // Sum all coeffs
    EXPECT_EQ(10, eq.ApplyToCoeffs([](auto... k) { return (k + ...); }));
  }
}

TEST(TestLinearEquation, ForEachCoefsDo) {
  using testing::StrictMock;
  using tests::CallableMock;

  {
    auto mock = StrictMock<CallableMock<void, int>>{};
    using testing::Return;
    EXPECT_CALL(mock, Call(1))
        .Times(4)
        .WillRepeatedly(Return())
        .RetiresOnSaturation();
    ForwardAsLinearEquation(1, 1, 1, 1).ForEachCoeffsDo(mock);
  }

  {
    auto mock = StrictMock<CallableMock<void, const char*>>{};
    using testing::Return;

    EXPECT_CALL(mock, Call("Coucou"))
        .Times(1)
        .WillRepeatedly(Return())
        .RetiresOnSaturation();

    ForwardAsLinearEquation(1, 1, "Coucou", 1).ForEachCoeffsDo(mock);
  }

  {
    auto mock = StrictMock<CallableMock<void, const char*, std::size_t>>{};
    using testing::Return;

    EXPECT_CALL(mock, Call(std::make_tuple("a", 1ul)))
        .Times(1)
        .WillRepeatedly(Return())
        .RetiresOnSaturation();

    EXPECT_CALL(mock, Call(std::make_tuple("Coucou", 3ul)))
        .Times(1)
        .WillRepeatedly(Return())
        .RetiresOnSaturation();

    ForwardAsLinearEquation(1, "a", 1, "Coucou", 1).ForEachCoeffsDo(mock);
  }
}

TEST(TestLinearEquation, Solve) {
  using testing::StrictMock;
  using tests::ArgSide;
  using tests::ArithmeticMock;

  auto k0 = StrictMock<ArithmeticMock<int, int>>{};
  auto k1 = StrictMock<ArithmeticMock<int, int>>{};
  auto k2 = StrictMock<ArithmeticMock<decltype(k1)&, int>>{};

  int x1 = 123;
  int x2 = 321;

  {
    using testing::Return;
    using testing::ReturnRef;

    testing::InSequence seq;

    // (k2 * x2) -> k1
    EXPECT_CALL(k2, Multiplication(x2, ArgSide::Right))
        .Times(1)
        .WillOnce(ReturnRef(k1))
        .RetiresOnSaturation();

    // (k1 * x1) -> 456
    EXPECT_CALL(k1, Multiplication(x1, ArgSide::Right))
        .Times(1)
        .WillOnce(Return(456))
        .RetiresOnSaturation();

    // ((k1 * x1) + (k2 * x2))
    // (  456    +     k1    ) -> 789
    EXPECT_CALL(k1, Addition(456, ArgSide::Left))
        .Times(1)
        .WillOnce(Return(789))
        .RetiresOnSaturation();

    // k0 + ((k1 * x1) + (k2 * x2))
    // k0 + (         789         ) -> -1
    EXPECT_CALL(k0, Addition(789, ArgSide::Right))
        .Times(1)
        .WillOnce(Return(-1))
        .RetiresOnSaturation();

    // Mocks can't be copied, we must pass by reference
    EXPECT_EQ(-1, TieAsLinearEquation(k0, k1, k2).Solve(x1, x2));
  }

  {
    using testing::Throw;
    // Exceptions are correctly forwarded and not intercepted
    EXPECT_CALL(k2, Multiplication(321, ArgSide::Right))
        .Times(1)
        .WillOnce(Throw(std::runtime_error{"Hi !"}))
        .RetiresOnSaturation();

    EXPECT_THROW(
        {
          auto _ = TieAsLinearEquation(k0, k1, k2).Solve(123, 321);
          (void)_;
        },
        std::runtime_error);
  }

  {
    using testing::Return;

    // (x1 * k1) -> -1
    EXPECT_CALL(k1, Multiplication(x1, ArgSide::Left))
        .Times(1)
        .WillOnce(Return(-1))
        .RetiresOnSaturation();

    // Solve<false>() transform (k1 * x1) to (x1 * k1)
    // Ignored disable k0 and forwards only (x1 * k1)
    EXPECT_EQ(-1,
              MakeLinearEquation(Ignored{}, std::cref(k1)).Solve<false>(x1));

    // Ignored disable (k1 * x1) and forward only k0
    EXPECT_EQ(1, MakeLinearEquation(1, std::cref(k1)).Solve(Ignored{}));
  }
}
}  // namespace
}  // namespace lfc
