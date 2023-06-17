#include <memory>
#include <stdexcept>
#include <iostream>

#include <SDL2/SDL_vulkan.h>

#include "instance.hpp"



namespace vkpp
{
	Instance::Instance(const vkpp::InstanceParameter &parameter) : 
		m_parameter {parameter},
		m_instance {VK_NULL_HANDLE},
		m_surface {VK_NULL_HANDLE},
		m_physicalDevice {nullptr},
		m_device {nullptr}
	{
		bool layerSupported {true};

		#ifdef NDEBUG
			const std::vector<const char*> layers {};
		#else
			const std::vector<const char*> layers {"VK_LAYER_KHRONOS_validation"};
			layerSupported = s_checkValidationLayers(layers);
		#endif

		const std::vector<const char*> extensions {s_checkExtensions(m_parameter)};
		s_createInstance(m_instance, m_parameter, extensions, layers, layerSupported);

		if (!SDL_Vulkan_CreateSurface(m_parameter.window, m_instance, &m_surface))
			throw std::runtime_error("VKPP : Can't create a VkSurfaceKHR : " + std::string(SDL_GetError()));

		m_physicalDevice = new vkpp::PhysicalDevice(*this);
		m_device = new vkpp::Device(*m_physicalDevice);
	}



	Instance::~Instance()
	{
		delete m_device;
		delete m_physicalDevice;
		vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
		vkDestroyInstance(m_instance, nullptr);
	}



	std::vector<const char *> Instance::s_checkExtensions(const vkpp::InstanceParameter &parameter)
	{
		uint32_t availableExtensionsCount {};
		if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, nullptr) != VK_SUCCESS)
			throw std::runtime_error("VKPP : Can't get supported vulkan instance extensions count");

		std::vector<VkExtensionProperties> availableExtensions {availableExtensionsCount};
		if (vkEnumerateInstanceExtensionProperties(nullptr, &availableExtensionsCount, availableExtensions.data()) != VK_SUCCESS)
			throw std::runtime_error("VKPP : Can't get supported vulkan instance extensions");


		uint32_t neededExtensionsCount {};
		if (!SDL_Vulkan_GetInstanceExtensions(parameter.window, &neededExtensionsCount, nullptr))
			throw std::runtime_error("VKPP : Can't get SDL2 required instance extensions count : " + std::string(SDL_GetError()));

		std::vector<const char*> neededExtensions {neededExtensionsCount};
		if (!SDL_Vulkan_GetInstanceExtensions(parameter.window, &neededExtensionsCount, neededExtensions.data()))
			throw std::runtime_error("VKPP : Can't get SDL2 required instance extensions : " + std::string(SDL_GetError()));


		neededExtensions.insert(
			neededExtensions.end(),
			std::make_move_iterator(parameter.instanceExtensions.begin()),
			std::make_move_iterator(parameter.instanceExtensions.end())
		);

		for (auto extension : neededExtensions)
		{
			bool found {false};

			for (auto available : availableExtensions)
			{
				if (strcmp(extension, available.extensionName) == 0)
				{
					found = true;
					break;
				}
			}

			if (!found)
				throw std::runtime_error("VKPP : Vulkan instance extension '" + std::string(extension) + "' is not available");
		}

		#ifndef NDEBUG

			std::clog << "Supported instance's extensions :" << std::endl;

			for (auto available : availableExtensions)
				std::clog << "\t" << available.extensionName << std::endl;

		#endif

		return neededExtensions;
	}



	bool Instance::s_checkValidationLayers(const std::vector<const char*> &layers)
	{
		uint32_t availableLayersCount {};
		if (vkEnumerateInstanceLayerProperties(&availableLayersCount, nullptr) != VK_SUCCESS)
		{
			std::cerr << "VKPP : Can't get supported validation layers count" << std::endl;
			return false;
		}

		std::vector<VkLayerProperties> availableLayers {availableLayersCount};
		if (vkEnumerateInstanceLayerProperties(&availableLayersCount, availableLayers.data()) != VK_SUCCESS)
		{
			std::cerr << "VKPP : Can't get supported validation layers" << std::endl;
			return false;
		}

		std::clog << "Available validation layers : " << std::endl;

		for (auto available : availableLayers)
		{
			std::clog << "\t" << available.layerName << " : " << available.description << std::endl;
		}


		for (auto layer : layers)
		{
			bool found {false};

			for (auto available : availableLayers)
			{
				if (strcmp(layer, available.layerName) == 0)
				{
					found = true;
					break;
				}
			}

			if (!found)
			{
				std::cerr << "VKPP : Validation layer '" << layer << "' isn't supported" << std::endl;
				return false;
			}
		}

		return true;
	}



	void Instance::s_createInstance(
		VkInstance &instance,
		const vkpp::InstanceParameter &parameter,
		const std::vector<const char *> &extensions,
		const std::vector<const char *> &layers,
		bool layersSupported
	)
	{
		VkApplicationInfo appInfo {};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = parameter.appName.c_str();
		appInfo.applicationVersion = VK_MAKE_VERSION(
			parameter.appVersion.major,
			parameter.appVersion.minor,
			parameter.appVersion.patch
		);
		appInfo.pEngineName = parameter.engineName.c_str();
		appInfo.engineVersion = VK_MAKE_VERSION(
			parameter.engineVersion.major,
			parameter.engineVersion.minor,
			parameter.engineVersion.patch
		);
		appInfo.apiVersion = static_cast<uint32_t> (parameter.vulkanVersion);


		VkInstanceCreateInfo createInfo {};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;
		createInfo.enabledExtensionCount = extensions.size();
		createInfo.ppEnabledExtensionNames = extensions.data();
		createInfo.enabledLayerCount = 0;
		createInfo.ppEnabledLayerNames = nullptr;

		#ifndef NDEBUG
			if (layersSupported)
			{
				createInfo.enabledLayerCount = layers.size();
				createInfo.ppEnabledLayerNames = layers.data();
			}
		#endif

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
			throw std::runtime_error("VKPP : Can't create a vulkan instance");
	}



} // namespace vkpp
