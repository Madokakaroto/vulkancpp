#pragma once

namespace vk
{
    /// physical device
    struct physical_device_config_t
    {
        physical_device_t                       physical_device;
        physical_device_properties_t            physical_device_properties;
        physical_device_features_t              physical_device_features;
        queue_families_t                        queue_families;
        extension_properties_t                  extension_properties;
    };

	namespace detail
	{
        template <typename T, typename = void>
        struct has_physical_device : std::false_type {};
        template <typename T>
        struct has_physical_device<T, std::enable_if_t<std::is_same_v<VkPhysicalDevice,
            decltype(std::declval<T>().physical_device)>>> : std::true_type {};
        template <typename T>
        inline constexpr bool has_physical_device_v = has_physical_device<T>::value;

        template <typename T, typename = void>
        struct has_physical_device_properties : std::false_type {};
        template <typename T>
        struct has_physical_device_properties<T, std::enable_if_t<std::is_same_v<physical_device_properties_t,
            decltype(std::declval<T>().physical_device_properties)>>> : std::true_type {};
        template <typename T>
        inline constexpr bool has_physical_device_properties_v = has_physical_device_properties<T>::value;

        template <typename T, typename = void>
        struct has_physical_device_features : std::false_type {};
        template <typename T>
        struct has_physical_device_features<T, std::enable_if_t<std::is_same_v<physical_device_features_t,
            decltype(std::declval<T>().physical_device_features)>>> : std::true_type {};
        template <typename T>
        inline constexpr bool has_physical_device_features_v = has_physical_device_features<T>::value;

        template <typename T, typename = void>
        struct has_queue_families : std::false_type {};
        template <typename T>
        struct has_queue_families<T, std::enable_if_t<std::is_same_v<queue_families_t,
            decltype(std::declval<T>().queue_family)>>> : std::true_type {};
        template <typename T>
        inline constexpr bool has_queue_families_v = has_queue_families<T>::value;

        template <typename T, typename = void>
        struct has_extension_properties : std::false_type {};
        template <typename T>
        struct has_extension_properties<T, std::enable_if_t<std::is_same_v<extension_properties_t, 
            decltype(std::declval<T>.extension_properties)>>> : std::true_type {};
        template <typename T>
        inline constexpr bool has_extension_properties_v = has_extension_properties<T>::value;
	}

	template <typename T>
	class physical_device_select_result_t
	{
        template <typename, typename>
        friend class instance_extension;

    public:
        static_assert(detail::has_physical_device_v<T>);
        using select_result_t = std::vector<T>;
        using element_type = T;

    protected:
        physical_device_select_result_t(select_result_t result)
            : select_result_(std::move(result))
        {
        }

        physical_device_select_result_t(physical_device_select_result_t const&) = delete;
        physical_device_select_result_t& operator=(physical_device_select_result_t const&) = delete;
        physical_device_select_result_t(physical_device_select_result_t&&) = default;
        physical_device_select_result_t& operator=(physical_device_select_result_t&&) = default;

    public:
        //template <typename F>

        auto count() const
        {
            return ranges::all_of(select_result_, [](auto const& r)
            {
                return r.physical_device_properties.deviceType == 
                    VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
            });
        }

    private:
        select_result_t select_result_;
	};
}