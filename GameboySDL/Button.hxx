#pragma once
#include <string>
#include <SDL.h>
#include <SDL_ttf.h>

class Button
{
private:
	SDL_Color color;
	std::string text;
	int id;
	SDL_Rect rect;
	SDL_Texture* textTexture;
	int buttonState;


public:
	Button(std::string text, int id, SDL_Rect rect, SDL_Texture* textTexture);
	SDL_Rect getRect();
	int getID();
};