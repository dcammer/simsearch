cmake_minimum_required (VERSION 3.5)
project(simsearch CXX)

set(CMAKE_CXX_STANDARD 17)
find_package(fmt REQUIRED)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
 
set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -O3 -march=native")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")
set(CMAKE_CXX_STANDARD 17) # C++17
add_subdirectory(lib/Catch2)
add_subdirectory(tests)
