CMAKE_MINIMUM_REQUIRED(VERSION 3.0 FATAL_ERROR)
if(COMMAND cmake_policy)
	cmake_policy(SET CMP0003 NEW)
endif(COMMAND cmake_policy)

project(CNPY)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(/opt/cnpy/include)
link_directories(/opt/cnpy/lib)

add_executable(example1 example1.cpp)
target_link_libraries(example1 cnpy z)

add_executable(mruiz cnpy-mruiz.cpp)
target_link_libraries(mruiz cnpy z)
