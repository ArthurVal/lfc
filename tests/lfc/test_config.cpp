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

auto GetStdoutFromCmd(const char* cmd) -> std::optional<std::string> {
  using tests::FileToString;
  using tests::PipeOpen;

  std::optional<std::string> res = std::nullopt;

  if (auto pipe = PipeOpen(cmd)) {
    res = FileToString(*pipe, (1 << 8));
  }

  return res;
}

TEST(ConfigTest, VersionMatchesWithGit) {
  using testing::ContainsRegex;
  EXPECT_THAT(GetStdoutFromCmd("git describe --abbrev=0 2>&1")
                  .value_or(std::strerror(errno)),
              ContainsRegex("v" lfc_VERSION_STR));
}

}  // namespace
}  // namespace lfc
