#include <optional>
#include <vulkancpp.hpp>

void init_vulkan(vk::window_t& window);

struct physical_device_config_t : 
    vk::physical_device_default_config_t
{
    vk::queue_info_t                    graphics_queue_info;
};

int main()
{
    size_t width = 1280;
    size_t height = 800;

    auto& app = vk::application_t::get();
    auto window = app.create_window(width, height, "my first vk window");

    init_vulkan(window);

    app.run(window);
    return 0;
}

void init_vulkan(vk::window_t& window)
{
    using namespace std::string_literals;

    // 1. instance vulkan global
    auto& global = vk::global_t::get();

    // 2. create vulkan instance
    vk::instance_param_t param = { "test app"s, "test engine"s };
    auto instance = global.create_instance(param, vk::khr::surface_win32_ext, vk::khr::surface_ext);

    // 3. create a surface from window
    auto surface = instance.create_surface(window);

    // 4. swapchain configuration
    vk::khr::swapchain_config_t swapchain_config{};

    // 5. filter function for selecting desired gpu
    auto select_function = 
        // 5.1. select only descrete gpus
        vk::is_discrete_gpu()
        // 5.2. select physical device that support swapchain KHR extension
        | vk::physical_device_has_extensions(vk::khr::swapchain_ext)
        // 5.3 select physical device of which the one of the queue famlilies are graphics queue and support a specific surface. 
        | vk::physical_device_pipe([&instance, &surface](auto& physical_device) {
        return ranges::any_of(physical_device.queue_families, [&](auto const& queue_family) {
            if (!instance.get_support(physical_device, surface, queue_family.index))
                return false;

            if ((queue_family.properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
                return false;

            physical_device.graphics_queue_info = { queue_family.index, { 1.0f } };
            return true;
        });
    });

    // 6. select physical device
    auto physical_device = instance.select_physical_device<physical_device_config_t>(select_function);

    // 7. create logical device with swapchain khr extension
    auto logical_device = instance.create_logical_device(
        physical_device.device,                                 // physical_device_t: the physical device
        { physical_device.graphics_queue_info },                // the information to create queue
        vk::khr::swapchain_ext                                  // extensions...
    );

    // 8. select present mode
    {
        auto present_modes = instance.get_present_modes(physical_device, surface);
        auto itr = ranges::find_if(present_modes, [](auto const& present_mode)
        {
            return present_mode == VK_FORMAT_B8G8R8A8_UNORM &&
                present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR;
        });

        if (itr != ranges::end(present_modes))
        {
            swapchain_config.present_mode = *itr;
        }
    }

    // 9. select swapchain image size
    {
        auto swapchain_capabilities = instance.get_capabilities(physical_device, surface);
        swapchain_config.present_image_size = swapchain_capabilities.currentExtent;
    }

    // 10. create swap chain
    auto swapchain = logical_device.create_swapchain(surface, swapchain_config);
}