#include "ContextMenu.hxx"
#include <string>
#include <iostream>


void ContextMenu::SDLInitialize()
{
	int xParent, yParent;

	//Get window position on screen
	SDL_GetWindowPosition(parentWindow->getWindow(), &xParent, &yParent);
	int xMouse, yMouse;
	//Get mouse position in the window
	SDL_GetMouseState(&xMouse, &yMouse);
	window = SDL_CreateWindow("Example", xParent + xMouse, yParent + yMouse, width, height, SDL_WINDOW_BORDERLESS | SDL_WINDOW_SKIP_TASKBAR);
	if (!window) {
		std::cout << "Error creating window: " << SDL_GetError() << std::endl;
		return;
	}

	windowID = SDL_GetWindowID(window);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (!renderer) {
		std::cout << "Error creating renderer: " << SDL_GetError() << std::endl;
		return;
	}
	surface = SDL_GetWindowSurface(window);
}

void ContextMenu::update(SDL_Event event)
{
	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_LEFT && event.window.windowID == windowID)
	{
		clickButton(getHoveredButtonID());
	}
}

void ContextMenu::addButtons()
{

	TTF_Init();
	TTF_Font* font;
	font = TTF_OpenFont(std::string("C://Windows//Fonts//arial.ttf").c_str(), 12);

	SDL_Rect rect = { 0, 0, 10, 200 };
	for (int i = 0; i < buttonTexts.size(); i++)
	{
		SDL_FillRect(surface, &rect, SDL_MapRGB(surface->format, 255, 255, 255));
		SDL_Color backgroundColor = { 255, 255, 255, 255 };
		SDL_Color textColor = { 0, 0, 0, 255 };
		SDL_Surface* textSurface = TTF_RenderText_Shaded(font, buttonTexts[i].c_str(), textColor, backgroundColor);
		SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
		rect.h = textSurface->h;
		rect.w = textSurface->w;
		buttons.push_back(Button(buttonTexts[i], i, rect, textTexture));

		SDL_RenderCopy(renderer, textTexture, NULL, &rect);
		rect.y += textSurface->h;
		SDL_SetWindowSize(window, 200, textSurface->h * static_cast<int>(buttonTexts.size()));
		SDL_FreeSurface(textSurface);

	}
	TTF_CloseFont(font);
	SDL_UpdateWindowSurface(window);
	SDL_RenderPresent(renderer);
}

int ContextMenu::getHoveredButtonID()
{
	//Get mouse position in the window
	int xMouse, yMouse;
	SDL_GetMouseState(&xMouse, &yMouse);

	for (int i = 0; i < buttons.size(); i++)
	{
		if (yMouse < buttons[i].getRect().y + buttons[i].getRect().h)
		{
			return buttons[i].getID();
		}
	}
	return -1;
}

void ContextMenu::clickButton(int buttonID)
{
}
