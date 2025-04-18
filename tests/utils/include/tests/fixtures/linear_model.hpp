#pragma once

// LFC main lib
#include "lfc/linear_model.hpp"

// LFC test utils lib
#include "tests/mocks/linear_model.hpp"

// Ext
#include "gtest/gtest.h"

namespace tests {

template <class T>
struct LinearModelFixture : public testing::Test {
  using input_t = T;

  static void SetUpTestSuite() {}

  LinearModelFixture() {
    using testing::An;
    using testing::Const;
    using testing::Ref;
    using testing::Return;

    ON_CALL(coeffs_, IsValid()).WillByDefault(Return(true));
    ON_CALL(Const(coeffs_), IsValid()).WillByDefault(Return(true));

    ON_CALL(coeffs_, IsValid(Ref(offset_))).WillByDefault(Return(true));
    ON_CALL(Const(coeffs_), IsValid(Ref(offset_))).WillByDefault(Return(true));

    ON_CALL(coeffs_, Accepts(An<const T&>())).WillByDefault(Return(true));
    ON_CALL(Const(coeffs_), Accepts(An<const T&>()))
        .WillByDefault(Return(true));
  }

  void SetUp() override {};

  auto MockedModel() { return lfc::TieAsLinearModel(coeffs_, offset_); }

  auto MockedModelWithoutOffset() { return lfc::TieAsLinearModel(coeffs_); }

  auto MockedConstModel() {
    return lfc::MakeLinearModel(std::cref(coeffs_), std::ref(offset_));
  }

  auto MockedConstModelWithoutOffset() {
    return lfc::MakeLinearModel(std::cref(coeffs_));
  }

  void TearDown() override {};

  virtual ~LinearModelFixture() override {}

  static void TearDownTestSuite() {}

 protected:
  MockOffset<T> offset_;
  MockCoeffs<T, MockOffset<T>&> coeffs_;
};

}  // namespace tests
