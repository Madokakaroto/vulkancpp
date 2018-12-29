#pragma once

namespace vk
{
    /// physical device
    struct physical_device_default_config_t
    {
        operator physical_device_t() { return device; }

        physical_device_t                       device;
        physical_device_properties_t            device_properties;
        physical_device_features_t              device_features;
        queue_families_t                        queue_families;
        extension_properties_t                  extension_properties;
    };

	namespace detail
	{
        template <typename T, typename = void>
        struct has_physical_device : std::false_type {};
        template <typename T>
        struct has_physical_device<T, std::enable_if_t<std::is_same_v<VkPhysicalDevice,
            decltype(std::declval<T>().device)>>> : std::true_type {};
        template <typename T>
        inline constexpr bool has_physical_device_v = has_physical_device<T>::value;

        template <typename T, typename = void>
        struct has_physical_device_properties : std::false_type {};
        template <typename T>
        struct has_physical_device_properties<T, std::enable_if_t<std::is_same_v<physical_device_properties_t,
            decltype(std::declval<T>().device_properties)>>> : std::true_type {};
        template <typename T>
        inline constexpr bool has_physical_device_properties_v = has_physical_device_properties<T>::value;

        template <typename T, typename = void>
        struct has_physical_device_features : std::false_type {};
        template <typename T>
        struct has_physical_device_features<T, std::enable_if_t<std::is_same_v<physical_device_features_t,
            decltype(std::declval<T>().device_features)>>> : std::true_type {};
        template <typename T>
        inline constexpr bool has_physical_device_features_v = has_physical_device_features<T>::value;

        template <typename T, typename = void>
        struct has_queue_families : std::false_type {};
        template <typename T>
        struct has_queue_families<T, std::enable_if_t<std::is_same_v<queue_families_t,
            decltype(std::declval<T>().queue_families)>>> : std::true_type {};
        template <typename T>
        inline constexpr bool has_queue_families_v = has_queue_families<T>::value;

        template <typename T, typename = void>
        struct has_extension_properties : std::false_type {};
        template <typename T>
        struct has_extension_properties<T, std::enable_if_t<std::is_same_v<extension_properties_t, 
            decltype(std::declval<T>().extension_properties)>>> : std::true_type {};
        template <typename T>
        inline constexpr bool has_extension_properties_v = has_extension_properties<T>::value;
	}

	/*template <typename T>
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
	};*/

    inline static auto is_discrete_gpu() noexcept
    {
        return ranges::view::filter(
            [](auto& physical_device) -> bool {
            return physical_device.device_properties.deviceType ==
                VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU;
        });
    }

    template <typename Pred, typename ... Args>
    inline static auto physical_device_pipe(Pred const& pred, Args&& ... args) noexcept
    {
        return ranges::view::filter([&](auto& physical_device) -> bool {
            return pred(physical_device, std::forward<Args>(args)...);
        });
    }
}