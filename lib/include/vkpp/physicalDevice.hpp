#pragma once

#include <map>
#include <optional>

#include <vulkan/vulkan.h>

#include "queueType.hpp"


namespace vkpp
{
	class Instance;

	struct QueueInfos
	{
		std::optional<uint32_t> index;
		std::optional<uint32_t> count;
	};

	class QueueFamilyIndices
	{
		public:
			using Map = std::map<vkpp::QueueType, vkpp::QueueInfos>;

			QueueFamilyIndices();

			void set(vkpp::QueueType type, const vkpp::QueueInfos &info);

			const vkpp::QueueInfos &get(vkpp::QueueType type) const;
			const vkpp::QueueFamilyIndices::Map &get() const noexcept;
			bool hasEverything() const noexcept;

		private:
			vkpp::QueueFamilyIndices::Map m_queues;
	};

	struct SwapChainInfos
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> formats;
		std::vector<VkPresentModeKHR> presentModes;
	};

	class PhysicalDevice
	{
		public:
			PhysicalDevice(vkpp::Instance &instance);
			~PhysicalDevice();

			inline VkPhysicalDevice get() const noexcept {return m_device;}
			inline vkpp::Instance &getInstance() noexcept {return m_instance;}
			inline const vkpp::QueueFamilyIndices &getQueues() const noexcept {return m_queues;}
			inline const vkpp::SwapChainInfos &getSwapChainInfos() const noexcept {return m_swapChainInfos;}
			inline const VkPhysicalDeviceProperties &getProperties() const noexcept {return m_properties;}
			inline const VkPhysicalDeviceFeatures &getFeatures() const noexcept {return m_features;}
			inline const std::vector<const char *> &getExtensions() const noexcept {return m_extensions;}

		private:
			int s_scoreGPU(VkPhysicalDevice device, vkpp::Instance &instance, const std::vector<const char *> &extensions);
			vkpp::QueueFamilyIndices s_getQueueFamiliesIndices(VkPhysicalDevice device);
			vkpp::SwapChainInfos s_getSwapChainInfos(VkPhysicalDevice device, VkSurfaceKHR surface);
			bool s_isValidGPU(VkPhysicalDevice device, vkpp::Instance &instance, const std::vector<const char *> &extensions);

			vkpp::Instance &m_instance;
			VkPhysicalDevice m_device;
			vkpp::QueueFamilyIndices m_queues;
			vkpp::SwapChainInfos m_swapChainInfos;
			VkPhysicalDeviceProperties m_properties;
			VkPhysicalDeviceFeatures m_features;
			std::vector<const char *> m_extensions;
	};

} // namespace vkpp
