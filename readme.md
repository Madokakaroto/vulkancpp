# Introduction
VulkanCpp is a modern c++ wrapper for vulkan library. Inspired by some idea from the Vulkan Specification, this library will do some experimental work on abstraction of vulkan full features and simplifying vulkan development. The library is designed to be cross platform, but now it just supports windodws desktop platform with vcpkg.

# Dependencies

 - vs2017 15.9 or later
 - vcpkg
 - vulkan sdk 1.1.82

 # Compile
 ## 1. Install boost, glfw, range-v3 and vulkan cmake support  

```
vcpkg install boost:x64-windows
vcpkg install glfw3:x64-windows
vcpkg install vulkan:x64-windows
vcpkg install range-v3:x64-windows
```

## 2. Make a _build_ directory and build
```sh
cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=${YOUR_VCPKG_PATH}/scripts/buildsystems/vcpkg.cmake
```