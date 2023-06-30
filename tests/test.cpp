#include "incremental_hashmap.hpp"
#include "incremental_hashset.hpp"

#include <iostream>

constexpr void ensure(bool v) {
  if (not v)
    throw "error";
}

template <class M>
void dump(M& map) 
{
  std::cout << "{";
  for (auto& e : map)
    std::cout << "(" << e.key << ", " << e.value << "), ";
  std::cout << "} \n";
}

consteval void test1()
{
  swl::incremental_hashmap<int, int> map;
  
  map.emplace( 4, 5 );

  ensure( map.contains(4) );
  ensure( *map.find(4) == 5 );
  
  map.emplace( 20, 8 );
  
  ensure( map.contains(4) && map.contains(20) );
  ensure( *map.find(4) == 5 );
  ensure( *map.find(20) == 8 );
}

static constexpr auto num_step = 324;

consteval void test2()
{
  swl::incremental_hashmap<int, int> map;
  
  for (int i = 0; i < num_step; ++i) 
  {
    map.emplace(i, i * 4);
  }
  
  for (int i = 0; i < num_step; ++i) 
  { 
    ensure( *map.find(i) == i * 4);
  }
}


consteval void test_set()
{
  swl::incremental_hashset<unsigned> s;
  
  unsigned Val[] = {
    4, 33, 11, 1, 11, 11, 22, 3, 15, 45, 102, 939, 23, 56, 2, 46, 412, 23429342, 32424, 293
  };
  
  for (auto v : Val)
    s.emplace(v);
  
  for (auto v : Val)
    ensure( s.contains(v) );
}

int main()
{
  test1();
  test2();
  test_set();
}