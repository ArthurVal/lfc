#include "tests/file.hpp"

#include <cassert>

namespace tests {

namespace {

constexpr auto ToString(PipeDirection dir) noexcept
    -> std::optional<const char*> {
  switch (dir) {
    case PipeDirection::kRead:
      return "r";
    case PipeDirection::kWrite:
      return "w";
  }

  return std::nullopt;
}

}  // namespace

auto FileOpen(const char* name, char const* mode) noexcept -> FilePtr {
  return {
      std::fopen(name, mode),
      FileSafelyClose,
  };
}

auto PipeOpen(const char* cmd, PipeDirection dir) noexcept -> PipePtr {
  const auto dir_str = ToString(dir);
  assert(dir_str.has_value());

  return {
      popen(cmd, dir_str.value_or("")),
      PipeSafelyClose,
  };
}

auto FileToString(std::FILE& file,
                  const std::size_t chunks) noexcept -> std::string {
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

auto FileToString(const std::filesystem::path& path) noexcept
    -> std::optional<std::string> {
  std::optional<std::string> res = std::nullopt;

  if (auto file_ptr = FileOpen(path.c_str(), "r")) {
    res = FileToString(*file_ptr);
  }

  return res;
}

}  // namespace tests
