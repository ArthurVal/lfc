#include <type_traits>

#include "gtest/gtest.h"
#include "lfc/pd.hpp"
#include "mocks/arithmetic.hpp"

namespace lfc {
namespace {

TEST(TestPD, Make) {
  static_assert(std::is_same_v<decltype(MakePD(std::declval<int>(),
                                               std::declval<char>())),
                               PD<int, char>>);

  // lvalue ref
  static_assert(std::is_same_v<decltype(MakePD(std::declval<int&>(),
                                               std::declval<char>())),
                               PD<int, char>>);

  static_assert(std::is_same_v<decltype(MakePD(std::declval<int>(),
                                               std::declval<char&>())),
                               PD<int, char>>);

  // const lvalue ref
  static_assert(std::is_same_v<decltype(MakePD(std::declval<const int&>(),
                                               std::declval<char>())),
                               PD<int, char>>);

  static_assert(std::is_same_v<decltype(MakePD(std::declval<int>(),
                                               std::declval<const char&>())),
                               PD<int, char>>);

  // Using reference_wrapper
  static_assert(
      std::is_same_v<decltype(MakePD(std::declval<int>(),
                                     std::ref(std::declval<char&>()))),
                     PD<int, char&>>);

  static_assert(std::is_same_v<decltype(MakePD(std::cref(std::declval<int&>()),
                                               std::declval<char>())),
                               PD<const int&, char>>);

  // rvalues
  static_assert(std::is_same_v<decltype(MakePD(std::declval<int&&>(),
                                               std::declval<char>())),
                               PD<int, char>>);

  static_assert(std::is_same_v<decltype(MakePD(int{}, std::declval<char&&>())),
                               PD<int, char>>);

  {
    auto pd = MakePD(1, 2);
    EXPECT_EQ(pd.kp, 1);
    EXPECT_EQ(pd.kd, 2);
  }

  {
    auto pd = MakePD(1, 2.);
    EXPECT_EQ(pd.kp, 1);
    EXPECT_DOUBLE_EQ(pd.kd, 2.);
  }

  {
    auto pd = MakePD(1, "Coucou");
    EXPECT_EQ(pd.kp, 1);
    EXPECT_STREQ(pd.kd, "Coucou");
  }

  {
    // lvalue are copied by default...
    constexpr auto initial_value = 2;

    auto i = initial_value;
    auto d = 0.728182;
    auto pd = MakePD(i, d);

    EXPECT_EQ(pd.kp, i);
    EXPECT_DOUBLE_EQ(pd.kd, d);

    i = 10;
    EXPECT_NE(pd.kp, 10);
    EXPECT_EQ(pd.kp, initial_value);
  }

  {
    // ... unles we use std::ref
    constexpr auto initial_value = 2;

    auto i = initial_value;
    auto d = 0.728182;
    auto pd = MakePD(std::ref(i), std::cref(d));

    EXPECT_EQ(pd.kp, i);
    EXPECT_DOUBLE_EQ(pd.kd, d);

    // Ref
    EXPECT_EQ(std::addressof(pd.kp), std::addressof(i));
    EXPECT_EQ(std::addressof(pd.kd), std::addressof(d));

    i = 10;
    EXPECT_EQ(pd.kp, 10);
    EXPECT_NE(pd.kp, initial_value);

    // Not using cref make it mutable from PD
    pd.kp = 20;
    EXPECT_EQ(i, 20);
  }
}

TEST(TestPD, Tie) {
  static_assert(std::is_same_v<decltype(TieAsPD(std::declval<int&>(),
                                                std::declval<int&>())),
                               PD<int&, int&>>);

  static_assert(std::is_same_v<decltype(TieAsPD(std::declval<int&>(),
                                                std::declval<const int&>())),
                               PD<int&, const int&>>);

  static_assert(std::is_same_v<decltype(TieAsPD(std::declval<const char&>(),
                                                std::declval<int&>())),
                               PD<const char&, int&>>);

  int a = 2;
  constexpr int b = 3;
  auto pd = TieAsPD(a, b);
  EXPECT_EQ(std::addressof(pd.kp), std::addressof(a));
  EXPECT_EQ(std::addressof(pd.kd), std::addressof(b));

  EXPECT_EQ(pd.kp, 2);
  EXPECT_EQ(pd.kd, 3);

  pd.kp = 4;
  EXPECT_EQ(a, 4);

  // pd.kd = 5; // -> Not possible
}

TEST(TestPD, Forward) {
  static_assert(std::is_same_v<decltype(ForwardAsPD(std::declval<int>(),
                                                    std::declval<char>())),
                               PD<int&&, char&&>>);

  // lvalue ref
  static_assert(std::is_same_v<decltype(ForwardAsPD(std::declval<int&>(),
                                                    std::declval<char>())),
                               PD<int&, char&&>>);

  static_assert(
      std::is_same_v<decltype(ForwardAsPD(int{}, std::declval<char&>())),
                     PD<int&&, char&>>);

  // const lvalue ref
  static_assert(std::is_same_v<decltype(ForwardAsPD(std::declval<const int&>(),
                                                    std::declval<char>())),
                               PD<const int&, char&&>>);

  static_assert(
      std::is_same_v<decltype(ForwardAsPD(int{}, std::declval<const char&>())),
                     PD<int&&, const char&>>);

  // Using reference_wrapper does nothing here
  static_assert(std::is_same_v<decltype(ForwardAsPD(
                                   int{}, std::ref(std::declval<char&>()))),
                               PD<int&&, std::reference_wrapper<char>&&>>);

  static_assert(
      std::is_same_v<decltype(ForwardAsPD(std::cref(std::declval<const int&>()),
                                          char{})),
                     PD<std::reference_wrapper<const int>&&, char&&>>);

  // rvalues
  static_assert(std::is_same_v<decltype(ForwardAsPD(std::declval<int&&>(),
                                                    std::declval<char>())),
                               PD<int&&, char&&>>);

  static_assert(std::is_same_v<decltype(ForwardAsPD(std::declval<int>(),
                                                    std::declval<char&&>())),
                               PD<int&&, char&&>>);
}

TEST(TestPD, Solve) {
  using testing::StrictMock;
  using tests::ArithmeticMock;

  auto kp = StrictMock<ArithmeticMock<int, int>>{};
  auto kd = StrictMock<ArithmeticMock<int, decltype(kp)&>>{};

  {
    using testing::Return;
    using testing::ReturnRef;

    testing::InSequence seq;

    // (kd * dx) -> kp
    EXPECT_CALL(kd, Multiplication(321))
        .Times(1)
        .WillOnce(ReturnRef(kp))
        .RetiresOnSaturation();

    // (kp * x) -> 456
    EXPECT_CALL(kp, Multiplication(123))
        .Times(1)
        .WillOnce(Return(456))
        .RetiresOnSaturation();

    // ((kp * x) + (kd * dx))
    // (  456    +    kp   )
    // -> 789
    EXPECT_CALL(kp, Addition(456))
        .Times(1)
        .WillOnce(Return(789))
        .RetiresOnSaturation();

    // Mocks can't be copied, we must pass by reference
    EXPECT_EQ(789, TieAsPD(kp, kd).Solve(123, 321));
  }

  {
    using testing::Throw;
    // Exceptions are correctly forwarded and not intercepted
    EXPECT_CALL(kd, Multiplication(321))
        .Times(1)
        .WillOnce(Throw(std::runtime_error{"Hi !"}))
        .RetiresOnSaturation();

    EXPECT_THROW(
        {
          auto _ = TieAsPD(kp, kd).Solve(123, 321);
          (void)_;
        },
        std::runtime_error);
  }
}
}  // namespace
}  // namespace lfc
