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

#ifndef VULKAN_LOAD_DEVICE_FUNCTION
#define VULKAN_LOAD_DEVICE_FUNCTION(name) instance.load_func(VULKAN_STR2(name), name, device)
#endif

#ifndef VULKAN_EXPORT_FUNCTION
#define VULKAN_EXPORT_FUNCTION(name) export_func(VULKAN_STR2(name), name)
#endif

	// some type defines
	struct instance_param_t
	{
		std::string				app_name;
		std::string				engine_name;
	};

	class physical_device_t
	{
		template <typename, typename>
		friend class instance_extension;

		template <typename, typename>
		friend class device_extension;

	protected:
		physical_device_t(
			VkPhysicalDevice device, 
			VkPhysicalDeviceProperties properties, 
			VkPhysicalDeviceFeatures feature,
			std::vector<VkQueueFamilyProperties	>&& queue,
			std::vector<VkExtensionProperties>&& extensions)
			: device_(device)
			, device_properties_(properties)
			, device_features_(feature)
			, queue_family_properties_(std::move(queue))
			, extension_properties_(std::move(extensions))
		{}

		VkPhysicalDevice get_device() const noexcept
		{
			return device_;
		}

	public:
		physical_device_t(physical_device_t const&) = default;
		physical_device_t& operator= (physical_device_t const&) = default;
		physical_device_t(physical_device_t&&) = default;
		physical_device_t& operator= (physical_device_t&&) = default;

		auto const& features() const noexcept
		{
			return device_features_;
		}

		auto const& properties() const noexcept
		{
			return device_properties_;
		}

		auto const& queue_families() const noexcept
		{
			return queue_family_properties_;
		}

		auto const& extensions() const noexcept
		{
			return extension_properties_;
		}

		auto const& chosed_extensions() const noexcept
		{
			return chosed_extensions_;
		}

		template <typename ... Exts>
		bool choose_extensions(Exts... exts)
		{
			auto extensions = get_extension_string_array(exts...);
			if (is_extensions_satisfied(extensions, extension_properties_))
			{
				chosed_extensions_ = std::move(extensions);
				return true;
			}
			return false;
		}

	private:
		VkPhysicalDevice						device_;
		VkPhysicalDeviceProperties				device_properties_;
		VkPhysicalDeviceFeatures				device_features_;
		std::vector<VkQueueFamilyProperties	>	queue_family_properties_;
		std::vector<VkExtensionProperties>		extension_properties_;
		std::vector<char const*>				chosed_extensions_;
	};

	namespace detail
	{
		template <typename PFN_vkGetProcAddr, typename PFN_type, typename VkInstanceOrDevice>
		inline void load_funtion(PFN_vkGetProcAddr vkGetProcAddr, std::string const& proc_name, 
			PFN_type& function, VkInstanceOrDevice instance_or_device = nullptr)
		{
			assert(nullptr != vkGetProcAddr);
			auto proc_addr = vkGetProcAddr(instance_or_device, proc_name.c_str());
			if (nullptr == proc_addr)
				throw std::runtime_error{ "Failed to load " + proc_name + " from vulkan!" };

			function = reinterpret_cast<PFN_type>(proc_addr);
		}
	}

	inline bool is_extensions_satisfied(
		std::vector<char const*> const& desired,
		std::vector<VkExtensionProperties> const& available)
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

	template <typename ... Exts>
	inline auto get_extension_string_array(Exts ...) -> std::vector<char const*>
	{
		return{ Exts::name()... };
	}

	// forward declaration
	template <typename T, typename Base = null_type>
	class instance_extension;

	template <typename ... Exts>
	class instance;

	template <typename T, typename Base = null_type>
	class device_extension;

	template <typename ... Exts>
	class device;

	// raii object
	template <typename T>
	class object
	{
	public:
		template <typename F>
		object(T object, F&& deleter)
			: object_(object)
			, deleter_(std::forward<F>(deleter))
		{
		}

		~object()
		{
			deleter_(object_);
		}

		object(object const&) = delete;
		object(object&&) = default;
		object& operator= (object const&) = delete;
		object& operator= (object&&) = default;

		T get_object() const noexcept
		{
			return object_;
		}

	private:
		T						object_;
		std::function<void(T)>	deleter_;
	};

	// vulkan library
	class global
	{
		template <typename, typename> 
		friend class instance_extension;

		template <typename ...> 
		friend class instance;

	public:
		global(global const&) = delete;
		global& operator= (global const&) = delete;

		static auto& get()
		{
			static global instance;
			return instance;
		}

		template <typename ... Exts>
		auto create_instance(instance_param_t const& param, Exts...) const
		{
			using instance_t = instance<Exts...>;
			return instance_t{ *this, param };
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

		/// create the vulkan instance
		VkInstance create_instance_handle(
			instance_param_t const& param,
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
				VK_MAKE_VERSION(1, 0, 0),					// uint32_t					applicationVersion
				param.engine_name.c_str(),					// const char*				pEngineName
				VK_MAKE_VERSION(1, 0, 0),					// uint32_t					engineVersion
				VK_MAKE_VERSION(1, 0, 0)					// uint32_t					apiVersion
			};

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
		using type = instance_extension<T, typename generate_extensions_hierarchy<Rests...>::type>;
	};

	template <typename T>
	struct generate_extensions_hierarchy<T>
	{
		using type = instance_extension<T, null_type>;
	};

	template <typename ... Exts>
	using generate_extensions_hierarchy_t = typename generate_extensions_hierarchy<Exts...>::type;

	// instance 
	struct instance_core_t{};

	template <>
	class instance_extension<instance_core_t>
	{
		template <typename, typename>
		friend class device_extension;

		template <typename...>
		friend class device;

	protected:
		instance_extension(instance_extension const&) = delete;
		instance_extension& operator=(instance_extension const&) = delete;
		instance_extension(instance_extension&&) = default;
		instance_extension& operator=(instance_extension&&) = default;

		instance_extension(global const& global, VkInstance instance)
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

		~instance_extension()
		{
			if (nullptr != instance_)
				vkDestroyInstance(instance_, nullptr);
		}

		VkInstance get_instance() const noexcept
		{
			return instance_;
		}

		auto enumerate_physical_devices() const
		{
			uint32_t device_count;
			vkEnumeratePhysicalDevices(instance_, &device_count, nullptr);

			std::vector<VkPhysicalDevice> physical_devices{ device_count };
			vkEnumeratePhysicalDevices(instance_, &device_count, physical_devices.data());

			return physical_devices;
		}

		template <typename PFN_type>
		void load_function(std::string const& proc_name, PFN_type& function, VkDevice device) const
		{
			detail::load_funtion(vkGetDeviceProcAddr, proc_name, function, device);
		}

		auto enumerate_device_extensions(VkPhysicalDevice device) const
		{
			uint32_t extension_count{ 0 };
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, nullptr);
			std::vector<VkExtensionProperties> extensions{ extension_count };
			vkEnumerateDeviceExtensionProperties(device, nullptr, &extension_count, extensions.data());
			return extensions;
		}

		auto get_physical_device_features(VkPhysicalDevice device) const
		{
			VkPhysicalDeviceFeatures features;
			vkGetPhysicalDeviceFeatures(device, &features);
			return features;
		}

		auto get_physical_device_properties(VkPhysicalDevice device) const
		{
			VkPhysicalDeviceProperties properties;
			vkGetPhysicalDeviceProperties(device, &properties);
			return properties;
		}

		auto enumerate_queue_family_properties(VkPhysicalDevice device) const
		{
			uint32_t queue_family_count{ 0 };
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
			std::vector<VkQueueFamilyProperties> properties{ queue_family_count };
			vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, properties.data());
			return properties;
		}

	public:
		template <typename Filter>
		auto select_physical_device(Filter&& filter) const
		{
			// enumerate all physical device pointer
			auto all_physical_devices = enumerate_physical_devices();

			// transform them to physical device type for user
			std::list<physical_device_t> all_user_physical_devices;
			std::transform(all_physical_devices.cbegin(), all_physical_devices.cend(), 
				std::back_inserter(all_user_physical_devices), [this](auto device)
			{
				auto properties = get_physical_device_properties(device);
				auto features = get_physical_device_features(device);
				auto queue_families = enumerate_queue_family_properties(device);
				auto extensions = enumerate_device_extensions(device);
				return physical_device_t{ device, properties, features, std::move(queue_families), std::move(extensions) };
			});
			
			// find if one of the physical device satifies the filter
			auto itr = std::find_if(all_user_physical_devices.begin(), all_user_physical_devices.end(),
				[filter=std::forward<Filter>(filter)](auto& physical_device)
			{
				return filter(physical_device);
			});
			
			// throw runtime error if no physical device is found
			if (all_user_physical_devices.end() == itr)
				throw std::runtime_error{ "Cannot select an appropriate physical device!" };

			return *itr;
		}

	private:
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
		instance(instance const&) = delete;
		instance& operator=(instance const&) = delete;
		instance(instance&&) = default;
		instance& operator=(instance&&) = default;

		instance(global const& global, instance_param_t const& param)
			: instance_with_extensions(global, global.create_instance_handle(param, { Exts::name() ... }))
			, param_(param)
		{}

		instance_param_t const& get_param() const noexcept
		{
			return param_;
		}

	private:
		instance_param_t const param_;
	};

	struct KHR_surface_properties_t
	{
		VkSurfaceCapabilitiesKHR			capabilities;
		std::vector<VkSurfaceFormatKHR>	formats;
		std::vector<VkPresentModeKHR>		present_modes;
	};

	// surface
	using KHR_surface_t = object<VkSurfaceKHR>;

	// instance extensions
	constexpr struct KHR_surface_ext_t
	{
		static char const* name() noexcept
		{
			return VK_KHR_SURFACE_EXTENSION_NAME;
		}
	} KHR_surface_ext;

	template <typename Base>
	class instance_extension<KHR_surface_ext_t, Base> : public Base
	{
	protected:
		instance_extension(instance_extension const&) = delete;
		instance_extension& operator=(instance_extension const&) = delete;
		instance_extension(instance_extension&&) = default;
		instance_extension& operator=(instance_extension&&) = default;

		instance_extension(global const& global, VkInstance instance)
			: Base(global, instance)
		{
			assert(this->get_instance() == instance);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkDestroySurfaceKHR);
		}

		void destory_surface(VkSurfaceKHR surface) const
		{
			if(nullptr != surface)
				vkDestroySurfaceKHR(this->get_instance(), surface, nullptr);
		}

	public:
		auto get_surface_properties(physical_device_t const& device, KHR_surface_t const& surface) const
		{
			uint32_t count{ 0 };
			KHR_surface_properties_t properties;
			vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device.get_device(), surface.get_object(), &properties.capabilities);
			vkGetPhysicalDeviceSurfaceFormatsKHR(device.get_device(), surface.get_object(), &count, nullptr);
			if (count > 0)
			{
				properties.formats.resize(count);
				vkGetPhysicalDeviceSurfaceFormatsKHR(device.get_device(), surface.get_object(), &count, properties.formats.data());
			}
			vkGetPhysicalDeviceSurfacePresentModesKHR(device.get_device(), surface.get_object(), &count, nullptr);
			if (count > 0)
			{
				properties.present_modes.resize(count);
				vkGetPhysicalDeviceSurfacePresentModesKHR(device.get_device(), surface.get_object(), &count, properties.present_modes.data());
			}

			return properties;
		}

	private:
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
		VULKAN_DECLARE_FUNCTION(vkDestroySurfaceKHR);
	};

#ifdef VK_USE_PLATFORM_WIN32_KHR
	constexpr struct KHR_surface_win32_ext_t
	{
		static char const* name() noexcept
		{
			return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
		}
	} KHR_surface_win32_ext;

	template <typename Base>
	class instance_extension<KHR_surface_win32_ext_t, Base> : public Base
	{
	protected:
		instance_extension(instance_extension const&) = delete;
		instance_extension& operator=(instance_extension const&) = delete;
		instance_extension(instance_extension&&) = default;
		instance_extension& operator=(instance_extension&&) = default;

		instance_extension(global const& global, VkInstance instance)
			: Base(global, instance)
		{
			assert(this->get_instance() == instance);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkCreateWin32SurfaceKHR);
		}

	public:
		auto create_surface(HINSTANCE app_instance, HWND window)
		{
			VkWin32SurfaceCreateInfoKHR create_info = 
			{
				VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,			// VkStructureType                 sType
				nullptr,												// const void*					pNext
				0,													// VkWin32SurfaceCreateFlagsKHR    flags
				app_instance,											// HINSTANCE                       hinstance
				window												// HWND                            hwnd
			};

			VkSurfaceKHR surface;
			vkCreateWin32SurfaceKHR(this->get_instance(), &create_info, nullptr, &surface);
			return KHR_surface_t{ surface, [this](VkSurfaceKHR surface) { this->destory_surface(surface); } };
		}

	private:
		VULKAN_DECLARE_FUNCTION(vkCreateWin32SurfaceKHR);
	};
#endif

	// device extensions
	struct device_core_t{};

	template <>
	class device_extension<device_core_t>
	{
	protected:
		template <typename Instance>
		device_extension(Instance const& instance, VkDevice device)
			: device_(device)
		{
			VULKAN_LOAD_DEVICE_FUNCTION(vkGetDeviceQueue);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDeviceWaitIdle);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyDevice);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkGetBufferMemoryRequirements);
			VULKAN_LOAD_DEVICE_FUNCTION(vkAllocateMemory);
			VULKAN_LOAD_DEVICE_FUNCTION(vkBindBufferMemory);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdPipelineBarrier);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateImage);
			VULKAN_LOAD_DEVICE_FUNCTION(vkGetImageMemoryRequirements);
			VULKAN_LOAD_DEVICE_FUNCTION(vkBindImageMemory);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateImageView);
			VULKAN_LOAD_DEVICE_FUNCTION(vkMapMemory);
			VULKAN_LOAD_DEVICE_FUNCTION(vkFlushMappedMemoryRanges);
			VULKAN_LOAD_DEVICE_FUNCTION(vkUnmapMemory);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdCopyBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdCopyBufferToImage);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdCopyImageToBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkBeginCommandBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkEndCommandBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkQueueSubmit);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyImageView);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyImage);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkFreeMemory);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateCommandPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkAllocateCommandBuffers);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateSemaphore);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateFence);
			VULKAN_LOAD_DEVICE_FUNCTION(vkWaitForFences);
			VULKAN_LOAD_DEVICE_FUNCTION(vkResetFences);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyFence);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroySemaphore);
			VULKAN_LOAD_DEVICE_FUNCTION(vkResetCommandBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkFreeCommandBuffers);
			VULKAN_LOAD_DEVICE_FUNCTION(vkResetCommandPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyCommandPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateBufferView);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyBufferView);
			VULKAN_LOAD_DEVICE_FUNCTION(vkQueueWaitIdle);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateSampler);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateDescriptorSetLayout);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateDescriptorPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkAllocateDescriptorSets);
			VULKAN_LOAD_DEVICE_FUNCTION(vkUpdateDescriptorSets);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdBindDescriptorSets);
			VULKAN_LOAD_DEVICE_FUNCTION(vkFreeDescriptorSets);
			VULKAN_LOAD_DEVICE_FUNCTION(vkResetDescriptorPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyDescriptorPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyDescriptorSetLayout);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroySampler);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateRenderPass);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateFramebuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyFramebuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyRenderPass);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdBeginRenderPass);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdNextSubpass);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdEndRenderPass);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreatePipelineCache);
			VULKAN_LOAD_DEVICE_FUNCTION(vkGetPipelineCacheData);
			VULKAN_LOAD_DEVICE_FUNCTION(vkMergePipelineCaches);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyPipelineCache);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateGraphicsPipelines);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateComputePipelines);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyPipeline);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyEvent);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyQueryPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateShaderModule);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyShaderModule);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreatePipelineLayout);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyPipelineLayout);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdBindPipeline);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdSetViewport);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdSetScissor);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdBindVertexBuffers);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdDraw);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdDrawIndexed);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdDispatch);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdCopyImage);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdPushConstants);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdClearColorImage);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdClearDepthStencilImage);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdBindIndexBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdSetLineWidth);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdSetDepthBias);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdSetBlendConstants);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdExecuteCommands);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdClearAttachments);
		}

		~device_extension()
		{
			if (nullptr != device_)
				vkDestroyDevice(device_, nullptr);
		}

		device_extension(device_extension const&) = delete;
		device_extension& operator=(device_extension const&) = delete;
		device_extension(device_extension&&) = default;
		device_extension& operator= (device_extension&&) = default;

		VkDevice get_device() const noexcept
		{
			return device_;
		}

	private:
		VkDevice		device_;
		VULKAN_DECLARE_FUNCTION(vkGetDeviceQueue);
		VULKAN_DECLARE_FUNCTION(vkDeviceWaitIdle);
		VULKAN_DECLARE_FUNCTION(vkDestroyDevice);
		VULKAN_DECLARE_FUNCTION(vkCreateBuffer);
		VULKAN_DECLARE_FUNCTION(vkGetBufferMemoryRequirements);
		VULKAN_DECLARE_FUNCTION(vkAllocateMemory);
		VULKAN_DECLARE_FUNCTION(vkBindBufferMemory);
		VULKAN_DECLARE_FUNCTION(vkCmdPipelineBarrier);
		VULKAN_DECLARE_FUNCTION(vkCreateImage);
		VULKAN_DECLARE_FUNCTION(vkGetImageMemoryRequirements);
		VULKAN_DECLARE_FUNCTION(vkBindImageMemory);
		VULKAN_DECLARE_FUNCTION(vkCreateImageView);
		VULKAN_DECLARE_FUNCTION(vkMapMemory);
		VULKAN_DECLARE_FUNCTION(vkFlushMappedMemoryRanges);
		VULKAN_DECLARE_FUNCTION(vkUnmapMemory);
		VULKAN_DECLARE_FUNCTION(vkCmdCopyBuffer);
		VULKAN_DECLARE_FUNCTION(vkCmdCopyBufferToImage);
		VULKAN_DECLARE_FUNCTION(vkCmdCopyImageToBuffer);
		VULKAN_DECLARE_FUNCTION(vkBeginCommandBuffer);
		VULKAN_DECLARE_FUNCTION(vkEndCommandBuffer);
		VULKAN_DECLARE_FUNCTION(vkQueueSubmit);
		VULKAN_DECLARE_FUNCTION(vkDestroyImageView);
		VULKAN_DECLARE_FUNCTION(vkDestroyImage);
		VULKAN_DECLARE_FUNCTION(vkDestroyBuffer);
		VULKAN_DECLARE_FUNCTION(vkFreeMemory);
		VULKAN_DECLARE_FUNCTION(vkCreateCommandPool);
		VULKAN_DECLARE_FUNCTION(vkAllocateCommandBuffers);
		VULKAN_DECLARE_FUNCTION(vkCreateSemaphore);
		VULKAN_DECLARE_FUNCTION(vkCreateFence);
		VULKAN_DECLARE_FUNCTION(vkWaitForFences);
		VULKAN_DECLARE_FUNCTION(vkResetFences);
		VULKAN_DECLARE_FUNCTION(vkDestroyFence);
		VULKAN_DECLARE_FUNCTION(vkDestroySemaphore);
		VULKAN_DECLARE_FUNCTION(vkResetCommandBuffer);
		VULKAN_DECLARE_FUNCTION(vkFreeCommandBuffers);
		VULKAN_DECLARE_FUNCTION(vkResetCommandPool);
		VULKAN_DECLARE_FUNCTION(vkDestroyCommandPool);
		VULKAN_DECLARE_FUNCTION(vkCreateBufferView);
		VULKAN_DECLARE_FUNCTION(vkDestroyBufferView);
		VULKAN_DECLARE_FUNCTION(vkQueueWaitIdle);
		VULKAN_DECLARE_FUNCTION(vkCreateSampler);
		VULKAN_DECLARE_FUNCTION(vkCreateDescriptorSetLayout);
		VULKAN_DECLARE_FUNCTION(vkCreateDescriptorPool);
		VULKAN_DECLARE_FUNCTION(vkAllocateDescriptorSets);
		VULKAN_DECLARE_FUNCTION(vkUpdateDescriptorSets);
		VULKAN_DECLARE_FUNCTION(vkCmdBindDescriptorSets);
		VULKAN_DECLARE_FUNCTION(vkFreeDescriptorSets);
		VULKAN_DECLARE_FUNCTION(vkResetDescriptorPool);
		VULKAN_DECLARE_FUNCTION(vkDestroyDescriptorPool);
		VULKAN_DECLARE_FUNCTION(vkDestroyDescriptorSetLayout);
		VULKAN_DECLARE_FUNCTION(vkDestroySampler);
		VULKAN_DECLARE_FUNCTION(vkCreateRenderPass);
		VULKAN_DECLARE_FUNCTION(vkCreateFramebuffer);
		VULKAN_DECLARE_FUNCTION(vkDestroyFramebuffer);
		VULKAN_DECLARE_FUNCTION(vkDestroyRenderPass);
		VULKAN_DECLARE_FUNCTION(vkCmdBeginRenderPass);
		VULKAN_DECLARE_FUNCTION(vkCmdNextSubpass);
		VULKAN_DECLARE_FUNCTION(vkCmdEndRenderPass);
		VULKAN_DECLARE_FUNCTION(vkCreatePipelineCache);
		VULKAN_DECLARE_FUNCTION(vkGetPipelineCacheData);
		VULKAN_DECLARE_FUNCTION(vkMergePipelineCaches);
		VULKAN_DECLARE_FUNCTION(vkDestroyPipelineCache);
		VULKAN_DECLARE_FUNCTION(vkCreateGraphicsPipelines);
		VULKAN_DECLARE_FUNCTION(vkCreateComputePipelines);
		VULKAN_DECLARE_FUNCTION(vkDestroyPipeline);
		VULKAN_DECLARE_FUNCTION(vkDestroyEvent);
		VULKAN_DECLARE_FUNCTION(vkDestroyQueryPool);
		VULKAN_DECLARE_FUNCTION(vkCreateShaderModule);
		VULKAN_DECLARE_FUNCTION(vkDestroyShaderModule);
		VULKAN_DECLARE_FUNCTION(vkCreatePipelineLayout);
		VULKAN_DECLARE_FUNCTION(vkDestroyPipelineLayout);
		VULKAN_DECLARE_FUNCTION(vkCmdBindPipeline);
		VULKAN_DECLARE_FUNCTION(vkCmdSetViewport);
		VULKAN_DECLARE_FUNCTION(vkCmdSetScissor);
		VULKAN_DECLARE_FUNCTION(vkCmdBindVertexBuffers);
		VULKAN_DECLARE_FUNCTION(vkCmdDraw);
		VULKAN_DECLARE_FUNCTION(vkCmdDrawIndexed);
		VULKAN_DECLARE_FUNCTION(vkCmdDispatch);
		VULKAN_DECLARE_FUNCTION(vkCmdCopyImage);
		VULKAN_DECLARE_FUNCTION(vkCmdPushConstants);
		VULKAN_DECLARE_FUNCTION(vkCmdClearColorImage);
		VULKAN_DECLARE_FUNCTION(vkCmdClearDepthStencilImage);
		VULKAN_DECLARE_FUNCTION(vkCmdBindIndexBuffer);
		VULKAN_DECLARE_FUNCTION(vkCmdSetLineWidth);
		VULKAN_DECLARE_FUNCTION(vkCmdSetDepthBias);
		VULKAN_DECLARE_FUNCTION(vkCmdSetBlendConstants);
		VULKAN_DECLARE_FUNCTION(vkCmdExecuteCommands);
		VULKAN_DECLARE_FUNCTION(vkCmdClearAttachments);
	};

	// VK_KHR_SWAPCHAIN_EXTENSION
	constexpr struct KHR_swapchain_ext_t
	{
		static char const* name() noexcept
		{
			return VK_KHR_SWAPCHAIN_EXTENSION_NAME;
		}
	} KHR_swapchain_ext;

	template <typename Base>
	class device_extension<KHR_swapchain_ext_t, Base> : public Base
	{
	public:

	private:

	};
}