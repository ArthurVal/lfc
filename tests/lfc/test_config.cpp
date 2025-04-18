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
  using testing::Eq;

  const auto git_version =
#ifdef PROJECT_GIT_LOCATION
      StdoutFrom(
          "cd " XSTR(PROJECT_GIT_LOCATION) " && git describe --abbrev=0 2>&1")
          .value_or(std::strerror(errno));
#else
      StdoutFrom("git describe --abbrev=0 2>&1").value_or(std::strerror(errno));
#endif

  EXPECT_THAT(git_version, Eq("v" lfc_VERSION_STR "\n"))
      << "Both version must match:"
         "\n - The version in the main CMakeLists.txt;"
         "\n - The last git tag (annotated);";
}

}  // namespace
}  // namespace lfc
