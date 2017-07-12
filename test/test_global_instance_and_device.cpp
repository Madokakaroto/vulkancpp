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

	//auto physical_device = instance.select_physical_device(
	//	[&instance, &surface](vk::physical_device_t& physical_device)
	//{
	//	// we want a discrete gpu
	//	auto& properties = physical_device.properties();
	//	if (properties.deviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	//		return false;
	//
	//	// all desired extensions must be satified
	//	if (!physical_device.choose_extensions(vk::khr::swapchain_ext))
	//		return false;
	//
	//	// one of the queue families must be used to draw
	//	auto& queue_families = physical_device.queue_families();
	//	uint32_t queue_index = 0;
	//	return std::any_of(queue_families.cbegin(), queue_families.cend(),
	//		[&](VkQueueFamilyProperties const& queue_family)
	//	{
	//		auto current_index = queue_index++;
	//
	//		if ((0 == (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)) || 0 == queue_family.queueCount)
	//			return false;
	//
	//		// check compatibility for surface
	//		auto surface_properties = instance.get_surface_properties(physical_device, surface);
	//		if (surface_properties.formats.empty() || surface_properties.present_modes.empty())
	//			return false;
	//
	//		if (!instance.get_surface_support(physical_device, surface, current_index))
	//			return false;
	//
	//		return true;
	//	});
	//});

	auto physical_device = instance.select_physical_device(
		vk::physical_device_require_descrete_card() && 
		vk::physical_device_include_extensions(vk::khr::swapchain_ext));
}

int main()
{
	test_global_instance();
	return 0;
}