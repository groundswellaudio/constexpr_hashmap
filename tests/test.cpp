#include <swl/incremental_hashmap.hpp>
#include <swl/incremental_hashset.hpp>

constexpr void ensure(bool v) {
  if (not v)
    throw "error";
}

consteval void test_map1()
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

consteval void test_map2()
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


consteval void check(bool cond) {
  if (not cond)
    throw "fail";
}

consteval void test_iter_basic()
{
  swl::incremental_hashmap<int, int> map;
  ensure( map.begin() == map.end() );
  
  for (int k = 0; k < 431; k += 7)
    map.emplace(231 + k, k);
  
  auto it = map.begin();
  for (int k = 0; k < 431; ++k)
    ++it;
  
  ensure( it == map.end() );
}

consteval void test_iter() 
{
  swl::incremental_hashmap<int, int> map;
  
  for (int i = 0; i < num_step; ++i) 
    map.emplace(124 + i, 0);
  
  int count = 0;
  for (auto& e : map)
  {
    ++(*map.find(e.key));
    ++count;
  }
  
  for (auto& e : map)
    ensure(e.value == 1);
  ensure( count == num_step );
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
  test_map1();
  test_map2();
  test_iter_basic();
  test_iter();
  test_set();
}