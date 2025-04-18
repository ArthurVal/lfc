#include <cstdio>
#include <cstring>
#include <memory>
#include <optional>
#include <string>

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "lfc/config.hpp"

namespace lfc {
namespace {

constexpr auto PipeSafelyClose = [](FILE* const f) noexcept -> void {
  if (f) pclose(f);
};

auto PipeOpen(const char* cmd, const char* modes)
    -> std::unique_ptr<FILE, decltype(PipeSafelyClose)> {
  return {
      popen(cmd, modes),
      PipeSafelyClose,
  };
}

auto FileToString(std::FILE& file,
                  const std::size_t chunks = 1024) noexcept -> std::string {
  std::string output(chunks, '\0');

  std::size_t read = 0;
  while ((read = std::fread(output.data() + (output.size() - chunks), 1, chunks,
                            std::addressof(file))) == chunks) {
    output.resize(output.size() + chunks, '\0');
  }

  // Remove the extra stuff
  // This is mandatory otherwise the size won't match the number of byte read
  output.resize(output.size() - (chunks - read));

  return output;
}

auto GetStdoutFromCmd(const char* cmd) -> std::optional<std::string> {
  std::optional<std::string> res = std::nullopt;

  if (auto pipe = PipeOpen(cmd, "r")) {
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
