#include <vulkancpp.hpp>

void init_vulkan(vk::window_t& window);

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
    auto physical_device = instance.select_physical_device(vk::requrie_descete_gpu);

    auto surpport_surface = [&surface, &instance](
        vk::physical_device_t const& physical_device, vk::queue_families_t const& queue_family) {
        return instance.get_surface_support(physical_device, surface, queue_family.front().index);
    };
}