#pragma once

namespace vk
{
	namespace detail
	{
		template <typename PFN_vkGetProcAddr, typename PFN_type, typename VkInstanceOrDevice>
		inline void load_funtion(PFN_vkGetProcAddr vkGetProcAddr, std::string const& proc_name,
			PFN_type& function, VkInstanceOrDevice instance_or_device = nullptr)
		{
			assert(nullptr != vkGetProcAddr);
			auto proc_addr = vkGetProcAddr(instance_or_device, proc_name.c_str());
			if (nullptr == proc_addr)
				throw std::runtime_error{ "Failed to load " + proc_name + " from vulkan!" };

			function = reinterpret_cast<PFN_type>(proc_addr);
		}
	}

	// vulkan library
	class global_t
	{
		template <typename, typename>
		friend class instance_extension;

		template <typename ...>
		friend class instance;

	public:
		global_t(global_t const&) = delete;
		global_t& operator= (global_t const&) = delete;

		static auto& get()
		{
			static global_t instance;
			return instance;
		}

		template <typename ... Exts>
		auto create_instance(instance_param_t const& param, Exts...) const
		{
			using instance_t = instance<Exts...>;
			return instance_t{ *this, param };
		}

	protected:
		global_t()
			: library_(platform_type::dynamic_library(), boost::dll::load_mode::search_system_folders)
		{
			VULKAN_EXPORT_FUNCTION(vkGetInstanceProcAddr);
			VULKAN_LOAD_FUNCTION(vkCreateInstance);
			VULKAN_LOAD_FUNCTION(vkEnumerateInstanceExtensionProperties);
			VULKAN_LOAD_FUNCTION(vkEnumerateInstanceLayerProperties);
		}

		/// get the extension supported by vulkan
		auto get_available_extension() const
		{
			std::vector<VkExtensionProperties> extension_properties;
			uint32_t extension_count{ 0 };
			vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, nullptr);
			if (extension_count > 0)
			{
				extension_properties.resize(extension_count);
				vkEnumerateInstanceExtensionProperties(nullptr, &extension_count, extension_properties.data());
			}

			return extension_properties;
		}

		/// create the vulkan instance
		VkInstance create_instance_handle(
			instance_param_t const& param,
			std::vector<char const*> const& extensions) const
		{
			// enumerate all available extensions
			auto available_extensions = get_available_extension();

			// check if all desired extensions are among the available extensions
			if (!is_extensions_satisfied(extensions, available_extensions))
				throw std::runtime_error{ "Extensions are not satisfired!" };

			// create the vulkan instance
			/// fill the VkApplicationInfo struct
			VkApplicationInfo app_info =
			{
				VK_STRUCTURE_TYPE_APPLICATION_INFO,			// VkStructureType			sType
				nullptr,                                 // const void*				pNext
				param.app_name.c_str(),					// const char*				pApplicationName
				VK_MAKE_VERSION(1, 0, 0),					// uint32_t					applicationVersion
				param.engine_name.c_str(),					// const char*				pEngineName
				VK_MAKE_VERSION(1, 0, 0),					// uint32_t					engineVersion
				VK_MAKE_VERSION(1, 0, 0)					// uint32_t					apiVersion
			};

			/// fill the VkInstanceCreateInfo struct
			VkInstanceCreateInfo create_info =
			{
				VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,		// VkStructureType			sType
				nullptr,									// const void*				pNext
				0,										// VkInstanceCreateFlags		flags
				&app_info,								// const VkApplicationInfo*	pApplicationInfo
				0,										// uint32_t					enabledLayerCount
				nullptr,									// const char * const*		ppEnabledLayerNames
				static_cast<uint32_t>(extensions.size()),   // uint32_t					enabledExtensionCount
				extensions.data()							// const char * const*		ppEnabledExtensionNames
			};

			// call the vkCreateInstance
			VkInstance instance = nullptr;
			if (VK_SUCCESS != vkCreateInstance(&create_info, nullptr, &instance))
			{
				throw std::runtime_error{ "Failed to call vkCreateInstance!" };
			}

			return instance;
		}

		/// export function from shared library
		template <typename PFN_type>
		void export_func(std::string const& proc_name, PFN_type& function) const
		{
			if (!library_.has(proc_name))
				throw std::runtime_error{ proc_name + " not found!" };

			function = library_.get<std::remove_pointer_t<PFN_type>>(proc_name);
		}

		/// load function by calling vkGetInstanceProcAddr
		template <typename PFN_type>
		void load_func(std::string const& proc_name, PFN_type& function, VkInstance instance = nullptr) const
		{
			detail::load_funtion(vkGetInstanceProcAddr, proc_name, function, instance);
		}

	private:
		boost::dll::shared_library library_;

		/// global level funtion
		VULKAN_DECLARE_FUNCTION(vkGetInstanceProcAddr);
		VULKAN_DECLARE_FUNCTION(vkEnumerateInstanceExtensionProperties);
		VULKAN_DECLARE_FUNCTION(vkEnumerateInstanceLayerProperties);
		VULKAN_DECLARE_FUNCTION(vkCreateInstance);
	};
}