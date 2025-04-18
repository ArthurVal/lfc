#include <cstring>
#include <optional>
#include <string>

// LFC libs
#include "lfc/config.hpp"

// tests utils
#include "tests/file.hpp"

// Ext
#include "gmock/gmock.h"
#include "gtest/gtest.h"

namespace lfc {
namespace {

auto StdoutFrom(const char* cmd) -> std::optional<std::string> {
  using tests::FileToString;
  using tests::PipeOpen;

  std::optional<std::string> res = std::nullopt;

  if (auto pipe = PipeOpen(cmd)) {
    res = FileToString(*pipe, (1 << 8));
  }

  return res;
}

#define XSTR(x) STR(x)
#define STR(x) #x

TEST(ConfigTest, VersionMatchesWithGit) {
  using testing::ContainsRegex;

#ifdef PROJECT_GIT_LOCATION
  EXPECT_THAT(
      StdoutFrom(
          "cd " XSTR(PROJECT_GIT_LOCATION) " && git describe --abbrev=0 2>&1")
          .value_or(std::strerror(errno)),
      ContainsRegex("v" lfc_VERSION_STR));
#else
  EXPECT_THAT(
      StdoutFrom("git describe --abbrev=0 2>&1").value_or(std::strerror(errno)),
      ContainsRegex("v" lfc_VERSION_STR));
#endif
}

}  // namespace
}  // namespace lfc
