#include <vulkancpp.hpp>

void test_global_instance()
{
	using namespace std::string_literals;
	auto& global = vk::global::get();

	vk::instance_param_t param = 
	{
		"test app"s, "test engine"s
	};

	auto instance = global.create_instance(param, vk::KHR_surface_win32_ext, vk::KHR_surface_ext);
	auto surface = instance.create_surface(nullptr, nullptr);

	auto physical_device = instance.select_physical_device(
		[&instance, &surface](vk::physical_device_t& physical_device)
	{
		// we want a discrete gpu
		auto& properties = physical_device.properties();
		if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			return false;

		// all desired extensions must be satified
		if (!physical_device.choose_extensions(vk::KHR_swapchain_ext))
			return false;

		// one of the queue families must be used to draw
		auto& queue_families = physical_device.queue_families();
		if (std::none_of(queue_families.cbegin(), queue_families.cend(),
			[](VkQueueFamilyProperties const& queue_family)
		{
			return (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
				queue_family.queueCount > 0;
		}))
			return false;

		// check compatibility for surface
		auto surface_properties = instance.get_surface_properties(physical_device, surface);
		if (surface_properties.formats.empty() || surface_properties.present_modes.empty())
			return false;

		return true;
	});
}

int main()
{
	test_global_instance();
	return 0;
}