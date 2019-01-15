#include <optional>
#include <vulkancpp.hpp>

void init_vulkan(vk::window_t& window);

struct physical_device_config_t : 
    vk::physical_device_default_config_t
{
    vk::queue_info_t            graphics_queue_info;
    vk::extent_2d_t             present_image_size;
    vk::khr::present_mode_t     present_mode;
    vk::khr::surface_format_t   desired_format;
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

    // instance vulkan global
    auto& global = vk::global_t::get();

    // create vulkan instance
    vk::instance_param_t param = { "test app"s, "test engine"s };
    auto instance = global.create_instance(param, vk::khr::surface_win32_ext, vk::khr::surface_ext);

    // create a surface from window
    auto surface = instance.create_surface(window);

    // filter function for selecting desired gpu
    auto select_function = vk::is_discrete_gpu() 
        | vk::physical_device_has_extensions(vk::khr::swapchain_ext)
        | vk::physical_device_pipe([&instance, &surface](auto& physical_device) {
        return ranges::any_of(physical_device.queue_families, [&](auto const& queue_family) {
            if (!instance.get_support(physical_device, surface, queue_family.index))
                return false;

            if ((queue_family.properties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
                return false;

            physical_device.graphics_queue_info = { queue_family.index, { 1.0f } };
            return true;
        });
    }) | vk::physical_device_pipe([&instance, &surface](auto& physical_device) {

        return true;
    });

    // select gpu
    auto physical_device = instance.select_physical_device<physical_device_config_t>(select_function);

    // create logical device
    auto logical_device = instance.create_logical_device(
        physical_device.device,                                 // physical_device_t: the physical device
        { physical_device.graphics_queue_info },                // the information to create queue
        vk::khr::swapchain_ext                                  // extensions...
    );
}