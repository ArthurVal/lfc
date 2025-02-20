#include "gtest/gtest.h"
#include "lfc/utils/interger_sequence.hpp"

namespace lfc::utils {
namespace {

TEST(TestIntegerSequence, MakeIntegerSequence) {
  static_assert(std::is_same_v<std::integer_sequence<int>,
                               MakeIntegerSequenceStartingAt<int, 0, 0>>);

  static_assert(std::is_same_v<std::integer_sequence<int>,
                               MakeIntegerSequenceStartingAt<int, 314, 0>>);

  static_assert(std::is_same_v<std::integer_sequence<int, 1, 2, 3>,
                               MakeIntegerSequenceStartingAt<int, 1, 3>>);

  static_assert(std::is_same_v<std::integer_sequence<int, 3, 4, 5>,
                               MakeIntegerSequenceStartingAt<int, 3, 3>>);
}

TEST(TestIntegerSequence, MakeIndexSequence) {
  static_assert(
      std::is_same_v<std::index_sequence<>, MakeIndexSequenceStartingAt<0, 0>>);

  static_assert(
      std::is_same_v<std::index_sequence<>, MakeIndexSequenceStartingAt<3, 0>>);

  static_assert(std::is_same_v<std::make_index_sequence<4>,
                               MakeIndexSequenceStartingAt<0, 4>>);

  static_assert(std::is_same_v<std::index_sequence<3, 4, 5>,
                               MakeIndexSequenceStartingAt<3, 3>>);

  static_assert(std::is_same_v<std::index_sequence<3>,
                               MakeIndexSequenceStartingAt<3, 1>>);
}

}  // namespace

}  // namespace lfc::utils
