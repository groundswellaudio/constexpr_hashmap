#pragma once

#include "incremental_hashmap.hpp"

namespace swl
{

struct empty{};

template <class Key, class Hash = hash<Key>, int Bucket = 16>
class incremental_hashset : incremental_hashmap<Key, empty, Hash, Bucket>
{
  using Base = incremental_hashmap<Key, empty, Hash, Bucket>;
  
  public : 
  
  constexpr bool insert( const Key& key ) 
  {
    auto [v, success] = Base::insert(key, {});
    return success;
  }
  
  using Base::contains;
  
  struct iterator : Base::iterator 
  {
    constexpr auto& operator*() const { return Base::iterator::operator*().key; } 
  };
  
  constexpr auto begin() { return iterator{Base::begin()}; }
  constexpr auto end()   { return iterator{Base::end()}; }
};

}