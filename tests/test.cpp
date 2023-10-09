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
  
  for (int i = num_step; i < num_step * 3; ++i)
    ensure( not map.contains(i) );
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

consteval void test_iter_type(const swl::incremental_hashmap<int, int>& x)
{
  const swl::incremental_hashmap<int, int> constMap;
  static_assert( std::is_same_v< decltype((constMap.begin()->value)), const int& > );
  
  swl::incremental_hashmap<int, int> Map;
  static_assert( std::is_same_v< decltype((Map.begin()->value)), int& > );
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
  const auto begin = 1241342424;
  const auto num_step = 1234;
  swl::incremental_hashset<unsigned> s;
  
  for (unsigned k = begin; k < begin + num_step; ++k)
  {
    auto [ptr, success] = s.emplace(k);
    ensure( success );
  }
  
  for (unsigned k = begin + num_step; k < begin + num_step * 3; ++k)
  {
    ensure( not s.contains(k) );
  }
}

int main()
{
  test_map1();
  test_map2();
  test_iter_basic();
  test_iter();
  test_set();
}