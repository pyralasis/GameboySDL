#include "Window.hxx"
#include <iostream>

int Window::getWidth()
{
	return width;
}

int Window::getHeight()
{
	return height;
}

SDL_Window* Window::getWindow()
{
	return window;
}

SDL_Renderer* Window::getRenderer()
{
	return renderer;
}

SDL_Surface* Window::getSurface()
{
	return surface;
}

void Window::SDLInitialize()
{
	window = SDL_CreateWindow("Example", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (!window) {
		std::cout << "Error creating window: " << SDL_GetError() << std::endl;
		return;
	}
	windowID = SDL_GetWindowID(window);
	std::cout << "Created window with ID: " << windowID << std::endl;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
		return;
	}

	surface = SDL_GetWindowSurface(window);
}

bool Window::isFocused()
{
	int flags = SDL_GetWindowFlags(window);
	int focused = flags & SDL_WINDOW_INPUT_FOCUS;
	return focused == SDL_WINDOW_INPUT_FOCUS ? true : false;
}
