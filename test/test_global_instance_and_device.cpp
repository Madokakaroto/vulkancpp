#include <vulkancpp.hpp>

/*void test_global_instance()
{
	using namespace std::string_literals;
	auto& global = vk::global_t::get();

	vk::instance_param_t param = 
	{
		"test app"s, "test engine"s
	};

	auto instance = global.create_instance(param, vk::khr::surface_win32_ext, vk::khr::surface_ext);
	auto surface = instance.create_surface(nullptr, nullptr);

	auto physical_device = instance.select_physical_device(
		vk::physical_device_require_descrete_card() && 
		vk::physical_device_include_extensions(vk::khr::swapchain_ext) &&
		vk::khr::any_queue_family_is_compatible_with_surface(instance, surface)
	);
}*/

void init_vulkan(vk::application_t const& app, vk::window_t const& window);

int main()
{
    size_t width = 1280;
    size_t height = 800;

    auto& app = vk::application_t::get();
    auto window = app.create_window(width, height, "my first vk window");

    init_vulkan(app, window);

    app.run(window);
	return 0;
}

void init_vulkan(vk::application_t const& app, vk::window_t const& window)
{
    using namespace std::string_literals;
    auto& global = vk::global_t::get();
    vk::instance_param_t param =
    {
        "test app"s, "test engine"s
    };

    auto instance = global.create_instance(param, vk::khr::surface_win32_ext, vk::khr::surface_ext);
    auto surface = instance.create_surface(window);
}