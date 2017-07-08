# Introduction
VulkanCpp is a modern c++ wrapper for vulkan library. Inspired by some idea from the Vulkan Cookbook, this library will do some experimental work on simplifying vulkan development. The library is designed to be cross platform, but now it just supports windodws desktop platform.

# Dependencies

## Boost

Boost 1.61 or later. Set BOOST_INCLUDE_PATH to the directory of boost includes files, BOOST_LIB_PATH to the installed libraries directory. Make sure the directory that includes all shared libraries is in the %Path% variable.

## vulkan sdk

Make sure that vulkan-d.dll is available. I`m using Lunarg Vulkan-SDK v1.0.42.0.