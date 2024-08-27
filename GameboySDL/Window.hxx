#pragma once

#include <SDL.h>
#include <SDL_ttf.h>
class Window
{
protected:
	int width = 0;
	int height = 0;

	SDL_Window* window = nullptr;
	SDL_Renderer* renderer = nullptr;
	SDL_Surface* surface = nullptr;


	int windowID = 1;

public:

	int getWidth();
	int getHeight();

	SDL_Window* getWindow();
	SDL_Renderer* getRenderer();
	SDL_Surface* getSurface();

	virtual void SDLInitialize();

	virtual void update(SDL_Event event) = 0;

	bool isFocused();

};