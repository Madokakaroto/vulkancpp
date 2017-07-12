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


#define VULKAN_STR1(token) #token
#define VULKAN_STR2(token) VULKAN_STR1(token)

#ifndef VULKAN_DECLARE_FUNCTION
#define VULKAN_DECLARE_FUNCTION(name) PFN_##name name;
#endif

namespace vk
{
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
		std::string				app_name;
		std::string				engine_name;
	};

#if defined _WIN32
	using platform_type = platform_windows;
#elif defined __linux
	using platform_type = platform_linux;
#else
#error Unknown platform!
#endif

	// forward declaration
	template <typename T, typename Base = null_type>
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