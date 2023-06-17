#pragma once

#include <vector>

#include <vulkan/vulkan.h>


namespace vkpp
{
	class Instance;

	class SwapChain
	{
		public:
			SwapChain(vkpp::Instance &instance);
			~SwapChain();

			void recreate();

			inline VkSwapchainKHR get() const noexcept {return m_swapChain;}
			inline const std::vector<VkImage> &getImages() const noexcept {return m_images;}

		
		private:
			VkSurfaceFormatKHR s_chooseFormat(const std::vector<VkSurfaceFormatKHR> &formats);
			VkPresentModeKHR s_choosePresentMode(const std::vector<VkPresentModeKHR> &presentModes);
			VkExtent2D s_chooseExtent(vkpp::Instance &instance, const VkSurfaceCapabilitiesKHR &capabilities);

			vkpp::Instance &m_instance;
			VkSwapchainKHR m_swapChain;
			std::vector<VkImage> m_images;
	};

} // namespace vkpp
