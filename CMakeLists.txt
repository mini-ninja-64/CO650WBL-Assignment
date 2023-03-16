cmake_minimum_required (VERSION 3.15)

include(cmake/helpers.cmake)
setup_google_test()

set(CMAKE_CXX_STANDARD 20)
project ("CO650WBL Assignment")

add_subdirectory("Task1")
#add_subdirectory("Task2")