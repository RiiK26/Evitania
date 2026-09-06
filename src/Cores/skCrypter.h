#pragma once
#include <utility>
#include <cstddef>

namespace skc
{
  template<typename Char, size_t N, Char Key, typename Indices>
  struct CryptData;

  template<typename Char, size_t N, Char Key, size_t... Is>
  struct CryptData<Char, N, Key, std::index_sequence<Is...>>
  {
    Char data[N];
    constexpr CryptData(const Char* str) :
        data{(Char) (str[Is] ^ Key)...}
    {
    }
  };
}  // namespace skc

#define skCrypt(str) \
  []() -> const char* { \
    constexpr size_t len       = sizeof(str); \
    constexpr char   key       = (char) (((__TIME__[7] - '0') + (__LINE__ % 255) + 1) & 0xFF); \
    static auto      crypted   = skc::CryptData<char, len, key, std::make_index_sequence<len>>(str); \
    static bool      decrypted = false; \
    if (!decrypted) { \
      for (size_t i = 0; i < len; ++i) \
        crypted.data[i] ^= key; \
      decrypted = true; \
    } \
    return crypted.data; \
  }()
