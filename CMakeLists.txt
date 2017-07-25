cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project( fastsum )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O0 -Wall ")
set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE Debug)

find_package( Boost COMPONENTS unit_test_framework REQUIRED )
include_directories( ${Boost_INCLUDE_DIRS} )

set( all_libs ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY} )

file(GLOB exe_srcs "*.cxx")
foreach( exe_src ${exe_srcs} )
    get_filename_component( exe_name ${exe_src} NAME_WE )
    add_executable( ${exe_name} ${exe_src} )
    target_link_libraries( ${exe_name} ${all_libs} )
endforeach( exe_src ${exe_srcs} )







