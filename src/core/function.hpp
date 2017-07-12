#pragma once

namespace vk
{
	inline bool is_extensions_satisfied(
		std::vector<char const*> const& desired,
		std::vector<VkExtensionProperties> const& available)
	{
		return std::all_of(desired.cbegin(), desired.cend(), [&available](auto one_desired)
		{
			return std::find_if(available.cbegin(), available.cend(),
				[one_desired](auto const& one_available)
			{
				return std::strcmp(one_desired, one_available.extensionName) == 0;
			}) != available.cend();
		});
	}

	template <typename ... Exts>
	inline auto get_extension_string_array(Exts ...) -> std::vector<char const*>
	{
		return{ Exts::name()... };
	}
}