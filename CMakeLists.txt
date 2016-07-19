cmake_minimum_required (VERSION 2.8)
project (final)

find_package(Threads)

include_directories(src)

set(CMAKE_CXX_FLAGS -std=c++11)
#set(CMAKE_CXX_FLAGS -std=c++11 -Wall -Wpedantic -Weffc++ -Woverloaded-virtual)

set(SOURCES src/main.cpp
            src/server.cpp
            src/unique_fd.cpp
            src/options.cpp
            )

add_executable(final ${SOURCES})
target_link_libraries (final ${CMAKE_THREAD_LIBS_INIT})

