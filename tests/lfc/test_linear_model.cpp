#include <type_traits>

#include "gtest/gtest.h"
#include "lfc/linear_model.hpp"
#include "tests/mocks/arithmetic.hpp"

namespace lfc {
namespace {

TEST(TestLinearModel, Make) {
  static_assert(std::is_same_v<decltype(MakeLinearModel(std::declval<int>())),
                               LinearModel<int, void>>);

  static_assert(std::is_same_v<decltype(MakeLinearModel(std::declval<int>(),
                                                        std::declval<char>())),
                               LinearModel<int, char>>);

  // lvalue ref
  static_assert(std::is_same_v<decltype(MakeLinearModel(std::declval<int&>(),
                                                        std::declval<char>())),
                               LinearModel<int, char>>);

  static_assert(std::is_same_v<decltype(MakeLinearModel(std::declval<int>(),
                                                        std::declval<char&>())),
                               LinearModel<int, char>>);

  // const lvalue ref
  static_assert(
      std::is_same_v<decltype(MakeLinearModel(std::declval<const int&>(),
                                              std::declval<char>())),
                     LinearModel<int, char>>);

  static_assert(
      std::is_same_v<decltype(MakeLinearModel(std::declval<int>(),
                                              std::declval<const char&>())),
                     LinearModel<int, char>>);

  // Using reference_wrapper
  static_assert(
      std::is_same_v<decltype(MakeLinearModel(std::declval<int>(),
                                              std::ref(std::declval<char&>()))),
                     LinearModel<int, char&>>);

  static_assert(
      std::is_same_v<decltype(MakeLinearModel(std::cref(std::declval<int&>()),
                                              std::declval<char>())),
                     LinearModel<const int&, char>>);

  // rvalues
  static_assert(std::is_same_v<decltype(MakeLinearModel(std::declval<int&&>(),
                                                        std::declval<char>())),
                               LinearModel<int, char>>);

  static_assert(
      std::is_same_v<decltype(MakeLinearModel(int{}, std::declval<char&&>())),
                     LinearModel<int, char>>);

  {
    auto model = MakeLinearModel(1);
    EXPECT_EQ(sizeof(model), sizeof(int));
    EXPECT_EQ(model.coeffs, 1);
    // EXPECT_EQ(model.offset, 2); // offset doesn't exists
  }

  {
    auto model = MakeLinearModel(1, 2);
    EXPECT_EQ(sizeof(model), 2 * sizeof(int));
    EXPECT_EQ(model.coeffs, 1);
    EXPECT_EQ(model.offset, 2);
  }

  {
    // No constraints on the type (will need to define arithmetic operators with
    // the X when calling Solve() ...)
    auto model = MakeLinearModel(1, "Coucou");
    EXPECT_EQ(model.coeffs, 1);
    EXPECT_STREQ(model.offset, "Coucou");
  }

  {
    // lvalue are copied by default...
    constexpr auto initial_value = 2;

    auto i = initial_value;
    auto d = 0.728182;
    auto model = MakeLinearModel(i, d);
    EXPECT_EQ(model.coeffs, i);
    EXPECT_EQ(model.coeffs, initial_value);

    EXPECT_DOUBLE_EQ(model.offset, d);

    i = 10;
    EXPECT_NE(model.coeffs, i);
    EXPECT_EQ(model.coeffs, initial_value);
  }

  {
    // ... unles we use std::ref
    constexpr auto initial_value = 2;

    auto i = initial_value;
    auto d = 0.728182;
    auto model = MakeLinearModel(std::ref(i), std::cref(d));

    // Ref
    EXPECT_EQ(std::addressof(model.coeffs), std::addressof(i));
    EXPECT_EQ(std::addressof(model.offset), std::addressof(d));

    EXPECT_EQ(model.coeffs, i);
    EXPECT_EQ(model.coeffs, initial_value);

    EXPECT_DOUBLE_EQ(model.offset, d);

    i = 10;
    EXPECT_EQ(model.coeffs, 10);
    EXPECT_NE(model.coeffs, initial_value);

    // Not using cref make it mutable from PD
    model.coeffs = 20;
    EXPECT_EQ(i, 20);

    // model.offset = 0.; // -> Doesn't compile
  }
}

TEST(TestLinearModel, Tie) {
  static_assert(
      std::is_same_v<decltype(TieAsLinearModel(std::declval<int&>(),
                                               std::declval<char&>())),
                     LinearModel<int&, char&>>);

  static_assert(std::is_same_v<decltype(TieAsLinearModel(std::declval<int&>())),
                               LinearModel<int&, void>>);

  static_assert(
      std::is_same_v<decltype(TieAsLinearModel(std::declval<int&>(),
                                               std::declval<const int&>())),
                     LinearModel<int&, const int&>>);

  static_assert(
      std::is_same_v<decltype(TieAsLinearModel(std::declval<const char&>(),
                                               std::declval<int&>())),
                     LinearModel<const char&, int&>>);

  {
    int k0_ref = 0;
    auto model = TieAsLinearModel(k0_ref);
    EXPECT_EQ(std::addressof(model.coeffs), std::addressof(k0_ref));
  }

  {
    int k0_ref = 0;
    constexpr int k1_ref = 1;
    auto model = TieAsLinearModel(k0_ref, k1_ref);
    EXPECT_EQ(std::addressof(model.coeffs), std::addressof(k0_ref));
    EXPECT_EQ(std::addressof(model.offset), std::addressof(k1_ref));
  }

  {
    const short k0_ref = 2;
    char k1_ref = 3;

    auto model = TieAsLinearModel(k0_ref, k1_ref);
    EXPECT_EQ(std::addressof(model.coeffs), std::addressof(k0_ref));
    EXPECT_EQ(std::addressof(model.offset), std::addressof(k1_ref));
  }
}

TEST(TestLinearModel, Forward) {
  static_assert(
      std::is_same_v<decltype(ForwardAsLinearModel(std::declval<int>())),
                     LinearModel<int&&, void>>);

  static_assert(std::is_same_v<decltype(ForwardAsLinearModel(
                                   std::declval<int>(), std::declval<char>())),
                               LinearModel<int&&, char&&>>);

  // lvalue ref
  static_assert(std::is_same_v<decltype(ForwardAsLinearModel(
                                   std::declval<int&>(), std::declval<char>())),
                               LinearModel<int&, char&&>>);

  static_assert(std::is_same_v<decltype(ForwardAsLinearModel(
                                   int{}, std::declval<char&>())),
                               LinearModel<int&&, char&>>);

  // const lvalue ref
  static_assert(
      std::is_same_v<decltype(ForwardAsLinearModel(std::declval<const int&>(),
                                                   std::declval<char>())),
                     LinearModel<const int&, char&&>>);

  static_assert(std::is_same_v<decltype(ForwardAsLinearModel(
                                   int{}, std::declval<const char&>())),
                               LinearModel<int&&, const char&>>);

  // Using reference_wrapper does nothing here
  static_assert(
      std::is_same_v<decltype(ForwardAsLinearModel(
                         int{}, std::ref(std::declval<char&>()))),
                     LinearModel<int&&, std::reference_wrapper<char>&&>>);

  static_assert(
      std::is_same_v<decltype(ForwardAsLinearModel(
                         std::cref(std::declval<const int&>()), char{})),
                     LinearModel<std::reference_wrapper<const int>&&, char&&>>);

  // rvalues
  static_assert(
      std::is_same_v<decltype(ForwardAsLinearModel(std::declval<int&&>(),
                                                   std::declval<char>())),
                     LinearModel<int&&, char&&>>);

  static_assert(
      std::is_same_v<decltype(ForwardAsLinearModel(std::declval<int>(),
                                                   std::declval<char&&>())),
                     LinearModel<int&&, char&&>>);
}

template <class T>
struct IsValidMock {
  MOCK_METHOD(bool, IsValid, (T), (const));
  friend constexpr auto IsValid(const IsValidMock& m, T o) -> bool {
    return m.IsValid(o);
  }

  MOCK_METHOD(bool, IsValidWithoutOffset, (), (const));
  friend constexpr auto IsValid(const IsValidMock& m) -> bool {
    return m.IsValidWithoutOffset();
  }
};

TEST(TestLinearModel, IsValid) {
  using testing::Return;

  {
    auto model = MakeLinearModel(1, 2);

    using model_traits = LinearModelTraits<decltype(model)>;
    static_assert(!model_traits::HasIsValid());

    EXPECT_TRUE(IsValid(model));
  }

  {
    auto model = MakeLinearModel(1);

    using model_traits = LinearModelTraits<decltype(model)>;
    static_assert(!model_traits::HasIsValid());

    EXPECT_TRUE(IsValid(model));
  }

  using testing::StrictMock;
  auto coeffs = StrictMock<IsValidMock<int>>{};

  {
    auto model = TieAsLinearModel(coeffs);

    using model_traits = LinearModelTraits<decltype(model)>;
    static_assert(model_traits::HasIsValid());

    EXPECT_CALL(coeffs, IsValidWithoutOffset())
        .Times(2)
        .WillOnce(Return(true))
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_TRUE(IsValid(model));
    EXPECT_FALSE(IsValid(model));
  }

  {
    auto offset = 2;
    auto model = MakeLinearModel(std::cref(coeffs), offset);

    using model_traits = LinearModelTraits<decltype(model)>;
    static_assert(model_traits::HasIsValid());

    EXPECT_CALL(coeffs, IsValid(offset))
        .Times(2)
        .WillOnce(Return(false))
        .WillOnce(Return(true))
        .RetiresOnSaturation();

    EXPECT_FALSE(IsValid(model));
    EXPECT_TRUE(IsValid(model));
  }
}

template <class T>
struct AcceptsMock {
  MOCK_METHOD(bool, Accepts, (T), (const));
  friend constexpr auto Accepts(const AcceptsMock& m, T t) -> bool {
    return m.Accepts(t);
  }
};

TEST(TestLinearModel, Accepts) {
  using testing::Return;

  {
    auto model = MakeLinearModel(1, 2);

    using model_traits = LinearModelTraits<decltype(model)>;
    static_assert(!model_traits::HasAccepts<int>());

    EXPECT_TRUE(Accepts(model, 3));
  }

  {
    auto model = MakeLinearModel(1);

    using model_traits = LinearModelTraits<decltype(model)>;
    static_assert(!model_traits::HasAccepts<int>());

    EXPECT_TRUE(Accepts(model, 3));
  }

  using testing::StrictMock;
  auto coeffs = StrictMock<AcceptsMock<int>>{};

  {
    auto model = TieAsLinearModel(coeffs);

    using model_traits = LinearModelTraits<decltype(model)>;
    static_assert(model_traits::HasAccepts<int>());

    EXPECT_CALL(coeffs, Accepts(4))
        .Times(2)
        .WillOnce(Return(true))
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_TRUE(Accepts(model, 4));
    EXPECT_FALSE(Accepts(model, 4));
  }
}

TEST(TestLinearModel, Solve) {
  using testing::StrictMock;
  using tests::ArgSide;
  using tests::ArithmeticMock;

  auto coeffs = StrictMock<ArithmeticMock<int, int>>{};
  auto offset = StrictMock<ArithmeticMock<int, int>>{};

  int x = 123;

  using testing::Return;

  {
    // WITH OFFSET
    testing::InSequence seq;

    // (coeffs * x) -> 321
    EXPECT_CALL(coeffs, Multiplication(x, ArgSide::Right))
        .Times(1)
        .WillOnce(Return(321))
        .RetiresOnSaturation();

    // offset + (coeffs * x)
    // offset + 321 -> -1
    EXPECT_CALL(offset, Addition(321, ArgSide::Right))
        .Times(1)
        .WillOnce(Return(-1))
        .RetiresOnSaturation();

    EXPECT_EQ(-1, Solve(ForwardAsLinearModel(coeffs, offset), x));
  }

  {
    // NO OFFSET

    // (coeffs * x) -> 456
    EXPECT_CALL(coeffs, Multiplication(x, ArgSide::Right))
        .Times(1)
        .WillOnce(Return(456))
        .RetiresOnSaturation();

    EXPECT_EQ(456, Solve(ForwardAsLinearModel(coeffs), x));
  }
}

}  // namespace
}  // namespace lfc
