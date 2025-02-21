#include "gtest/gtest.h"
#include "lfc/utils/interger_sequence.hpp"

namespace lfc::utils {
namespace {

TEST(TestIntegerSequence, SliceOfInts) {
  static_assert(std::is_same_v<std::integer_sequence<int>,
                               decltype(SliceOfInts<int, 0, 0>())>);

  static_assert(std::is_same_v<std::integer_sequence<int>,
                               decltype(SliceOfInts<int, 314, 0>())>);

  static_assert(std::is_same_v<std::integer_sequence<int, 1, 2, 3>,
                               decltype(SliceOfInts<int, 1, 3>())>);

  static_assert(std::is_same_v<std::integer_sequence<int, 3, 4, 5>,
                               decltype(SliceOfInts<int, 3, 3>())>);
}

TEST(TestIntegerSequence, SliceOfIndex) {
  static_assert(
      std::is_same_v<std::index_sequence<>, decltype(SliceOfIndex<0, 0>())>);

  static_assert(
      std::is_same_v<std::index_sequence<>, decltype(SliceOfIndex<3, 0>())>);

  static_assert(std::is_same_v<std::make_index_sequence<4>,
                               decltype(SliceOfIndex<0, 4>())>);

  static_assert(std::is_same_v<std::index_sequence<3, 4, 5>,
                               decltype(SliceOfIndex<3, 3>())>);

  static_assert(
      std::is_same_v<std::index_sequence<3>, decltype(SliceOfIndex<3, 1>())>);
}

}  // namespace

}  // namespace lfc::utils
