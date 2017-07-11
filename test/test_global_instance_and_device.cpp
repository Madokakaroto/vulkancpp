#include <vulkancpp.hpp>

void test_global_instance()
{
	using namespace std::string_literals;
	auto& global = vk::global::get();

	vk::instance_param param = 
	{
		"test app"s, "test engine"s,
		VK_MAKE_VERSION(1, 0, 0), 
		VK_MAKE_VERSION(1, 0, 0), 
		VK_MAKE_VERSION(1, 0, 0)
	};

	auto instance = global.create_instance(param, vk::KHR_surface_win32_ext, vk::KHR_surface_ext);
	auto surface = instance.create_surface(nullptr, nullptr);

	auto physical_device = instance.select_physical_device(
		[&instance](auto physical_device) 
	{ 
		return true; 
	}, vk::KHR_swapchain_ext);
}

int main()
{
	test_global_instance();
	return 0;
}