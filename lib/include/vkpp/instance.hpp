#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "device.hpp"
#include "physicalDevice.hpp"
#include "swapChain.hpp"
#include "utils/version.hpp"


namespace vkpp
{
	enum class VulkanVersion
	{
		v10 = VK_API_VERSION_1_0,
		v11 = VK_API_VERSION_1_1,
		v12 = VK_API_VERSION_1_2,
		v13 = VK_API_VERSION_1_3,
	};

	struct InstanceParameter
	{
		SDL_Window *window;
		std::string appName;
		std::string engineName {""};
		vkpp::utils::Version appVersion;
		vkpp::utils::Version engineVersion {0, 0, 0};
		vkpp::VulkanVersion vulkanVersion {vkpp::VulkanVersion::v10};
		std::vector<const char *> instanceExtensions {};
		std::vector<const char *> deviceExtensions {};
	};


	class Instance
	{
		public:
			Instance(const vkpp::InstanceParameter &parameter);
			~Instance();

			inline VkInstance get() const noexcept {return m_instance;}
			inline VkSurfaceKHR getSurface() const noexcept {return m_surface;}
			inline const vkpp::InstanceParameter &getParameters() const noexcept {return m_parameter;}
			inline const vkpp::PhysicalDevice &getPhysicalDevice() const noexcept {return *m_physicalDevice;}
			inline const vkpp::Device &getDevice() const noexcept {return *m_device;}

		private:
			static std::vector<const char *> s_checkExtensions(const vkpp::InstanceParameter &parameter);
			static bool s_checkValidationLayers(const std::vector<const char*> &layers);

			static void s_createInstance(
				VkInstance &instance,
				const vkpp::InstanceParameter &parameter,
				const std::vector<const char *> &extensions,
				const std::vector<const char *> &layers,
				bool layersSupported
			);

			const vkpp::InstanceParameter &m_parameter;
			VkInstance m_instance;
			VkSurfaceKHR m_surface;
			vkpp::PhysicalDevice *m_physicalDevice;
			vkpp::Device *m_device;
			vkpp::SwapChain *m_swapChain;
	};

} // namespace vkpp
