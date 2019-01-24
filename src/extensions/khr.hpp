#pragma once

namespace vk 
{
    /// khr surface extension is an instance extension
    namespace khr
    {
        // surface
        using surface_t = object<VkSurfaceKHR>;
        using surface_capabilities_t = VkSurfaceCapabilitiesKHR;
        using surface_format_t = VkSurfaceFormatKHR;
        using present_mode_t = VkPresentModeKHR;
        using color_space_t = VkColorSpaceKHR;

        struct surface_properties_t
        {
            surface_capabilities_t            capabilities;
            std::vector<surface_format_t>     formats;
            std::vector<present_mode_t>       present_modes;
        };

        constexpr struct surface_ext_t
        {
            static char const* name() noexcept
            {
                return VK_KHR_SURFACE_EXTENSION_NAME;
            }
        } surface_ext;
    }

    template <typename TT, typename Base>
    class instance_extension<khr::surface_ext_t, TT, Base> : public Base
    {
        using this_type = TT;

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
        auto get_properties(physical_device_t const& device, khr::surface_t const& surface) const
            -> khr::surface_properties_t
        {
            return {
                get_capabilities(device, surface),
                get_formats(device, surface),
                get_present_modes(device, surface)
            };
        }

        auto get_capabilities(physical_device_t device, khr::surface_t const& surface) const
        {
            khr::surface_capabilities_t surface_capabilities = { 0 };
            vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface.get_object(), &surface_capabilities);
            return surface_capabilities;
        }

        auto get_formats(physical_device_t device, khr::surface_t const& surface) const
        {
            uint32_t count{ 0 };
            std::vector<khr::surface_format_t> surface_formats;
            vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.get_object(), &count, nullptr);
            if (count > 0)
            {
                surface_formats.resize(count);
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface.get_object(), &count, surface_formats.data());
            }
            return surface_formats;
        }

        auto get_present_modes(physical_device_t device, khr::surface_t const& surface) const
        {
            uint32_t count{ 0 };
            std::vector<khr::present_mode_t> present_modes;
            vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.get_object(), &count, nullptr);
            if (count > 0)
            {
                present_modes.resize(count);
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface.get_object(), &count, present_modes.data());
            }
            return present_modes;
        }

        bool get_support(physical_device_t device, khr::surface_t const& surface, uint32_t queue_index) const
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

	template <typename TT, typename Base>
	class instance_extension<khr::surface_win32_ext_t, TT, Base> : public Base
	{
        using this_type = TT;

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
			return khr::surface_t{ surface, 
                [this](VkSurfaceKHR surface) { 
                    //this->get().destroy_surface(surface);
                }
            };
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

        struct swapchain_config_t
        {
            uint32_t            present_image_count = 2;
            extent_2d_t         present_image_size;
            present_mode_t      present_mode = VK_PRESENT_MODE_FIFO_KHR;
            surface_format_t    present_image_format;
        };

        using swapchain_t = object<VkSwapchainKHR>;
	}

	template <typename TT, typename Base>
	class device_extension<khr::swapchain_ext_t, TT, Base> : public Base
	{
        using this_type = TT;

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

        auto create_swapchain(khr::surface_t const& surface, khr::swapchain_config_t const& config)
        {
            VkSwapchainCreateInfoKHR create_info = 
            {
                VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
                nullptr,
                0,
                surface.get_object(),
                config.present_image_count,
                config.present_image_format.format,
                config.present_image_format.colorSpace
            };

            VkSwapchainKHR swapchain;
            //vkCreateSwapchainKHR(this->get_device(), &create_info, nullptr, &swapchain);
            //return khr::swapchain_t{ swapchain, 
            //    [this](VkSwapchainKHR swapchain) { 
            //        vkDestroySwapchainKHR(this->get_device(), swapchain, nullptr); 
            //    } 
            //};
        }

	private:
		VULKAN_DECLARE_FUNCTION(vkCreateSwapchainKHR);
		VULKAN_DECLARE_FUNCTION(vkGetSwapchainImagesKHR);
		VULKAN_DECLARE_FUNCTION(vkAcquireNextImageKHR);
		VULKAN_DECLARE_FUNCTION(vkQueuePresentKHR);
		VULKAN_DECLARE_FUNCTION(vkDestroySwapchainKHR);
	};
}