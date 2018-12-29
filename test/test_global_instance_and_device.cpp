#include <optional>
#include <vulkancpp.hpp>

void init_vulkan(vk::window_t& window);

struct physical_device_config_t : 
    vk::physical_device_default_config_t
{
    std::optional<uint32_t>     graphics_famliy;
    std::optional<uint32_t>     queue_family;
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
    auto& global = vk::global_t::get();
    vk::instance_param_t param = { "test app"s, "test engine"s };
    auto instance = global.create_instance(param, vk::khr::surface_win32_ext, vk::khr::surface_ext);
    auto surface = instance.create_surface(window);
    auto physical_device = instance.select_physical_device<physical_device_config_t>();

    auto count = physical_device | vk::is_discrete_gpu |
        ranges::view::filter([&surface, &instance](auto& pd) -> bool
    {
        return ranges::any_of(ph.queue_families. [&pd](auto const& queue_family) 
        {

        });

        return instance.get_surface_support(pd, surface, ph.queue_families.front(), index);
    });
}