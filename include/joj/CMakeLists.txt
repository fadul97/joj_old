# PATH: include/joj/CMakeLists.txt

cmake_minimum_required(VERSION 3.22)
project(joj)

add_subdirectory(core)
add_subdirectory(platform)
add_subdirectory(renderer)
add_subdirectory(math)
add_subdirectory(systems)
add_subdirectory(vendor)
add_subdirectory(application)
add_subdirectory(events)
add_subdirectory(test)
add_subdirectory(eap)
add_subdirectory(utils)
add_subdirectory(resources)