#pragma once

#include <memory>
#include <type_traits>
#include "./hash.hpp"

namespace swl
{

template <class A, class B>
struct pair {
  A first;
  B second;
};

struct empty {};

// A simple hashmap made to perform reasonably well when : 
// - avoiding memory relocation
// - deletion from the map is not needed
// - inputs are small (the meaning of small depends on the BucketSize)

// it's implemented as a forward list of fixed size bucket, 
// the map initially looks at hash % BucketSize, perform N steps of linear probing (with N << BucketSize)
// if not found, look at the next bucket, repeat.

template <class Key, class Val, class Hash = hash<Key>, std::size_t BucketSize = 32>
class incremental_hashmap
{
  static constexpr int ProbingSteps = BucketSize / 5 <= 3 ? 3 : BucketSize / 5;
  
  public : 
  
  struct element
  { 
    const Key key;
    [[no_unique_address]] Val value;
  };
  
  private : 
  
  union ElemStorage
  {
    constexpr ~ElemStorage() 
    requires (std::is_trivially_destructible_v<element>)
    = default;
    
    constexpr ~ElemStorage() 
    requires (not std::is_trivially_destructible_v<element>)
    {
    }
    
    empty e;
    element data;
  };
  
  struct chunk 
  {
    constexpr chunk() = default;
    
    constexpr chunk(chunk&& c)
    {
      for (int k = 0; k < BucketSize; ++k)
      {
        if (not c.flag[k])
          continue;
        std::construct_at( &elems[k].data, (element&&) c.elems[k].data );
        flag[k] = true;
      }
      next = c.next;
      c.next = nullptr;
    }
    
    // only to be called in dtor
    constexpr void destroy_elems() {
      for (int k = 0; k < BucketSize; ++k)
        if (flag[k])
          std::destroy_at(&elems[k].data);
    }
    
    constexpr void destroy() {
      destroy_elems();
      for (auto& f : flag)
        f = false;
    }
    
    ElemStorage elems[BucketSize] = {empty{}};
    bool flag[BucketSize] = {false};
    chunk* next = nullptr;
  };
  
  chunk root;
  
  static constexpr std::size_t hash(const Key& K) {
    return Hash{}(K);
  }
  
  template <class... Args>
  constexpr element* construct_at(chunk& t, unsigned idx, Args&&... args) {
    t.flag[idx] = true;
    return std::construct_at(&t.elems[idx].data, (Args&&) args...);
  }
  
  struct try_find_result {
    chunk& c;
    std::size_t index;
    bool found = false;
  };
  
  static constexpr auto make_next_chunk(chunk* c) {
    auto next = new chunk{};
    c->next = next;
    return next;
  }
  
  // find the location of key, or a suitable place to emplace it
  // - first search at base_index = hash % BucketSize
  // - if not found, prob linearly between index + ProbingStep (and wrap around if needed)
  // - if not found, try the next chunk
  constexpr try_find_result try_find(const Key& key, bool for_emplace = false) const 
  {
    const auto idx = hash(key) % BucketSize;
    auto* c = &root;
    
    while(true)
    {
      auto k = idx;
      for (int n = 0; n < ProbingSteps; ++n)
      {
        if (not c->flag[k])
          return {(chunk&)*c, k, false};
        if (c->elems[k].data.key == key)
          return {(chunk&)*c, k, true};
          
        ++k;
        k = (k == BucketSize) ? 0 : k;
      }
      
      if (c->next)
        c = c->next;
      else if (not for_emplace)
        return {(chunk&)*c, k, false};
      else
        return {*make_next_chunk((chunk*)c), idx, false};
    }
  }
  
  constexpr void destroy() {
    root.destroy_elems();
    for (chunk* c = root.next; (bool)c; )
    {
      c->destroy_elems();
      auto old = c;
      c = c->next;
      delete old;
    }
    root.next = nullptr; 
  }
  
  constexpr void destroy_for_reuse() {
    destroy();
    for (auto& f : root.flag)
      f = false;
  }
  
  constexpr void assign_from(const incremental_hashmap& o)
  {
    const chunk* oc = &o.root;
    chunk* c = &root;
    
    while(true)
    {
      for (int k = 0; k < BucketSize; ++k)
      {
        if (oc->flag[k])
        {
          std::construct_at( &c->elems[k].data, oc->elems[k].data );
          c->flag[k] = true;
        }
      }
      
      oc = oc->next;
      if (not oc)
        return;
      c = make_next_chunk(c);
    }
  }
  
  public : 
  
  template <class T>
  struct iterator_t 
  {
    constexpr iterator_t(chunk* c_, int idx_) 
    : c{c_}, idx{idx_}
    {}
    
    constexpr auto& operator++() 
    { 
      if (not c)
        return *this;
      
      while(++idx != BucketSize)
        if (c->flag[idx])
          return *this;
      
      c = c->next;
      if (not c)
        return *this;
      idx = 0;
      // There is at least one element per bucket
      for (; not c->flag[idx]; ++idx)
        ;
      return *this;
    }
    
    constexpr T& operator*() const { return (c->elems[idx].data); }
    
    constexpr T* operator->() const { return (&c->elems[idx].data); }
    
    constexpr bool operator==(iterator_t i) const {
      return c == i.c and idx == i.idx;
    }
    
    private : 
    
    chunk* c;
    int idx;
  };
  
  using key_type = Key;
  using value_type = Val;
  using iterator = iterator_t<element>;
  using const_iterator = iterator_t<const element>;

  constexpr incremental_hashmap() = default;
  
  constexpr incremental_hashmap(const incremental_hashmap& o) 
    requires (std::is_copy_constructible_v<Val>)
  {
    assign_from(o);
  }
  
  constexpr incremental_hashmap(incremental_hashmap&& o) 
  : root{(chunk&&)o.root}
  {
  }
  
  constexpr incremental_hashmap& operator=(const incremental_hashmap& o)
    requires (std::is_copy_constructible_v<Val>)
  {
    destroy_for_reuse();
    assign_from(o);
    return *this;
  }
  
  constexpr ~incremental_hashmap() 
  {
    destroy();
  }
  
  constexpr iterator begin() {
    return root.flag[0] ? iterator{&root, 0} : ++iterator{&root, 0};
  }
  
  constexpr iterator end() {
    return iterator{nullptr, BucketSize};
  }
  
  constexpr const_iterator begin() const {
    auto r = const_iterator{(chunk*)&root, 0};
    return root.flag[0] ? r : ++r;
  }
  
  constexpr const_iterator end() const {
    return const_iterator{nullptr, BucketSize};
  }
  
  template <class... Args>
  constexpr pair<element*, bool> emplace( const Key& key, Args&&... args ) {
    auto [ptr, success] = try_emplace( key, (Args&&) args... );
    if (not success)
      ptr->value = Val{(Args&&)args...};
    return {ptr, success};
  }
  
  template <class... Args>
  constexpr pair<element*, bool> try_emplace( const Key& key, Args&&... args )
  {
    auto [c, idx, found] = try_find(key, true);
    if (found)
      return {&c.elems[idx].data, false};
    return {construct_at(c, idx, key, (Args&&)args...), true};
  }
  
  constexpr bool contains(const Key& key) const {
    auto [c, idx, found] = try_find(key);
    return found;
  }
  
  constexpr Val* find(const Key& key) {
    auto [c, idx, found] = try_find(key);
    return found ? &c.elems[idx].data.value : nullptr;
  }
  
  constexpr const Val* find(const Key& key) const {
    return const_cast<incremental_hashmap&>(*this).find(key);
  }
};

}