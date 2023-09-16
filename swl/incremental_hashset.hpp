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
  
  struct iterator : Base::const_iterator
  {
    constexpr const Key& operator*() const { return Base::const_iterator::operator*().key; } 
    constexpr const Key* operator->() const { return &Base::const_iterator::operator*().key; }
  };
  
  using value_type = Key;
  using const_iterator = iterator;
  
  constexpr auto begin() const { return iterator{Base::begin()}; }
  constexpr auto end() const { return iterator{Base::end()}; }
};

}