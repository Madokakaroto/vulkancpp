#pragma once

namespace vk
{
    /// physical device
    class physical_device_t
    {
        template <typename, typename>
        friend class instance_extension;

        template <typename, typename>
        friend class device_extension;

    protected:
        physical_device_t(
            VkPhysicalDevice device,
            physical_device_properties_t properties,
            physical_device_features_t feature,
            queue_families_t&& queue,
            extension_properties_t&& extensions)
            : device_(device)
            , device_properties_(properties)
            , device_features_(feature)
            , queue_families_(std::move(queue))
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
            return queue_families_;
        }

        auto const& extensions() const noexcept
        {
            return extension_properties_;
        }

    private:
        VkPhysicalDevice                        device_;
        physical_device_properties_t            device_properties_;
        physical_device_features_t              device_features_;
        queue_families_t                        queue_families_;
        extension_properties_t                  extension_properties_;
    };

    // for physical device chooser
    namespace detail
    {
        template <typename T>
        struct pdc_helper;

        // get device propert
        template <>
        struct pdc_helper<physical_device_properties_t>
        {
            static auto& get(physical_device_t const& physical_device)
            {
                return physical_device.properties();
            }
        };

        // get feature
        template <>
        struct pdc_helper<physical_device_features_t>
        {
            static auto& get(physical_device_t const& physical_device)
            {
                return physical_device.features();
            }
        };

        // get queue families
        template <>
        struct pdc_helper<queue_families_t>
        {
            static auto& get(physical_device_t const& physical_device)
            {
                return physical_device.queue_families();
            }
        };

        // extensions
        template <>
        struct pdc_helper<extension_properties_t>
        {
            static auto& get(physical_device_t const& physical_device)
            {
                return physical_device.extensions();
            }
        };

        template <>
        struct pdc_helper<physical_device_t>
        {
            static auto& get(physical_device_t const physical_device)
            {
                return physical_device;
            }
        };

        template <size_t Index, typename ArgsPack>
        using pdc_arg_type = std::remove_cv_t<std::remove_reference_t<std::tuple_element_t<Index, ArgsPack>>>;

        template <typename ArgsPack, typename Func, size_t ... Is>
        inline constexpr static auto bind_physical_device_chooser(Func&& func, std::index_sequence<Is...>) noexcept
        {
            return [funciton = std::forward<Func>(func)]
                (physical_device_t const& physical_device) noexcept // todo better noexcept()
            {
                return funciton(pdc_helper<pdc_arg_type<Is, ArgsPack>>::get(physical_device)...);
            };
        }

        template <typename Func>
        inline constexpr static auto bind_physical_device_chooser(Func&& func) noexcept
        {
            using callable_traits_t = callable_traits<std::remove_reference_t<Func>>;
            return detail::bind_physical_device_chooser<typename callable_traits_t::args_pack>(
                std::forward<Func>(func), std::make_index_sequence<callable_traits_t::arity>{});
        }
    }

    inline const auto requrie_descete_gpu = detail::bind_physical_device_chooser(
        [](physical_device_properties_t const& pdp) {
        return VkPhysicalDeviceType::VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == pdp.deviceType;
    });
}