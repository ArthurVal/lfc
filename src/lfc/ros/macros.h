#pragma once

#define FWD(v) std::forward<decltype(v)>(v)

#define STRINGIZE_IMPL(x) #x
#define STRINGIZE(x) STRINGIZE_IMPL(x)

#define FILE_LINE __FILE__ "@" STRINGIZE(__LINE__)
