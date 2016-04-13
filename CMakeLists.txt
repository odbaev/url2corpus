cmake_minimum_required(VERSION 3.5)
project(url2corpus)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp UrlCorpus.cpp)
add_executable(url2corpus ${SOURCE_FILES})


target_link_libraries(url2corpus curl htmlcxx)