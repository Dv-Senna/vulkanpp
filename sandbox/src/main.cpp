#include <exception>
#include <iostream>
#include <memory>

#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <vulkan/vulkan.h>

#include "vkpp/vulkanpp.hpp"



int main(int, char *[])
{
	try
	{
		SDL_Init(SDL_INIT_VIDEO);
		std::unique_ptr<
			SDL_Window,
			decltype([](SDL_Window *window){SDL_DestroyWindow(window);})
		> window {SDL_CreateWindow(
			"vulkanpp",
			SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
			16 * 70, 9 * 70,
			SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN
		)};

		vkpp::InstanceParameter instanceParameter {};
		instanceParameter.window = window.get();
		instanceParameter.appName = "vulkanpp";
		instanceParameter.appVersion = {1, 0, 0};
		instanceParameter.vulkanVersion = vkpp::VulkanVersion::v10;
		//instanceParameter.instanceExtensions = {"vk_this_is_not_a_valid_extension_haha"};

		vkpp::Instance instance {instanceParameter};

		
		bool running {true};
		SDL_Event event {};

		while (running)
		{
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_QUIT || event.type == SDL_KEYDOWN)
					running = false;
			}
		}
	}

	catch (const std::exception &exception)
	{
		std::cerr << "ERROR : " << exception.what() << std::endl;
	}

	SDL_Quit();

	return 0;
}