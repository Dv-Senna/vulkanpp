#include <iostream>
#include <stdexcept>
#include <vector>

#include "physicalDevice.hpp"
#include "instance.hpp"
#include "utils/max.hpp"


namespace vkpp
{
	QueueFamilyIndices::QueueFamilyIndices() : m_queues {}
	{
		for (uint32_t i {0}; i < vkpp::QUEUE_TYPE_AMOUNT; i++)
			m_queues[static_cast<vkpp::QueueType> (i)] = {};
	}



	void QueueFamilyIndices::set(vkpp::QueueType type, const vkpp::QueueInfos &info)
	{
		m_queues[type] = info;
	}



	const vkpp::QueueInfos &QueueFamilyIndices::get(vkpp::QueueType type) const
	{
		auto it {m_queues.find(type)};
		if (it == m_queues.end())
			throw std::runtime_error("VKPP : Queue of type " + std::to_string(static_cast<int> (type)) + " can't be found");

		return it->second;
	}



	const vkpp::QueueFamilyIndices::Map &QueueFamilyIndices::get() const noexcept
	{
		return m_queues;
	}



	bool QueueFamilyIndices::hasEverything() const noexcept
	{
		bool result {true};

		for (auto it : m_queues)
			result = result && it.second.index.has_value() && it.second.count.has_value();

		return result;
	}



	PhysicalDevice::PhysicalDevice(vkpp::Instance &instance) : 
		m_instance {instance},
		m_device {VK_NULL_HANDLE},
		m_queues {},
		m_properties {},
		m_features {},
		m_extensions {
			VK_KHR_SWAPCHAIN_EXTENSION_NAME
		}
	{
		m_extensions.insert(
			m_extensions.end(),
			std::make_move_iterator(m_instance.getParameters().deviceExtensions.begin()),
			std::make_move_iterator(m_instance.getParameters().deviceExtensions.end())
		);


		uint32_t devicesCount {};
		if (vkEnumeratePhysicalDevices(m_instance.get(), &devicesCount, nullptr) != VK_SUCCESS)
			throw std::runtime_error("VKPP : Can't get physical devices count");

		if (devicesCount == 0)
			throw std::runtime_error("VKPP : No GPU support Vulkan");

		std::vector<VkPhysicalDevice> devices {devicesCount};
		if (vkEnumeratePhysicalDevices(m_instance.get(), &devicesCount, devices.data()) != VK_SUCCESS)
			throw std::runtime_error("VKPP : Can't get physical devices");

		std::vector<int> scores {};
		scores.reserve(devicesCount);

		for (auto device : devices)
			scores.push_back(s_scoreGPU(device, m_extensions));

		auto bestScore {vkpp::utils::max(scores.begin(), scores.end())};
		if (*bestScore <= 0)
			throw std::runtime_error("VKPP : No GPU is suitable for needed use");

		uint32_t index {static_cast<uint32_t> (bestScore - scores.begin())};
		m_device = devices[index];

		m_queues = s_getQueueFamiliesIndices(m_device);
		vkGetPhysicalDeviceProperties(m_device, &m_properties);
		vkGetPhysicalDeviceFeatures(m_device, &m_features);


		#ifndef NDEBUG

			std::clog << "Choosen physical device : " << m_properties.deviceName << " [" << m_properties.deviceID << "]" << std::endl;

			uint32_t supportedExtensionsCount {};
			if (vkEnumerateDeviceExtensionProperties(m_device, nullptr, &supportedExtensionsCount, nullptr) != VK_SUCCESS)
				return;

			std::vector<VkExtensionProperties> supportedExtensions {supportedExtensionsCount};
			if (vkEnumerateDeviceExtensionProperties(m_device, nullptr, &supportedExtensionsCount, supportedExtensions.data()) != VK_SUCCESS)
				return;

			std::clog << "This device support the following extensions : " << std::endl;

			for (auto supported : supportedExtensions)
				std::clog << "\t" << supported.extensionName << std::endl;

		#endif
	}



	PhysicalDevice::~PhysicalDevice()
	{

	}



	int PhysicalDevice::s_scoreGPU(VkPhysicalDevice device, const std::vector<const char *> &extensions)
	{
		int score {0};

		if (!s_isValidGPU(device, extensions))
			return -1;

		VkPhysicalDeviceProperties properties {};
		vkGetPhysicalDeviceProperties(device, &properties);

		VkPhysicalDeviceFeatures features {};
		vkGetPhysicalDeviceFeatures(device, &features);

		if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
			score += 100;

		return score;
	}



	vkpp::QueueFamilyIndices PhysicalDevice::s_getQueueFamiliesIndices(VkPhysicalDevice device)
	{
		QueueFamilyIndices indices {};

		uint32_t queueCount {};
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, nullptr);

		std::vector<VkQueueFamilyProperties> queues {queueCount};
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueCount, queues.data());

		for (uint32_t i {0}; i < queueCount; i++)
		{
			if (queues[i].queueFlags & VK_QUEUE_GRAPHICS_BIT)
				indices.set(vkpp::QueueType::graphics, {i, queues[i].queueCount});

			VkBool32 presentSupport {static_cast<VkBool32> (false)};
			if (vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_instance.getSurface(), &presentSupport) != VK_SUCCESS)
				throw std::runtime_error("VKPP : Can't get availability of present of queue " + std::to_string(i));

			if (presentSupport)
				indices.set(vkpp::QueueType::present, {i, queues[i].queueCount});

			if (indices.hasEverything())
				return indices;
		}

		return indices;
	}



	bool PhysicalDevice::s_isValidGPU(VkPhysicalDevice device, const std::vector<const char *> &extensions)
	{
		uint32_t supportedExtensionsCount {};
		if (vkEnumerateDeviceExtensionProperties(device, nullptr, &supportedExtensionsCount, nullptr) != VK_SUCCESS)
			throw std::runtime_error("VKPP : Can't get supported device extensions count");

		std::vector<VkExtensionProperties> supportedExtensions {supportedExtensionsCount};
		if (vkEnumerateDeviceExtensionProperties(device, nullptr, &supportedExtensionsCount, supportedExtensions.data()) != VK_SUCCESS)
			throw std::runtime_error("VKPP : Can't get supported device extensions");

		for (auto extension : extensions)
		{
			bool isSupported {false};

			for (auto supported : supportedExtensions)
			{
				if (strcmp(extension, supported.extensionName) == 0)
				{
					isSupported = true;
					break;
				}
			}

			if (!isSupported)
				return false;
		}


		vkpp::QueueFamilyIndices queues {s_getQueueFamiliesIndices(device)};

		if (!(queues.get(vkpp::QueueType::graphics).index.has_value()
			&& queues.get(vkpp::QueueType::graphics).count.has_value()
		))
			return false;

		if (!(queues.get(vkpp::QueueType::present).index.has_value()
			&& queues.get(vkpp::QueueType::present).count.has_value()
		))
			return false;

		
		return true;
	}



} // namespace vkpp
