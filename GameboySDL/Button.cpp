#include "Button.hxx"
Button::Button(std::string text, int id, SDL_Rect rect, SDL_Texture* textTexture)
{
	this->text = text;
	this->id = id;
	this->rect = rect;
	this->textTexture = textTexture;
}

SDL_Rect Button::getRect()
{
	return rect;
}

int Button::getID()
{
	return id;
}