cmake_minimum_required(VERSION 3.1)
project(vulkancpp)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++17")

if(CMAKE_TOOLCHAIN_FILE)
include(${CMAKE_TOOLCHAIN_FILE})
endif(CMAKE_TOOLCHAIN_FILE)

if(MSVC)
add_compile_options (/permissive-)
endif(MSVC)

set(VULKANCPP_DIR ${CMAKE_CURRENT_SOURCE_DIR})

find_package(Boost COMPONENTS filesystem REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(range-v3 CONFIG REQUIRED)
find_package(vulkan REQUIRED)

# include_directories(
#    ${VULKANCPP_DIR}/src
#    ${Vulkan_INCLUDE_DIRS}
# )

set(VULKAN_CPP_HEADERS 
    ${VULKANCPP_DIR}/src/vulkancpp.hpp
    ${VULKANCPP_DIR}/src/vulkancpp_forward.hpp
    ${VULKANCPP_DIR}/src/application/application.hpp
    ${VULKANCPP_DIR}/src/application/platform.hpp
    ${VULKANCPP_DIR}/src/application/window.hpp
    ${VULKANCPP_DIR}/src/base/functional.hpp
    ${VULKANCPP_DIR}/src/base/mpl.hpp
    ${VULKANCPP_DIR}/src/core/device.hpp
    ${VULKANCPP_DIR}/src/core/function.hpp
    ${VULKANCPP_DIR}/src/core/global.hpp
    ${VULKANCPP_DIR}/src/core/instance.hpp
    ${VULKANCPP_DIR}/src/core/object.hpp
    ${VULKANCPP_DIR}/src/core/physical_device.hpp
    ${VULKANCPP_DIR}/src/extensions/khr.hpp
)

set(VULKANCPP_UNIT_TEST
    ${VULKANCPP_DIR}/test/test_global_instance_and_device.cpp
)

add_library(vulkancpp INTERFACE)
target_sources(vulkancpp INTERFACE ${VULKAN_CPP_HEADERS})
target_include_directories(vulkancpp INTERFACE
    ${VULKANCPP_DIR}/src/ 
    ${Vulkan_INCLUDE_DIRS}
)

add_executable(bk_test ${VULKANCPP_UNIT_TEST})
target_link_libraries(bk_test PRIVATE ${Boost_LIBRARIES} glfw meta range-v3 vulkancpp)