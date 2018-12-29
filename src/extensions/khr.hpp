#pragma once

namespace vk 
{
	/// khr surface extension is an instance extension
	namespace khr
	{
		struct surface_properties_t
		{
			VkSurfaceCapabilitiesKHR            capabilities;
			std::vector<VkSurfaceFormatKHR>     formats;
			std::vector<VkPresentModeKHR>       present_modes;
		};

		// surface
		using surface_t = object<VkSurfaceKHR>;

		constexpr struct surface_ext_t
		{
			static char const* name() noexcept
			{
				return VK_KHR_SURFACE_EXTENSION_NAME;
			}
		} surface_ext;
	}

	template <typename Base>
	class instance_extension<khr::surface_ext_t, Base> : public Base
	{
	protected:
		instance_extension(instance_extension const&) = delete;
		instance_extension& operator=(instance_extension const&) = delete;
		instance_extension(instance_extension&&) = default;
		instance_extension& operator=(instance_extension&&) = default;

		instance_extension(global_t const& global, VkInstance instance)
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
			if (nullptr != surface)
				vkDestroySurfaceKHR(this->get_instance(), surface, nullptr);
		}

	public:
		auto get_surface_properties(physical_device_t const& device, khr::surface_t const& surface) const
		{
			uint32_t count{ 0 };
			khr::surface_properties_t properties;
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

		bool get_surface_support(physical_device_t device, khr::surface_t const& surface, uint32_t queue_index) const
		{
			VkBool32 result;
			vkGetPhysicalDeviceSurfaceSupportKHR(device, queue_index, surface.get_object(), &result);
			return 0 != result;
		}

	private:
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceSupportKHR);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceCapabilitiesKHR);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfaceFormatsKHR);
		VULKAN_DECLARE_FUNCTION(vkGetPhysicalDeviceSurfacePresentModesKHR);
		VULKAN_DECLARE_FUNCTION(vkDestroySurfaceKHR);
	};


#ifdef VK_USE_PLATFORM_WIN32_KHR
#include <Windows.h>
	/// KHR win32 surface extension is an instance extension
	namespace khr
	{
		constexpr struct surface_win32_ext_t
		{
            using native_handle_t = HWND;

			static char const* name() noexcept
			{
				return VK_KHR_WIN32_SURFACE_EXTENSION_NAME;
			}
		} surface_win32_ext;
	}

	template <typename Base>
	class instance_extension<khr::surface_win32_ext_t, Base> : public Base
	{
	protected:
        using native_handle_t = khr::surface_win32_ext_t::native_handle_t;
        using app_handle_t = HINSTANCE;

		instance_extension(instance_extension const&) = delete;
		instance_extension& operator=(instance_extension const&) = delete;
		instance_extension(instance_extension&&) = default;
		instance_extension& operator=(instance_extension&&) = default;

		instance_extension(global_t const& global, VkInstance instance)
			: Base(global, instance)
		{
			assert(this->get_instance() == instance);
			VULKAN_LOAD_INSTNACE_FUNCTION(vkCreateWin32SurfaceKHR);
		}

	public:

        auto create_surface(window_t const& window)
        {
            return create_surface(window.get_app_handle(), window.get_native_handle());
        }

		auto create_surface(app_handle_t app_handle, native_handle_t window_handle)
		{
			VkWin32SurfaceCreateInfoKHR create_info =
			{
				VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR,    // VkStructureType                 sType
				nullptr,                                            // const void*                     pNext
				0,                                                  // VkWin32SurfaceCreateFlagsKHR    flags
                app_handle,                                         // HINSTANCE                       hinstance
                window_handle                                       // HWND                            hwnd
			};

			VkSurfaceKHR surface;
			vkCreateWin32SurfaceKHR(this->get_instance(), &create_info, nullptr, &surface);
			return khr::surface_t{ surface, [this](VkSurfaceKHR surface) { this->destory_surface(surface); } };
		}

	private:
		VULKAN_DECLARE_FUNCTION(vkCreateWin32SurfaceKHR);
	};
#endif

	// KHR swapchain extension is a device extension
	namespace khr
	{
		constexpr struct swapchain_ext_t
		{
			static char const* name() noexcept
			{
				return VK_KHR_SWAPCHAIN_EXTENSION_NAME;
			}
		} swapchain_ext;
	}

	template <typename Base>
	class device_extension<khr::swapchain_ext_t, Base> : public Base
	{
	public:
		template <typename Instance>
		device_extension(Instance const& instance, VkDevice device)
			: Base(instance, device)
		{
			assert(this->get_device() == device);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateSwapchainKHR);
			VULKAN_LOAD_DEVICE_FUNCTION(vkGetSwapchainImagesKHR);
			VULKAN_LOAD_DEVICE_FUNCTION(vkAcquireNextImageKHR);
			VULKAN_LOAD_DEVICE_FUNCTION(vkQueuePresentKHR);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroySwapchainKHR);

		}

	private:
		VULKAN_DECLARE_FUNCTION(vkCreateSwapchainKHR);
		VULKAN_DECLARE_FUNCTION(vkGetSwapchainImagesKHR);
		VULKAN_DECLARE_FUNCTION(vkAcquireNextImageKHR);
		VULKAN_DECLARE_FUNCTION(vkQueuePresentKHR);
		VULKAN_DECLARE_FUNCTION(vkDestroySwapchainKHR);
	};

	namespace khr
	{
		template <typename Instance>
		auto any_queue_family_is_compatible_with_surface(Instance const& instance, surface_t const& surface)
		{
			return physical_device_function([&](physical_device_t const& physical_device)
			{
				auto surface_properties = instance.get_surface_properties(physical_device, surface);
				if (surface_properties.formats.empty() || surface_properties.present_modes.empty())
					return false;

				auto const& queue_families = physical_device.queue_families();
				return std::any_of(queue_families.cbegin(), queue_families.cend(),
					[&](auto const& queue_family)
				{
					if (0 != (queue_family.properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
						0 < queue_family.properties.queueCount)
						return true;

					return instance.get_surface_support(physical_device, surface, queue_family.index);
				});
			});
		}
	}
}