#pragma once

#include "./incremental_hashmap.hpp"

namespace swl
{

template <class Key, class Hash = hash<Key>, int Bucket = 16>
class incremental_hashset : incremental_hashmap<Key, empty, Hash, Bucket>
{
  using Base = incremental_hashmap<Key, empty, Hash, Bucket>;
  
  public : 
  
  using Base::Base;
  
  constexpr pair<const Key*, bool> emplace( const Key& key ) {
    auto [v, success] = Base::emplace(key, empty{});
    return {&v->key, success};
  }
  
  constexpr pair<const Key*, bool> try_emplace( const Key& key ) {
    auto [v, success] = Base::try_emplace(key, empty{});
    return {&v->key, success};
  }
  
  using Base::contains;
  
  struct iterator : Base::iterator 
  {
    constexpr const auto& operator*() const { return Base::iterator::operator*().key; } 
  };
  
  constexpr auto begin() { return iterator{Base::begin()}; }
  constexpr auto end()   { return iterator{Base::end()}; }
};

}