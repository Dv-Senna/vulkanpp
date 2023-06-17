#include <iostream>
#include <stdexcept>
#include <vector>

#include "device.hpp"



namespace vkpp
{
	Device::Device(vkpp::PhysicalDevice &physicalDevice) :
		m_instance {physicalDevice.getInstance()},
		m_physicalDevice {physicalDevice},
		m_device {VK_NULL_HANDLE},
		m_queues {}
	{
		float priority {1.0f};

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos {};
		queueCreateInfos.reserve(vkpp::QUEUE_TYPE_AMOUNT);


		std::vector<uint32_t> usedQueues {};


		for (auto queueInfo : m_physicalDevice.getQueues().get())
		{
			bool addValue {true};

			for (auto usedQueue : usedQueues)
			{
				if (usedQueue == queueInfo.second.index.value())
				{
					addValue = false;
					break;
				}
			}

			if (!addValue)
				continue;

			queueCreateInfos.push_back({});
			(queueCreateInfos.end() - 1)->sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			(queueCreateInfos.end() - 1)->queueFamilyIndex = queueInfo.second.index.value();
			(queueCreateInfos.end() - 1)->queueCount = 1;
			(queueCreateInfos.end() - 1)->pQueuePriorities = &priority;

			usedQueues.push_back(queueInfo.second.index.value());
		}
		

		VkPhysicalDeviceFeatures wantedFeatures {};

		VkDeviceCreateInfo deviceCreateInfo {};
		deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t> (m_physicalDevice.getExtensions().size());
		deviceCreateInfo.ppEnabledExtensionNames = m_physicalDevice.getExtensions().data();
		deviceCreateInfo.enabledLayerCount = 0;
		deviceCreateInfo.ppEnabledLayerNames = nullptr;
		deviceCreateInfo.pEnabledFeatures = &wantedFeatures;
		deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t> (queueCreateInfos.size());
		deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

		if (vkCreateDevice(m_physicalDevice.get(), &deviceCreateInfo, nullptr, &m_device) != VK_SUCCESS)
			throw std::runtime_error("VKPP : Can't create a logical device");


		for (uint32_t i {0}; i < vkpp::QUEUE_TYPE_AMOUNT; i++)
		{
			m_queues[static_cast<vkpp::QueueType> (i)] = {};
			vkGetDeviceQueue(
				m_device,
				m_physicalDevice.getQueues().get(static_cast<vkpp::QueueType> (i)).index.value(),
				0,
				&m_queues[static_cast<vkpp::QueueType> (i)]
			);
		}
	}



	Device::~Device()
	{
		vkDestroyDevice(m_device, nullptr);
	}



} // namespace vkpp
