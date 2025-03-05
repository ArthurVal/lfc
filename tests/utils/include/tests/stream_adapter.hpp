#pragma once

#include <string_view>
#include <type_traits>

namespace tests {

template <class T>
struct StreamAdapter;

namespace details {

/// TODO
template <class Stream, class T, class = void>
struct IsStreamableInto : std::false_type {};

template <class Stream, class T>
struct IsStreamableInto<
    Stream, T,
    std::void_t<decltype(std::declval<Stream>() << std::declval<T>())>>
    : std::true_type {};

template <class Stream, class T>
constexpr bool IsStreamableInto_v = IsStreamableInto<Stream, T>::value;

/// TODO
template <class T>
struct IsStreamAdapter : std::false_type {};

template <class T>
struct IsStreamAdapter<StreamAdapter<T>> : std::true_type {};

template <class T>
constexpr bool IsStreamAdapter_v = IsStreamAdapter<T>::value;

/// TODO
template <class T>
struct StreamAdapter {
  template <class OStream>
  static constexpr auto CanBeStreamInto() -> bool {
    return details::IsStreamableInto_v<OStream, T>;
  }

  template <class OStream>
  static constexpr auto CanBeStreamInto(OStream&& os) -> bool {
    return CanBeStreamInto<decltype(os)>();
  }

  template <class U,
            std::enable_if_t<not std::is_same_v<std::decay_t<U>, StreamAdapter>,
                             bool> = true>
  constexpr StreamAdapter(U&& v, std::string_view backup = "<Not Streamable>")
      : m_value(std::forward<U>(v)), m_backup(backup) {}

  constexpr StreamAdapter() : StreamAdapter(T{}) {}

  constexpr StreamAdapter(const StreamAdapter&) = default;
  constexpr StreamAdapter& operator=(const StreamAdapter&) = default;

  constexpr StreamAdapter(StreamAdapter&&) = default;
  constexpr StreamAdapter& operator=(StreamAdapter&&) = default;

  ~StreamAdapter() = default;

  constexpr auto OrElsePrint(std::string_view msg) -> StreamAdapter& {
    m_backup = msg;
    return *this;
  }

  template <class AnyOStream>
  friend constexpr auto operator<<(
      AnyOStream&& os, const StreamAdapter& adapter) -> decltype(auto) {
    if constexpr (CanBeStreamInto<decltype(os)>()) {
      os << adapter.m_value;
    } else {
      os << adapter.m_backup;
    }

    return std::forward<AnyOStream>(os);
  }

  template <class AnyOStream>
  friend constexpr auto operator<<(AnyOStream&& os,
                                   StreamAdapter&& adapter) -> decltype(auto) {
    if constexpr (CanBeStreamInto<decltype(os)>()) {
      os << std::move(adapter.m_value);
    } else {
      os << std::move(adapter.m_backup);
    }

    return std::forward<AnyOStream>(os);
  }

 private:
  T m_value;
  std::string_view m_backup;
};

}  // namespace details

/// TODO
template <class T>
constexpr auto TryToStream(T&& value) -> details::StreamAdapter<T&&> {
  return {std::forward<T>(value)};
}

}  // namespace tests
