#pragma once

#ifndef VULKAN_LOAD_DEVICE_FUNCTION
#define VULKAN_LOAD_DEVICE_FUNCTION(name) instance.load_func(VULKAN_STR2(name), name, device)
#endif

namespace vk
{
	/// physical device
	class physical_device_t
	{
		template <typename, typename>
		friend class instance_extension;

		template <typename, typename>
		friend class device_extension;

	protected:
		physical_device_t(
			VkPhysicalDevice device,
			physical_device_properties_t properties,
			physical_device_features_t feature,
			std::vector<queue_family_t>&& queue,
			std::vector<extension_properties_t>&& extensions)
			: device_(device)
			, device_properties_(properties)
			, device_features_(feature)
			, queue_families_(std::move(queue))
			, extension_properties_(std::move(extensions))
		{}

		VkPhysicalDevice get_device() const noexcept
		{
			return device_;
		}

	public:
		physical_device_t(physical_device_t const&) = default;
		physical_device_t& operator= (physical_device_t const&) = default;
		physical_device_t(physical_device_t&&) = default;
		physical_device_t& operator= (physical_device_t&&) = default;

		auto const& features() const noexcept
		{
			return device_features_;
		}

		auto const& properties() const noexcept
		{
			return device_properties_;
		}

		auto const& queue_families() const noexcept
		{
			return queue_families_;
		}

		auto const& extensions() const noexcept
		{
			return extension_properties_;
		}

		auto const& chosed_extensions() const noexcept
		{
			return chosed_extensions_;
		}

		template <typename ... Exts>
		bool choose_extensions(Exts... exts) const
		{
			auto extensions = get_extension_string_array(exts...);
			if (is_extensions_satisfied(extensions, extension_properties_))
			{
				chosed_extensions_ = std::move(extensions);
				return true;
			}
			return false;
		}

	private:
		VkPhysicalDevice						device_;
		physical_device_properties_t			device_properties_;
		physical_device_features_t				device_features_;
		std::vector<queue_family_t	>			queue_families_;
		std::vector<extension_properties_t>		extension_properties_;
		mutable std::vector<char const*>		chosed_extensions_;
	};

	/// logical device
	struct device_core_t {};

	template <>
	class device_extension<device_core_t>
	{
	protected:
		template <typename Instance>
		device_extension(Instance const& instance, VkDevice device)
			: device_(device)
		{
			VULKAN_LOAD_DEVICE_FUNCTION(vkGetDeviceQueue);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDeviceWaitIdle);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyDevice);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkGetBufferMemoryRequirements);
			VULKAN_LOAD_DEVICE_FUNCTION(vkAllocateMemory);
			VULKAN_LOAD_DEVICE_FUNCTION(vkBindBufferMemory);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdPipelineBarrier);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateImage);
			VULKAN_LOAD_DEVICE_FUNCTION(vkGetImageMemoryRequirements);
			VULKAN_LOAD_DEVICE_FUNCTION(vkBindImageMemory);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateImageView);
			VULKAN_LOAD_DEVICE_FUNCTION(vkMapMemory);
			VULKAN_LOAD_DEVICE_FUNCTION(vkFlushMappedMemoryRanges);
			VULKAN_LOAD_DEVICE_FUNCTION(vkUnmapMemory);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdCopyBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdCopyBufferToImage);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdCopyImageToBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkBeginCommandBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkEndCommandBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkQueueSubmit);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyImageView);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyImage);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkFreeMemory);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateCommandPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkAllocateCommandBuffers);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateSemaphore);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateFence);
			VULKAN_LOAD_DEVICE_FUNCTION(vkWaitForFences);
			VULKAN_LOAD_DEVICE_FUNCTION(vkResetFences);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyFence);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroySemaphore);
			VULKAN_LOAD_DEVICE_FUNCTION(vkResetCommandBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkFreeCommandBuffers);
			VULKAN_LOAD_DEVICE_FUNCTION(vkResetCommandPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyCommandPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateBufferView);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyBufferView);
			VULKAN_LOAD_DEVICE_FUNCTION(vkQueueWaitIdle);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateSampler);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateDescriptorSetLayout);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateDescriptorPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkAllocateDescriptorSets);
			VULKAN_LOAD_DEVICE_FUNCTION(vkUpdateDescriptorSets);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdBindDescriptorSets);
			VULKAN_LOAD_DEVICE_FUNCTION(vkFreeDescriptorSets);
			VULKAN_LOAD_DEVICE_FUNCTION(vkResetDescriptorPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyDescriptorPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyDescriptorSetLayout);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroySampler);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateRenderPass);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateFramebuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyFramebuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyRenderPass);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdBeginRenderPass);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdNextSubpass);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdEndRenderPass);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreatePipelineCache);
			VULKAN_LOAD_DEVICE_FUNCTION(vkGetPipelineCacheData);
			VULKAN_LOAD_DEVICE_FUNCTION(vkMergePipelineCaches);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyPipelineCache);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateGraphicsPipelines);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateComputePipelines);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyPipeline);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyEvent);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyQueryPool);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreateShaderModule);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyShaderModule);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCreatePipelineLayout);
			VULKAN_LOAD_DEVICE_FUNCTION(vkDestroyPipelineLayout);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdBindPipeline);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdSetViewport);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdSetScissor);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdBindVertexBuffers);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdDraw);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdDrawIndexed);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdDispatch);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdCopyImage);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdPushConstants);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdClearColorImage);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdClearDepthStencilImage);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdBindIndexBuffer);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdSetLineWidth);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdSetDepthBias);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdSetBlendConstants);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdExecuteCommands);
			VULKAN_LOAD_DEVICE_FUNCTION(vkCmdClearAttachments);
		}

		~device_extension()
		{
			if (nullptr != device_)
				vkDestroyDevice(device_, nullptr);
		}

		device_extension(device_extension const&) = delete;
		device_extension& operator=(device_extension const&) = delete;
		device_extension(device_extension&&) = default;
		device_extension& operator= (device_extension&&) = default;

		VkDevice get_device() const noexcept
		{
			return device_;
		}

	private:
		VkDevice		device_;
		VULKAN_DECLARE_FUNCTION(vkGetDeviceQueue);
		VULKAN_DECLARE_FUNCTION(vkDeviceWaitIdle);
		VULKAN_DECLARE_FUNCTION(vkDestroyDevice);
		VULKAN_DECLARE_FUNCTION(vkCreateBuffer);
		VULKAN_DECLARE_FUNCTION(vkGetBufferMemoryRequirements);
		VULKAN_DECLARE_FUNCTION(vkAllocateMemory);
		VULKAN_DECLARE_FUNCTION(vkBindBufferMemory);
		VULKAN_DECLARE_FUNCTION(vkCmdPipelineBarrier);
		VULKAN_DECLARE_FUNCTION(vkCreateImage);
		VULKAN_DECLARE_FUNCTION(vkGetImageMemoryRequirements);
		VULKAN_DECLARE_FUNCTION(vkBindImageMemory);
		VULKAN_DECLARE_FUNCTION(vkCreateImageView);
		VULKAN_DECLARE_FUNCTION(vkMapMemory);
		VULKAN_DECLARE_FUNCTION(vkFlushMappedMemoryRanges);
		VULKAN_DECLARE_FUNCTION(vkUnmapMemory);
		VULKAN_DECLARE_FUNCTION(vkCmdCopyBuffer);
		VULKAN_DECLARE_FUNCTION(vkCmdCopyBufferToImage);
		VULKAN_DECLARE_FUNCTION(vkCmdCopyImageToBuffer);
		VULKAN_DECLARE_FUNCTION(vkBeginCommandBuffer);
		VULKAN_DECLARE_FUNCTION(vkEndCommandBuffer);
		VULKAN_DECLARE_FUNCTION(vkQueueSubmit);
		VULKAN_DECLARE_FUNCTION(vkDestroyImageView);
		VULKAN_DECLARE_FUNCTION(vkDestroyImage);
		VULKAN_DECLARE_FUNCTION(vkDestroyBuffer);
		VULKAN_DECLARE_FUNCTION(vkFreeMemory);
		VULKAN_DECLARE_FUNCTION(vkCreateCommandPool);
		VULKAN_DECLARE_FUNCTION(vkAllocateCommandBuffers);
		VULKAN_DECLARE_FUNCTION(vkCreateSemaphore);
		VULKAN_DECLARE_FUNCTION(vkCreateFence);
		VULKAN_DECLARE_FUNCTION(vkWaitForFences);
		VULKAN_DECLARE_FUNCTION(vkResetFences);
		VULKAN_DECLARE_FUNCTION(vkDestroyFence);
		VULKAN_DECLARE_FUNCTION(vkDestroySemaphore);
		VULKAN_DECLARE_FUNCTION(vkResetCommandBuffer);
		VULKAN_DECLARE_FUNCTION(vkFreeCommandBuffers);
		VULKAN_DECLARE_FUNCTION(vkResetCommandPool);
		VULKAN_DECLARE_FUNCTION(vkDestroyCommandPool);
		VULKAN_DECLARE_FUNCTION(vkCreateBufferView);
		VULKAN_DECLARE_FUNCTION(vkDestroyBufferView);
		VULKAN_DECLARE_FUNCTION(vkQueueWaitIdle);
		VULKAN_DECLARE_FUNCTION(vkCreateSampler);
		VULKAN_DECLARE_FUNCTION(vkCreateDescriptorSetLayout);
		VULKAN_DECLARE_FUNCTION(vkCreateDescriptorPool);
		VULKAN_DECLARE_FUNCTION(vkAllocateDescriptorSets);
		VULKAN_DECLARE_FUNCTION(vkUpdateDescriptorSets);
		VULKAN_DECLARE_FUNCTION(vkCmdBindDescriptorSets);
		VULKAN_DECLARE_FUNCTION(vkFreeDescriptorSets);
		VULKAN_DECLARE_FUNCTION(vkResetDescriptorPool);
		VULKAN_DECLARE_FUNCTION(vkDestroyDescriptorPool);
		VULKAN_DECLARE_FUNCTION(vkDestroyDescriptorSetLayout);
		VULKAN_DECLARE_FUNCTION(vkDestroySampler);
		VULKAN_DECLARE_FUNCTION(vkCreateRenderPass);
		VULKAN_DECLARE_FUNCTION(vkCreateFramebuffer);
		VULKAN_DECLARE_FUNCTION(vkDestroyFramebuffer);
		VULKAN_DECLARE_FUNCTION(vkDestroyRenderPass);
		VULKAN_DECLARE_FUNCTION(vkCmdBeginRenderPass);
		VULKAN_DECLARE_FUNCTION(vkCmdNextSubpass);
		VULKAN_DECLARE_FUNCTION(vkCmdEndRenderPass);
		VULKAN_DECLARE_FUNCTION(vkCreatePipelineCache);
		VULKAN_DECLARE_FUNCTION(vkGetPipelineCacheData);
		VULKAN_DECLARE_FUNCTION(vkMergePipelineCaches);
		VULKAN_DECLARE_FUNCTION(vkDestroyPipelineCache);
		VULKAN_DECLARE_FUNCTION(vkCreateGraphicsPipelines);
		VULKAN_DECLARE_FUNCTION(vkCreateComputePipelines);
		VULKAN_DECLARE_FUNCTION(vkDestroyPipeline);
		VULKAN_DECLARE_FUNCTION(vkDestroyEvent);
		VULKAN_DECLARE_FUNCTION(vkDestroyQueryPool);
		VULKAN_DECLARE_FUNCTION(vkCreateShaderModule);
		VULKAN_DECLARE_FUNCTION(vkDestroyShaderModule);
		VULKAN_DECLARE_FUNCTION(vkCreatePipelineLayout);
		VULKAN_DECLARE_FUNCTION(vkDestroyPipelineLayout);
		VULKAN_DECLARE_FUNCTION(vkCmdBindPipeline);
		VULKAN_DECLARE_FUNCTION(vkCmdSetViewport);
		VULKAN_DECLARE_FUNCTION(vkCmdSetScissor);
		VULKAN_DECLARE_FUNCTION(vkCmdBindVertexBuffers);
		VULKAN_DECLARE_FUNCTION(vkCmdDraw);
		VULKAN_DECLARE_FUNCTION(vkCmdDrawIndexed);
		VULKAN_DECLARE_FUNCTION(vkCmdDispatch);
		VULKAN_DECLARE_FUNCTION(vkCmdCopyImage);
		VULKAN_DECLARE_FUNCTION(vkCmdPushConstants);
		VULKAN_DECLARE_FUNCTION(vkCmdClearColorImage);
		VULKAN_DECLARE_FUNCTION(vkCmdClearDepthStencilImage);
		VULKAN_DECLARE_FUNCTION(vkCmdBindIndexBuffer);
		VULKAN_DECLARE_FUNCTION(vkCmdSetLineWidth);
		VULKAN_DECLARE_FUNCTION(vkCmdSetDepthBias);
		VULKAN_DECLARE_FUNCTION(vkCmdSetBlendConstants);
		VULKAN_DECLARE_FUNCTION(vkCmdExecuteCommands);
		VULKAN_DECLARE_FUNCTION(vkCmdClearAttachments);
	};

	// high level function
	class physical_device_functor_t
	{
	public:
		template <typename F>
		explicit physical_device_functor_t(F&& f)
			: functor_(std::forward<F>(f))
		{}

		auto operator()(physical_device_t const& physical_device) const
		{
			return functor_(physical_device);
		}

	private:
		std::function<bool(physical_device_t const&)> functor_;
	};

	template <typename F>
	inline auto physical_device_function(F&& f)
	{
		return physical_device_functor_t{ [f = std::forward<F>(f)](physical_device_t const& physical_device)
		{
			return f(physical_device);
		} };
	}

	template <typename F>
	inline auto physical_device_properties_function(F&& f)
	{
		return physical_device_functor_t{ [f = std::forward<F>(f)](physical_device_t const& physical_device)
		{
			return f(physical_device.properties());
		} };
	}

	template <typename F>
	inline auto physical_device_features_function(F&& f)
	{
		return physical_device_functor_t{ [f = std::forward<F>(f)](physical_device_t const& physical_device)
		{
			return f(physical_device.features());
		} };
	}

	template <typename F>
	inline auto physical_device_extension_function(F&& f)
	{
		return physical_device_functor_t{ [f = std::forward<F>(f)](physical_device_t const& physical_device)
		{
			return f(physical_device.extensions());
		} };
	}

	template <typename F>
	inline auto physical_device_queue_families_function(F&& f)
	{
		return physical_device_functor_t{ [f = std::forward<F>(f)](physical_device_t const& physical_device)
		{
			return f(physical_device.queue_families());
		} };
	}

	class physical_device_and_operator_t
	{
	public:
		physical_device_and_operator_t(
			physical_device_functor_t const& lhs,
			physical_device_functor_t const& rhs)
			: lhs_(lhs), rhs_(rhs)
		{
		}

		auto operator() (physical_device_t const& physical_device) const
		{
			return lhs_(physical_device) && rhs_(physical_device);
		}

	private:
		physical_device_functor_t const& lhs_;
		physical_device_functor_t	const& rhs_;
	};

	class physical_device_or_operator_t
	{
	public:
		physical_device_or_operator_t(
			physical_device_functor_t const& lhs,
			physical_device_functor_t const& rhs)
			: lhs_(lhs), rhs_(rhs)
		{
		}

		auto operator() (physical_device_t const& physical_device) const
		{
			return lhs_(physical_device) || rhs_(physical_device);
		}

	private:
		physical_device_functor_t const& lhs_;
		physical_device_functor_t	const& rhs_;
	};

	inline auto operator&& (physical_device_functor_t const& lhs, physical_device_functor_t const& rhs)
	{
		return physical_device_and_operator_t{ lhs, rhs };
	}

	inline auto operator|| (physical_device_functor_t const& lhs, physical_device_functor_t const& rhs)
	{
		return physical_device_or_operator_t{ lhs, rhs };
	}

	inline auto physical_device_require_descrete_card()
	{
		return physical_device_properties_function([](auto const& properties)
		{
			return VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == properties.deviceType;
		});
	}

	template <typename ... Exts>
	inline auto physical_device_include_extensions(Exts ... extensions)
	{
		return physical_device_function([=](auto const& physical_device)
		{
			return physical_device.choose_extensions(extensions...);
		});
	}
}