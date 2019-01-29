#pragma once

namespace vk
{
    // instance  core
    struct instance_core_t {};

    template <typename T, typename TT, typename Base>
    using instance_extension_alias = instance_extension<T, TT, Base>;

    // instance wrapper
    template <typename ... Exts>
    class instance
        : public generate_extensions_hierarchy_t<instance<Exts...>, instance_extension_alias, Exts..., instance_core_t>
    {
        using instance_with_extensions = typename generate_extensions_hierarchy<instance, instance_extension_alias, Exts..., instance_core_t>::type;

    public:
        instance(instance const&) = delete;
        instance& operator=(instance const&) = delete;
        instance(instance&&) = default;
        instance& operator=(instance&&) = default;

        instance(global_t const& global, instance_param_t const& param)
            : instance_with_extensions(global, global.create_instance_handle(param, { Exts::name() ... }))
            , param_(param)
        {
        }

        instance_param_t const& get_param() const noexcept
        {
            return param_;
        }

    private:
        instance_param_t const param_;
    };

    template <typename TT>
    class instance_extension<instance_core_t, TT, null_type>
    {
        template <typename, typename, typename>
        friend class device_extension;

        template <typename...>
        friend class device;

        using this_type = TT;

    protected:
        instance_extension(instance_extension const&) = delete;
        instance_extension& operator=(instance_extension const&) = delete;
        instance_extension(instance_extension&&) = default;
        instance_extension& operator=(instance_extension&&) = default;

        instance_extension(global_t const& global, VkInstance instance)
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

        this_type const& get() const noexcept
        {
            return *static_cast<this_type const*>(this);
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
        void load_func(std::string const& proc_name, PFN_type& function, VkDevice device) const
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

        auto enumerate_queue_families(VkPhysicalDevice device) const
        {
            // enumerate all queue families properties
            uint32_t queue_family_count{ 0 };
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, nullptr);
            std::vector<VkQueueFamilyProperties> properties{ queue_family_count };
            vkGetPhysicalDeviceQueueFamilyProperties(device, &queue_family_count, properties.data());

            // transform into std::vector<queue_families>
            std::vector<queue_family_t> queue_families{ queue_family_count };
            queue_family_count = 0;
            std::transform(properties.cbegin(), properties.cend(), queue_families.begin(),
                [&queue_family_count](auto const& queue_family_properties) -> queue_family_t
            {
                return{ queue_family_count++, queue_family_properties };
            });

            return queue_families;
        }

        VkDevice create_logical_device_handle(
            physical_device_t physical_device,
            std::vector<queue_info_t> const& queue_infos,
            std::vector<char const*> const& desired_extensions,
            physical_device_features_t const& desired_features) const
        {
            std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
            queue_create_infos.reserve(queue_infos.size());
            std::transform(queue_infos.cbegin(), queue_infos.cend(), std::back_inserter(queue_create_infos),
                [](auto const& queue_info) {
                return VkDeviceQueueCreateInfo{
                    VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,             // VkStructureType                  sType
                    nullptr,                                                // const void*                      pNext
                    0,                                                      // VkDeviceQueueCreateFlags         flag
                    queue_info.family_index,                                // uint32_t                         queueFamilyIndex
                    static_cast<uint32_t>(queue_info.priorities.size()),    // uint32_t                         queueCount
                    queue_info.priorities.data()                            // const float*                     pQueuePriorities
                };
            });

            VkDeviceCreateInfo device_create_info = {
                VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO,                       // VkStructureType                  sType
                nullptr,                                                    // const void*                      pNext
                0,                                                          // VkDeviceCreateFlag               flag
                static_cast<uint32_t>(queue_create_infos.size()),           // uint32_t                         queueCreateInfoCount
                queue_create_infos.data(),                                  // const VkDeviceQueueCreateInfo*   pQueueCreateInfos
                0,                                                          // uint32_t                         enabledLayerCount
                nullptr,                                                    // char const* const*               ppEnableLayerNames
                static_cast<uint32_t>(desired_extensions.size()),           // uint32_t                         enableExtensionCount
                desired_extensions.data(),                                  // char const* const*               ppEnableExtensionNames
                &desired_features                                           // VkPhysicalDeviceFeatures const*  pEnableFeatures
            };

            VkDevice logical_device = nullptr;
            vkCreateDevice(physical_device, &device_create_info, nullptr, &logical_device);
            return logical_device;
        }

    public:
        template <typename T = physical_device_default_config_t, typename RngF>
        auto select_physical_device(RngF&& f) const
        {
            // enumerate all physical device pointer
            auto all_physical_devices = enumerate_physical_devices();

            // transform them to physical device type for user
            using select_result_t = std::vector<T>;
            select_result_t result;
            result.reserve(all_physical_devices.size());

            // create 
            std::transform(all_physical_devices.cbegin(), all_physical_devices.cend(),
                std::back_inserter(result), [this](auto device)
            {
                T t{};
                t.device = device;
                if constexpr (detail::has_physical_device_properties_v<T>)
                    t.device_properties = get_physical_device_properties(device);
                if constexpr (detail::has_physical_device_features_v<T>)
                    t.device_features = get_physical_device_features(device);
                if constexpr (detail::has_queue_families_v<T>)
                    t.queue_families = enumerate_queue_families(device);
                if constexpr (detail::has_extension_properties_v<T>)
                    t.extension_properties = enumerate_device_extensions(device);
                return t;
            });

            try
            {
                return ranges::front(f(result));
            }
            catch (...)
            {
                throw std::runtime_error{ "Failed to find a suitable physical device" };
            }
        }

        template <typename ... DeviceExts>
        auto create_logical_device(physical_device_t physical_device, std::vector<queue_info_t> const& queue_infos, DeviceExts ... device_exts) const
        {
            // prepare queue create informations
            auto device_extensions = get_extension_string_array(device_exts...);
            
            // create logical device handle
            auto logical_device_handle = create_logical_device_handle(physical_device, queue_infos, device_extensions, physical_device_features_t{});

            // create logical device
            using logical_device_t = device<DeviceExts...>;
            return logical_device_t{ *this, logical_device_handle };
        }

    private:
        VkInstance      instance_;      // instance object

        /// instance level functions
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
}