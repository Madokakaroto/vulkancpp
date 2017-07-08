#include <vulkancpp.hpp>

void test_global_instance()
{
	using namespace std::string_literals;
	using instance_t = vk::instance<vk::KHR_surface_win32, vk::KHR_surface>;

	auto& global = vk::global::get();

	vk::instance_param param = 
	{
		"test app"s, "test engine"s,
		VK_MAKE_VERSION(1, 0, 0), 
		VK_MAKE_VERSION(1, 0, 0), 
		VK_MAKE_VERSION(1, 0, 0)
	};

	instance_t instance{ global, param };
}

int main()
{
	test_global_instance();
	return 0;
}