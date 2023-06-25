#pragma once

#include <memory>
#include <type_traits>
#include "hash.hpp"

namespace swl
{

template <class A, class B>
struct pair {
  A first;
  B second;
};

template <class Key, class Val, class Hash = hash<Key>, int Bucket = 16>
class incremental_hashmap
{
  public : 
  
  struct element
  { 
    Key key;
    [[no_unique_address]] Val value;
  };
  
  private : 
  
  union ElemStorage
  {
    char e;
    element value;
  };
  
  struct chunk 
  {
  
    constexpr ~chunk() {
      delete next;
      //next.reset();
      for (int k = 0; k < Bucket; ++k)
        if (flag[k])
          std::destroy_at(&elems[k].value);
    }
    
    ElemStorage elems[Bucket];
    bool flag[Bucket] = {false};
    chunk* next = nullptr;
  };
  
  chunk root;
  
  struct table;
  
  static constexpr std::size_t hash(const Key& K) {
    return Hash{}(K);
  }
  
  template <class... Args>
  constexpr element* construct_at(chunk& t, unsigned idx, Args&&... args) {
    t.flag[idx] = true;
    return std::construct_at(&t.elems[idx].value, element{args...});
  }
  
  struct chunk_and_index {
    chunk& c;
    std::size_t index;
    bool must_alloc = false;
  };
  
  constexpr chunk_and_index find_impl(const Key& key) const 
  {
    const auto idx = hash(key) % Bucket;
    auto* c = &root;
    
    auto k = idx;
    while(true)
    {
      if ((not c->flag[k]) || (c->elems[k].value.key == key))
        return {(chunk&)*c, k};
      ++k;
      if (k == Bucket)
      {
        if (not c->next)
          return {(chunk&)*c, idx, true};
        c = c->next;
        k = idx;
      }
    }
  }
  
  constexpr chunk_and_index find_or_alloc(const Key& key) 
  {
    auto [c, idx, must_alloc] = find_impl(key);
    if (not must_alloc)
      return {c, idx};
    c.next = new chunk{};
    return {*c.next, idx};
  }
  
  public : 
  
  struct iterator 
  {
    constexpr iterator(chunk* c_, int idx_) 
    : c{c_}, idx{idx_}
    {}
    
    
    constexpr auto& operator++() { 
      while(true)
      {
        if (++idx == Bucket)
        {
          c = c->next; 
          idx = 0;
          
          if (not c)
            return *this;
        }
        
        if (c->flag[idx])
          return *this;
      }
    }
    
    constexpr auto& operator*() const {
      return (c->elems[idx].value);
    }
    
    constexpr bool operator==(iterator i) const {
      return c == i.c and idx == i.idx;
    }
    
    chunk* c;
    int idx;
  };
  
  constexpr auto begin() {
    auto res = iterator{&root, 0};
    return root.flag[0] ? iterator{&root, 0} : ++iterator{&root, 0};
  }
  
  constexpr auto end() {
    return iterator{nullptr, 0};
  }
  
  constexpr pair<element*, bool> insert( const Key& key, Val val ) 
  {
    auto [c, idx, z] = find_or_alloc(key);
    if ( c.flag[idx] )
      return {&c.elems[idx].value, false};
    return {construct_at(c, idx, key, (Val&&)val), true};
  }
  
  constexpr bool contains(const Key& key) const {
    auto [c, idx, zz] = find_impl(key);
    return (c.flag[idx]);
  }
  
  constexpr Val* find(const Key& key) {
    auto [c, idx, zz] = find_impl(key);
    return c.flag[idx] ? &c.elems[idx].value.value : nullptr;
  }
  
  constexpr const Val* find(const Key& key) const {
    return const_cast<incremental_hashmap&>(*this).find(key);
  }
};

}