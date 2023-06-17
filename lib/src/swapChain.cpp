#include <algorithm>
#include <cstdint>
#include <limits>

#include "instance.hpp"
#include "swapChain.hpp"



namespace vkpp
{
	SwapChain::SwapChain(vkpp::Instance &instance) : 
		m_instance {instance},
		m_swapChain {VK_NULL_HANDLE},
		m_images {}
	{
		this->recreate();
	}



	SwapChain::~SwapChain()
	{
		vkDestroySwapchainKHR(m_instance.getDevice().get(), m_swapChain, nullptr);
	}



	void SwapChain::recreate()
	{
		VkSurfaceFormatKHR format {s_chooseFormat(m_instance.getPhysicalDevice().getSwapChainInfos().formats)};
		VkPresentModeKHR presentMode {s_choosePresentMode(m_instance.getPhysicalDevice().getSwapChainInfos().presentModes)};
		VkExtent2D extent {s_chooseExtent(m_instance, m_instance.getPhysicalDevice().getSwapChainInfos().capabilities)};

		uint32_t imageCount {m_instance.getPhysicalDevice().getSwapChainInfos().capabilities.minImageCount + 1};
		if (m_instance.getPhysicalDevice().getSwapChainInfos().capabilities.maxImageCount != 0)
			imageCount = std::min(imageCount, m_instance.getPhysicalDevice().getSwapChainInfos().capabilities.maxImageCount);

		
		VkSwapchainCreateInfoKHR createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = m_instance.getSurface();
		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = format.format;
		createInfo.imageColorSpace = format.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		createInfo.preTransform = m_instance.getPhysicalDevice().getSwapChainInfos().capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;
		createInfo.oldSwapchain = VK_NULL_HANDLE;

		std::vector<uint32_t> independentQueueIndices {};

		for (auto queue : m_instance.getPhysicalDevice().getQueues().get())
		{
			if (!queue.second.index.has_value())
				continue;

			bool found {false};

			for (auto independent : independentQueueIndices)
			{
				if (independent == queue.second.index.value())
				{
					found = true;
					break;
				}
			}

			if (!found)
				independentQueueIndices.push_back(queue.second.index.value());
		}

		if (independentQueueIndices.size() == 1)
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
			createInfo.queueFamilyIndexCount = 0;
			createInfo.pQueueFamilyIndices = nullptr;
		}

		else
		{
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = static_cast<uint32_t> (independentQueueIndices.size());
			createInfo.pQueueFamilyIndices = independentQueueIndices.data();
		}

		if (m_swapChain != VK_NULL_HANDLE)
			vkDestroySwapchainKHR(m_instance.getDevice().get(), m_swapChain, nullptr);


		if (vkCreateSwapchainKHR(m_instance.getDevice().get(), &createInfo, nullptr, &m_swapChain) != VK_SUCCESS)
			throw std::runtime_error("VKPP : Can't create (or recreate) a swap chain");

		uint32_t imagesCount {};
		if (vkGetSwapchainImagesKHR(m_instance.getDevice().get(), m_swapChain, &imagesCount, nullptr) != VK_SUCCESS)
			throw std::runtime_error("VKPP : Can't get swap chain images count");

		m_images.resize(imagesCount);
		if (vkGetSwapchainImagesKHR(m_instance.getDevice().get(), m_swapChain, &imagesCount, m_images.data()) != VK_SUCCESS)
			throw std::runtime_error("VKPP : Can't get swap chain images");
	}



	VkSurfaceFormatKHR SwapChain::s_chooseFormat(const std::vector<VkSurfaceFormatKHR> &formats)
	{
		for (auto format : formats)
		{
			if (format.format == VK_FORMAT_B8G8R8A8_SRGB && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
				return format;
		}

		return formats[0];
	}



	VkPresentModeKHR SwapChain::s_choosePresentMode(const std::vector<VkPresentModeKHR> &presentModes)
	{
		for (auto presentMode : presentModes)
		{
			if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
				return presentMode;
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}



	VkExtent2D SwapChain::s_chooseExtent(vkpp::Instance &instance, const VkSurfaceCapabilitiesKHR &capabilities)
	{
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
			return capabilities.currentExtent;

		int width {};
		int height {};
		SDL_GetWindowSizeInPixels(instance.getParameters().window, &width, &height);

		VkExtent2D extent {static_cast<uint32_t> (width), static_cast<uint32_t> (height)};

		extent.width = std::clamp(extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		extent.height = std::clamp(extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

		return extent;
	}



} // namespace vkpp
