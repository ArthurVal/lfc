#include <type_traits>

// lfc
#include "lfc/linear_model.hpp"

// test utils
#include "tests/fixtures/linear_model.hpp"

// gtest
#include "gtest/gtest.h"

namespace lfc {
namespace {

TEST(LinearModelTest, Make) {
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

TEST(LinearModelTest, Tie) {
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

TEST(LinearModelTest, Forward) {
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

using LinearModelMockedTest = tests::LinearModelFixture<int>;

TEST_F(LinearModelMockedTest, IsValid) {
  using testing::Const;
  using testing::Ref;
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

  {
    auto model = MockedModelWithoutOffset();

    using model_traits = LinearModelTraits<decltype(model)>;
    static_assert(model_traits::HasIsValid());

    EXPECT_CALL(model.coeffs, IsValid())
        .Times(2)
        .WillOnce(Return(true))
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_TRUE(IsValid(model));
    EXPECT_FALSE(IsValid(model));
  }

  {
    auto model = MockedModel();

    using model_traits = LinearModelTraits<std::decay_t<decltype(model)>>;
    static_assert(model_traits::HasIsValid());

    EXPECT_CALL(model.coeffs, IsValid(Ref(model.offset)))
        .Times(2)
        .WillOnce(Return(false))
        .WillOnce(Return(true))
        .RetiresOnSaturation();

    EXPECT_FALSE(IsValid(model));
    EXPECT_TRUE(IsValid(model));
  }

  {
    auto model = MockedConstModelWithoutOffset();

    using model_traits = LinearModelTraits<decltype(model)>;
    static_assert(model_traits::HasIsValid());

    EXPECT_CALL(model.coeffs, IsValid())
        .Times(2)
        .WillOnce(Return(true))
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_TRUE(IsValid(model));
    EXPECT_FALSE(IsValid(model));
  }
}

TEST_F(LinearModelMockedTest, Accepts) {
  using testing::Const;
  using testing::Ref;
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

  {
    auto model = MockedModelWithoutOffset();
    input_t x = 4;

    using model_traits = LinearModelTraits<decltype(model)>;
    static_assert(model_traits::HasAccepts<int>());
    static_assert(!model_traits::HasAccepts<std::string>());

    EXPECT_CALL(model.coeffs, Accepts(x))
        .Times(2)
        .WillOnce(Return(true))
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_TRUE(Accepts(model, x));
    EXPECT_FALSE(Accepts(model, x));
  }

  {
    auto model = MockedConstModelWithoutOffset();
    input_t x = 5;

    using model_traits = LinearModelTraits<decltype(model)>;
    static_assert(model_traits::HasAccepts<int>());
    static_assert(!model_traits::HasAccepts<const char*>());

    EXPECT_CALL(model.coeffs, Accepts(x))
        .Times(2)
        .WillOnce(Return(false))
        .WillOnce(Return(true))
        .RetiresOnSaturation();

    EXPECT_FALSE(Accepts(model, x));
    EXPECT_TRUE(Accepts(model, x));
  }
}

TEST_F(LinearModelMockedTest, Solve) {
  using testing::Ref;
  using testing::Return;
  using tests::ArgSide;

  input_t x = 123;

  {
    // WITH OFFSET
    auto model = MockedModel();
    testing::InSequence seq;

    // (coeffs * x) -> 321
    EXPECT_CALL(model.coeffs, Multiplication(x, ArgSide::Right))
        .Times(1)
        .WillOnce(Return(321))
        .RetiresOnSaturation();

    // offset + (coeffs * x)
    // offset + 321 -> -1
    EXPECT_CALL(model.offset, Addition(321, ArgSide::Right))
        .Times(1)
        .WillOnce(Return(-1))
        .RetiresOnSaturation();

    EXPECT_EQ(-1, Solve(model, x));
  }

  {
    // NO OFFSET
    auto model = MockedModelWithoutOffset();

    // (coeffs * x) -> 456
    EXPECT_CALL(model.coeffs, Multiplication(x, ArgSide::Right))
        .Times(1)
        .WillOnce(Return(456))
        .RetiresOnSaturation();

    EXPECT_EQ(456, Solve(model, x));
  }
}

TEST_F(LinearModelMockedTest, TryToSolve) {
  using tests::ArgSide;

  using testing::Ref;
  using testing::Return;

  input_t x = 123;

  {
    // WITH OFFSET
    auto model = MockedModel();
    testing::InSequence seq;

    // (coeffs * x) -> 321
    EXPECT_CALL(model.coeffs, Multiplication(x, ArgSide::Right))
        .Times(1)
        .WillOnce(Return(321))
        .RetiresOnSaturation();

    // offset + (coeffs * x)
    // offset + 321 -> -1
    EXPECT_CALL(model.offset, Addition(321, ArgSide::Right))
        .Times(1)
        .WillOnce(Return(-1))
        .RetiresOnSaturation();

    EXPECT_EQ(-1, TryToSolve(model, x));
  }

  {
    // NO OFFSET
    auto model = MockedModelWithoutOffset();
    testing::InSequence seq;

    // (coeffs * x) -> 321
    EXPECT_CALL(model.coeffs, Multiplication(x, ArgSide::Right))
        .Times(1)
        .WillOnce(Return(321))
        .RetiresOnSaturation();

    EXPECT_EQ(321, TryToSolve(model, x));
  }

  {
    // IsValid fails
    auto model = MockedModelWithoutOffset();
    testing::InSequence seq;

    EXPECT_CALL(model.coeffs, IsValid())
        .Times(1)
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_EQ(std::nullopt, TryToSolve(model, x));
  }

  {
    // Accepts fails
    auto model = MockedModelWithoutOffset();
    testing::InSequence seq;

    EXPECT_CALL(model.coeffs, IsValid())
        .Times(1)
        .WillOnce(Return(true))
        .RetiresOnSaturation();

    EXPECT_CALL(model.coeffs, Accepts(x))
        .Times(1)
        .WillOnce(Return(false))
        .RetiresOnSaturation();

    EXPECT_EQ(std::nullopt, TryToSolve(model, x));
  }
}

using LinearModelMockedDeathTest = LinearModelMockedTest;
TEST_F(LinearModelMockedDeathTest, SolvePreconditions) {
  using testing::_;
  using testing::Return;

  auto model = MockedModelWithoutOffset();
  ON_CALL(model.coeffs, Multiplication(_, _)).WillByDefault(Return(-1));

  EXPECT_DEBUG_DEATH(
      {
        ON_CALL(model.coeffs, IsValid()).WillByDefault(Return(false));
        Solve(model, input_t{});
      },
      "IsValid\\(m\\)");

  EXPECT_DEBUG_DEATH(
      {
        ON_CALL(model.coeffs, IsValid()).WillByDefault(Return(true));
        ON_CALL(model.coeffs, Accepts(_)).WillByDefault(Return(false));
        Solve(model, input_t{});
      },
      "Accepts\\(m, x\\)");
}

}  // namespace
}  // namespace lfc
