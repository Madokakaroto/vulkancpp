#include <vulkancpp.hpp>

void test_global_instance()
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
}

int main()
{
	test_global_instance();
	return 0;
}