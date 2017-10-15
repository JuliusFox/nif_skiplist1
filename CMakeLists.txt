cmake_minimum_required(VERSION 3.8)
project(nif_skiplist)

set(CMAKE_C_STANDARD 99)

# nif required
include_directories(/opt/local/lib/erlang/usr/include)
link_directories(/opt/local/lib/erlang/usr/lib)
#set(GCC_COVERAGE_COMPILE_FLAGS "-undefined dynamic_lookup -dynamiclib")
#SET(CMAKE_C_FLAGS  "${CMAKE_C_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )

set(NOT_USED_FILE nif_skiplist.c)

set(SOURCE_FILES main.c skiplist.c nif_skiplist.c)
add_executable(nif_skiplist ${SOURCE_FILES})
