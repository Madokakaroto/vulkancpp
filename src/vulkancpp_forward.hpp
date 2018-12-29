#pragma once

// standart library
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <functional>

// boost library
#include <boost/dll.hpp>

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

#if defined _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

// vulkan library
#include <vulkan/vulkan.h>

// base library
#include "base/mpl.hpp"
#include "base/functional.hpp"

#define VULKAN_STR1(token) #token
#define VULKAN_STR2(token) VULKAN_STR1(token)

#ifndef VULKAN_DECLARE_FUNCTION
#define VULKAN_DECLARE_FUNCTION(name) PFN_##name name;
#endif

#ifndef VULKAN_LOAD_INSTNACE_FUNCTION
#define VULKAN_LOAD_INSTNACE_FUNCTION(name) global.load_func(VULKAN_STR2(name), name, instance)
#endif

#ifndef VULKAN_LOAD_DEVICE_FUNCTION
#define VULKAN_LOAD_DEVICE_FUNCTION(name) instance.load_func(VULKAN_STR2(name), name, device)
#endif

#ifndef VULKAN_EXPORT_FUNCTION
#define VULKAN_EXPORT_FUNCTION(name) export_func(VULKAN_STR2(name), name)
#endif

#ifndef VULKAN_LOAD_FUNCTION
#define VULKAN_LOAD_FUNCTION(name) load_func(VULKAN_STR2(name), name)
#endif

namespace vk
{
	// some type defines
    using physical_device_t = VkPhysicalDevice;
	using physical_device_properties_t = VkPhysicalDeviceProperties;
	using physical_device_features_t = VkPhysicalDeviceFeatures;
	using extension_property_t = VkExtensionProperties;
	using queue_family_properties_t = VkQueueFamilyProperties;

	struct null_type {};

	/// platform
	struct platform_windows
	{
		static std::string const dynamic_library() noexcept
		{
			using namespace std::string_literals;
			return "vulkan-1.dll";
		}
	};

	struct platform_linux
	{
		static std::string const dynamic_library() noexcept
		{
			using namespace std::string_literals;
			return "libvulkan.so.1";
		}
	};

    // some type defines
    struct instance_param_t
    {
        std::string                 app_name;
        std::string                 engine_name;
    };

    struct queue_family_t
    {
        uint32_t                    index;
        queue_family_properties_t   properties;
    };

    struct queue_info_t
    {
        uint32_t                    family_index;
        std::vector<float>          priorities;
    };

    using queue_families_t = std::vector<queue_family_t>;
    using extension_properties_t = std::vector<extension_property_t>;

#if defined _WIN32
	using platform_type = platform_windows;
#elif defined __linux
	using platform_type = platform_linux;
#else
#error Unknown platform!
#endif

	// forward declaration
	template <typename T, typename Base>
    class instance_extension;

	template <typename ... Exts>
	class instance;

	template <typename T, typename Base = null_type>
    class device_extension;

	template <typename ... Exts>
	class device;

	template <typename T>
	class object;

	class global_t;
}