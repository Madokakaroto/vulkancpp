cmake_minimum_required(VERSION 3.0)
project(vulkancpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(VULKANCPP_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Boost COMPONENTS filesystem REQUIRED)

include_directories(
	${VULKANCPP_DIR}/src
)

set(VULKANCPP_UNIT_TEST
    ${VULKANCPP_DIR}/test/test_global_instance_and_device.cpp
)

add_executable(bk_test ${VULKANCPP_UNIT_TEST})
target_link_libraries(bk_test ${Boost_LIBRARIES})