#pragma once

#include <map>

#include <vulkan/vulkan.h>

#include "physicalDevice.hpp"


namespace vkpp
{
	class Device
	{
		public:
			Device(vkpp::PhysicalDevice &physicalDevice);
			~Device();

		
		private:
			vkpp::Instance &m_instance;
			vkpp::PhysicalDevice &m_physicalDevice;
			VkDevice m_device;
			std::map<vkpp::QueueType, VkQueue> m_queues;
	};


} // namespace vkpp
