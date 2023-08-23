# Constexpr associative containers

This repository provides constexpr incremental hash-map and set for C++20 and onwards
(incremental meaning you can add/mutate values, but not remove them) : 

`swl::incremental_hashmap<Key, Value, Hash>` \
`swl::incremental_hashset<Key, Hash>` 

Iterators and references are never invalidated. 

Those containers were designed for small, "metaprogramming sized" inputs in mind, in the future I'd like to add the non-incremental equivalents, however so far I have not found any use for those in my own code. 

## Hash trait

`std::hash` is not used here as it is not constexpr. To provide the `hash` trait for your own data types, simply specialize `swl::hash` like you would specialize `std::hash`. 
