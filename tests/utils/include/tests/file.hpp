#pragma once

#include <cstdio>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>

namespace tests {

/**
 *  @brief Safely close any std::FILE ptr
 *
 *  Architecture Decision (AD): We use a lambda here because of a caveat of
 *  using std::unique_ptr<> with custom deleter. When using a function, the
 *  unique_ptr store the function ptr alongside the ptr to watch for, therefore
 *  doubling its size...
 *
 *  See https://godbolt.org/z/vh8zE4bYo
 *
 *  @param[in] file The file ptr we wish to close
 */
inline constexpr auto FileSafelyClose =
    [](std::FILE* const f) noexcept -> void {
  if (f) std::fclose(f);
};

/// Alias use to represents safe file handler (using RAII) returned by FileOpen
using FilePtr = std::unique_ptr<std::FILE, decltype(FileSafelyClose)>;

/**
 *  @brief Wrapper around std::fopen in order to create a unique_ptr<std::FILE>
 *
 *  @param[in] name Name of the file, see std::fopen
 *  @param[in] mode File access flags (r, w, r+, w+, ...), see std::fopen
 *
 *  @return FilePtr containing a std::FILE on success, with a custom deleter to
 *                  close the file automatically. Nullptr when the operation
 *                  failed with errno set according to std::fopen().
 */
auto FileOpen(const char* name, char const* mode) noexcept -> FilePtr;

/**
 *  @brief Safely close a std::FILE opened through popen
 *
 *  Architecture Decision (AD): We use a lambda here because of a caveat of
 *  using std::unique_ptr<> with custom deleter. When using a function, the
 *  unique_ptr store the function ptr alongside the ptr to watch for, therefore
 *  doubling its size...
 *
 *  @param[in] f The pipe ptr we wish to pclose
 */
inline constexpr auto PipeSafelyClose =
    [](std::FILE* const f) noexcept -> void {
  if (f) pclose(f);
};

/// Alias use to represents safe file handler (using RAII) returned by PipeOpen
using PipePtr = std::unique_ptr<std::FILE, decltype(PipeSafelyClose)>;

/// The pipe direction
enum class PipeDirection {
  kRead,  /*!< STDOUT of the process redirected into the pipe */
  kWrite, /*!< STDIN of the process redirected into the pipe */
};

/**
 *  @brief Wrapper around popen in order to create a unique_ptr<std::FILE>
 *
 *  @param[in] cmd The cmd, see popen()
 *  @param[in] dir Pipe direction
 *
 *  @pre dir is one of kRead or kWrite
 *
 *  @return PipePtr containing a std::FILE on success, with a custom deleter to
 *                  close the pipe automatically. Nullptr when the operation
 *                  failed with errno set according to popen().
 */
auto PipeOpen(const char* cmd,
              PipeDirection dir = PipeDirection::kRead) noexcept -> PipePtr;

/**
 *  @brief Dump the file content into a std::string
 *
 *  @warning Reading ends until EOF or if any file operation error happens,
 *           those must be checked by hand afterwards (std::ferror/std::feof)
 *
 *  @param[in] file File we wish to read from
 *  @param[in] chunks Size of the chunks read from the file
 *                    (default to 1024 bytes)
 *
 *  @return std::string The file content store within a string
 */
auto FileToString(std::FILE& file,
                  const std::size_t chunks = 1024) noexcept -> std::string;

/**
 *  @brief Dump the file located at \a path into a std::string
 *
 *  @param[in] path File path we wish to read from
 *
 *  @return std::string The file content store within a string if
 *          succeeded. Other std::nullopt, with errno set accordingly (see
 *          fopen())
 */
auto FileToString(const std::filesystem::path& path) noexcept
    -> std::optional<std::string>;

}  // namespace tests
