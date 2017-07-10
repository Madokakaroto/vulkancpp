#pragma once

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif

// standart library
#include <memory>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>

// boost library
#include <boost/dll.hpp>

#if defined _WIN32
#define VK_USE_PLATFORM_WIN32_KHR
#endif

// vulkan library
#include <vulkan/vulkan.h>

#define VULKAN_STR1(token) #token
#define VULKAN_STR2(token) VULKAN_STR1(token)

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

#if defined _WIN32
	using platform_type = platform_windows;
#elif defined __linux
	using platform_type = platform_linux;
#else
#error Unknown platform!
#endif

	// global functions
#ifndef VULKAN_DECLARE_FUNCTION
#define VULKAN_DECLARE_FUNCTION(name) PFN_##name name;
#endif

#ifndef VULKAN_LOAD_FUNCTION
#define VULKAN_LOAD_FUNCTION(name) load_func(VULKAN_STR2(name), name)
#endif

#ifndef VULKAN_LOAD_INSTNACE_FUNCTION
#define VULKAN_LOAD_INSTNACE_FUNCTION(name) global.load_func(VULKAN_STR2(name), name, instance)
#endif

#ifndef VULKAN_EXPORT_FUNCTION
#define VULKAN_EXPORT_FUNCTION(name) export_func(VULKAN_STR2(name), name)
#endif

	struct instance_param
	{
		std::string				app_name;
		std::string				engine_name;
		uint32_t					app_version;
		uint32_t					engine_version;
		uint32_t					api_version;
	};

	namespace detail
	{
		template <typename PFN_type>
		void load_funtion(PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr,
			std::string const& proc_name, PFN_type& function, VkInstance instance = nullptr)
		{
			assert(nullptr != vkGetInstanceProcAddr);
			auto proc_addr = vkGetInstanceProcAddr(instance, proc_name.c_str());
			if (nullptr == proc_addr)
				throw std::runtime_error{ "Failed to load " + proc_name + " from vulkan!" };

			function = reinterpret_cast<PFN_type>(proc_addr);
		}
	}

	template <typename T, typename Base = null_type>
	class extension;

	template <typename ... Exts>
	class instance;

	// vulkan library
	class global
	{
		template <typename, typename> friend class extension;
		template <typename ...> friend class instance;

	public:
		global(global const&) = delete;
		global& operator= (global const&) = delete;

		static auto& get()
		{
			static global instance;
			return instance;
		}

	protected:
		global()
			: library_(platform_type::dynamic_library(), boost::dll::load_mode::search_system_folders)
		{
			VULKAN_EXPORT_FUNCTION(vkGetInstanceProcAddr);
			VULKAN_LOAD_FUNCTION(vkCreateInstance);
			VULKAN_LOAD_FUNCTION(vkEnumerateInstanceExtensionProperties);
			VULKAN_LOAD_FUNCTION(vkEnumerateInstanceLayerProperties);
		}

		/// get the extension supported by vulkan
		auto get_available_extension() const
		{
			std::vector<VkExtensionProperties> extension_properties;
			uint32_t extension_count{ 0 };
			vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
			if (extension_count > 0)
			{
				extension_properties.resize(extension_count);
				vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_properties.data());
			}

			return extension_properties;
		}

		bool is_extensions_satisfied(
			std::vector<char const*> const& desired, 
			std::vector<VkExtensionProperties> const& available) const
		{
			return std::all_of(desired.cbegin(), desired.cend(), [&available](auto one_desired)
			{
				return std::find_if(available.cbegin(), available.cend(), 
					[one_desired](auto const& one_available)
				{
					return std::strcmp(one_desired, one_available.extensionName) == 0;
				}) != available.cend();
			});
		}

		/// create the vulkan instance
		VkInstance create_instance(
			instance_param const& param,
			std::vector<char const*> const& extensions) const
		{
			// enumerate all available extensions
			auto available_extensions = get_available_extension();

			// check if all desired extensions are among the available extensions
			if (!is_extensions_satisfied(extensions, available_extensions))
				throw std::runtime_error{ "Extensions are not satisfired!" };

			// create the vulkan instance
			/// fill the VkApplicationInfo struct
			VkApplicationInfo app_info = 
			{
				VK_STRUCTURE_TYPE_APPLICATION_INFO,			// VkStructureType			sType
				nullptr,                                 // const void*				pNext
				param.app_name.c_str(),					// const char*				pApplicationName
				param.app_version,						// uint32_t					applicationVersion
				param.engine_name.c_str(),					// const char*				pEngineName
				param.engine_version,						// uint32_t					engineVersion
				param.api_version							// uint32_t					apiVersion
			};

			/// transform the extension from string vector to c-sytle string vector
			//std::vector<char const*> extensions;
			//std::transform(param.extensions.begin(), param.extensions.end(), std::back_inserter(extensions),
			//	[](auto const& extension_string) { return extension_string.c_str(); });

			/// fill the VkInstanceCreateInfo struct
			VkInstanceCreateInfo create_info = 
			{
				VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,		// VkStructureType			sType
				nullptr,									// const void*				pNext
				0,										// VkInstanceCreateFlags		flags
				&app_info,								// const VkApplicationInfo*	pApplicationInfo
				0,										// uint32_t					enabledLayerCount
				nullptr,									// const char * const*		ppEnabledLayerNames
				static_cast<uint32_t>(extensions.size()),   // uint32_t					enabledExtensionCount
				extensions.data()							// const char * const*		ppEnabledExtensionNames
			};

			// call the vkCreateInstance
			VkInstance instance = nullptr;
			if(VK_SUCCESS != vkCreateInstance(&create_info, nullptr, &instance))
			{
				throw std::runtime_error{ "Failed to call vkCreateInstance!" };
			}

			return instance;
		}

		/// export function from shared library
		template <typename PFN_type>
		void export_func(std::string const& proc_name, PFN_type& function) const
		{
			if (!library_.has(proc_name))
				throw std::runtime_error{ proc_name + " not found!" };

			function = library_.get<std::remove_pointer_t<PFN_type>>(proc_name);
		}

		/// load function by calling vkGetInstanceProcAddr
		template <typename PFN_type>
		void load_func(std::string const& proc_name, PFN_type& function, VkInstance instance = nullptr) const
		{
			detail::load_funtion(vkGetInstanceProcAddr, proc_name, function, instance);
		}

	private:
		boost::dll::shared_library library_;

		/// global level funtion
		VULKAN_DECLARE_FUNCTION(vkGetInstanceProcAddr);
		VULKAN_DECLARE_FUNCTION(vkEnumerateInstanceExtensionProperties);
		VULKAN_DECLARE_FUNCTION(vkEnumerateInstanceLayerProperties);
		VULKAN_DECLARE_FUNCTION(vkCreateInstance);
	};

	// class hierarchy generation
	template <typename ... Exts>
	struct generate_extensions_hierarchy;

	template <typename T, typename ... Rests>
	struct generate_extensions_hierarchy<T, Rests...>
	{
		using type = extension<T, typename generate_extensions_hierarchy<Rests...>::type>;
	};

	template <typename T>
	struct generate_extensions_hierarchy<T>
	{
		using type = extension<T, null_type>;
	};

	template <typename ... Exts>
	using generate_extensions_hierarchy_t = typename generate_extensions_hierarchy<Exts...>::type;

	// instance 
	struct instance_core_t{};

	template <>
	class extension<instance_core_t>
	{
	protected:
		extension(extension const&) = delete;
		extension& operator=(extension const&) = delete;
		extension(extension&&) = default;
		extension& operator=(extension&&) = default;

		extension(global const& global, VkInstance instance)
			: instance_(instance)
		{
			VULKAN_LOAD_INSTNACE_FUNCTION(vkEnumeratePhysicalDevices);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkDestroyInstance);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkEnumerateDeviceExtensionProperties);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceFeatures);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceProperties);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceFormatProperties);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkCreateDevice);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetDeviceProcAddr);
		}

		~extension()
		{
			if (nullptr == instance_)
			{
				vkDestroyInstance(instance_, nullptr);
			}
		}

		VkInstance get_instance() const noexcept
		{
			return instance_;
		}

	private:
		instance_param		param_;			// parametres to create vulkan instance
		VkInstance			instance_;		// instance object

		// instance level functions
		VULKAN_DECLARE_FUNCTION(vkEnumeratePhysicalDevices);
		VULKAN_DECLARE_FUNCTION(vkDestroyInstance);
		VULKAN_DECLARE_FUNCTION(vkEnumerateDeviceExtensionProperties);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceFeatures);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceProperties);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceQueueFamilyProperties);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceMemoryProperties);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceFormatProperties);
		VULKAN_DECLARE_FUNCTION(vkCreateDevice);
		VULKAN_DECLARE_FUNCTION(vkGetDeviceProcAddr);
	};
	
	template <typename ... Exts>
	class instance 
		: public generate_extensions_hierarchy_t<Exts..., instance_core_t>
	{
		using instance_with_extensions = generate_extensions_hierarchy_t<Exts..., instance_core_t>;

	public:
		instance(global const& global, instance_param const& param)
			: instance_with_extensions(global, global.create_instance(param, { Exts::name() ... }))
			, param_(param)
		{}
	
		instance_param const& get_param() const noexcept
		{
			return param_;
		}

	private:
		instance_param const param_;
	};

	// extensions
	struct KHR_surface_t 
	{
		static char const* name() noexcept
		{
			return VK_KHR_SURFACE_EXTENSION_NAME;
		}
	};

	template <typename Base>
	class extension<KHR_surface_t, Base> : public Base
	{
	protected:
		extension(extension const&) = delete;
		extension& operator=(extension const&) = delete;

		extension(global const& global, VkInstance instance)
			: Base(global, instance)
		{
			assert(this->get_instance() == instance);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkDestroySurfaceKHR);
		}

	private:
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
		VULKAN_DECLARE_FUNCTION(vkDestroySurfaceKHR);
	};

#ifdef VK_USE_PLATFORM_WIN32_KHR
	struct KHR_surface_win32_t
	{
		static char const* name() noexcept
		{
			return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
		}
	};

	template <typename Base>
	class extension<KHR_surface_win32_t, Base> : public Base
	{
	protected:
		extension(extension const&) = delete;
		extension& operator=(extension const&) = delete;

		extension(global const& global, VkInstance instance)
			: Base(global, instance)
		{
			assert(this->get_instance() == instance);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkCreateWin32SurfaceKHR);
		}

	private:
		VULKAN_DECLARE_FUNCTION(vkCreateWin32SurfaceKHR);
	};
#endif
}