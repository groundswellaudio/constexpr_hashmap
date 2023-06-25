#pragma once

#include <type_traits>

namespace swl
{

template <class T>
struct hash;

template <class T>
  requires std::is_integral_v<T>
struct hash<T>
{
  constexpr std::size_t operator()(T v) const { return static_cast<std::size_t>(v); }
};

template <class T>
  requires std::is_floating_point_v<T>
struct hash<T>
{
  constexpr std::size_t operator()(T v) const { 
    if (v == 0.f or v == -0.f)
      return 0;
    return static_cast<std::size_t>(v);
  }
};

// can't hash pointers at compile time

};
