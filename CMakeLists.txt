cmake_minimum_required(VERSION 3.20)

project(swl_constexpr_hashmap
	VERSION 1.0.0 
	LANGUAGES CXX)

add_library(swl-constexpr-hashmap INTERFACE)

target_include_directories(swl-constexpr-hashmap INTERFACE
  $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
  $<INSTALL_INTERFACE:./>)

target_compile_features(swl-constexpr-hashmap INTERFACE cxx_std_20)

install(DIRECTORY swl TYPE INCLUDE)