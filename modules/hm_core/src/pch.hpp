#pragma once
#include <cstdint>
#include <cwchar>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

namespace hm {

using u8 = std::uint8_t;
using u16 = std::uint16_t;
using u32 = std::uint32_t;
using u64 = std::uint64_t;

using i8 = std::int8_t;
using i16 = std::int16_t;
using i32 = std::int32_t;
using i64 = std::int64_t;

using f32 = float;
using f64 = double;

using usize = std::size_t;

template <typename T>
using Unique = std::unique_ptr<T>;

template <typename T>
using Shared = std::shared_ptr<T>;

template <typename T>
using Opt = std::optional<T>;

template <typename T>
using Vec = std::vector<T>;

template <typename T, usize N>
using Array = std::array<T, N>;

template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;

using String = std::string;
using StringView = std::string_view;
} // namespace hm
