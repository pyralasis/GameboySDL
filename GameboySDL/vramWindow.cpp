#include "vramWindow.hxx"
tile::tile()
{
}
tile::tile(std::vector<int> pixels, int tileNumber, int tileAddress)
{
	this->pixels = pixels;
	this->tileNumber = tileNumber;
	this->tileAddress = tileAddress;
}

void tile::createTexture(SDL_Renderer* renderer)
{
	surface = SDL_CreateRGBSurface(0, 8, 8, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_LockSurface(surface);
	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 8; j++) {
			Uint32* pixel = (Uint32*)((Uint8*)surface->pixels + i * surface->pitch + j * 4);
			switch (pixels[i * 8 + j])
			{
			case 0:
				*pixel = 0xFFFFFFFF;
				break;
			case 1:
				*pixel = 0xACACACFF;
				break;
			case 2:
				*pixel = 0x565656FF;
				break;
			case 3:
				*pixel = 0x000000FF;
				break;
			}
		}
	}
	SDL_UnlockSurface(surface);
	texture = SDL_CreateTextureFromSurface(renderer, surface);
}

void tile::freeTexture()
{
	if (texture != nullptr)
	{
		SDL_DestroyTexture(texture);
		texture = nullptr;
	}
	if (surface != nullptr)
	{
		SDL_FreeSurface(surface);
		surface = nullptr;
	}
}

SDL_Texture* tile::getTexture()
{
	return texture;
}

VramWindow::VramWindow(Gameboy* gameboy)
{
	this->gameboy = gameboy;
	width = 500;
	height = 1000;
	SDLInitialize();
}

void VramWindow::update(SDL_Event event)
{
	if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT && event.window.windowID == windowID && vramWindowMenu == nullptr)
	{
		vramWindowMenu = new VramWindowMenu(this);
	}


	//Check if the popup menu exists and update it appropriatley
	if (vramWindowMenu != nullptr)
	{
		vramWindowMenu->update(event);

		//Close popup menu if the user clicks anywhere besided the menu
		if (vramWindowMenu->isFocused() == false)
		{
			delete vramWindowMenu;
			vramWindowMenu = nullptr;
		}
	}
}

void VramWindow::drawTiles()
{
	/*SDL_Surface* redSurface = SDL_CreateRGBSurface(0, 640, 480, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, redSurface);
	SDL_Rect dstRectq = { tileMapDrawXOffset, tileMapDrawYOffset, 16*8, 24*8 };
	SDL_RenderCopy(renderer, tex, NULL, &dstRectq);
	SDL_RenderPresent(renderer);
	SDL_FreeSurface(redSurface);
	SDL_DestroyTexture(tex);
	SDL_RenderPresent(renderer);*/



	for (int y = 0; y < 24; y++) {
		for (int x = 0; x < 16; x++) {
			SDL_Rect dstRect = { x * 8 + tileMapDrawXOffset + 1, y * 8 + tileMapDrawYOffset + 1, 8, 8 };
			SDL_RenderCopy(renderer, tiles[x + y * 16].getTexture(), NULL, &dstRect);
		}
	}
	SDL_RenderPresent(renderer);
}

void VramWindow::populateTileData()
{
	std::vector<int> tileData = gameboy->getPpu()->getTiles();
	int tileIndex = 0;
	for (int i = 0; i < tileData.size(); i += 64)
	{
		tiles[tileIndex].freeTexture(); // free the previous tile texture
		std::vector<int> currentTilePixels(tileData.begin() + i, tileData.begin() + i + 64); //copy the pixels for this tile
		tiles[tileIndex] = tile(currentTilePixels, tileIndex, (0x8000 + (tileIndex * 16))); //create the new tile
		tiles[tileIndex].createTexture(renderer); //create the texture for the new tile
		tileIndex++;
	}
}

void VramWindow::drawBackground()
{
	std::vector<int> backgroundTileNumbers = gameboy->getPpu()->getBackgroundTileNumbers();

	for (int y = 0; y < 32; y++) {
		for (int x = 0; x < 32; x++) {
			SDL_Rect dstRect = { x * 8 + backgroundDrawXOffset, y * 8 + backgroundDrawYOffset, 8, 8 };
			SDL_RenderCopy(renderer, tiles[backgroundTileNumbers[x + y * 32]].getTexture(), NULL, &dstRect);
		}
	}
	SDL_RenderPresent(renderer);

}

void VramWindow::drawOam()
{
	std::vector<Sprite> sprites = gameboy->getPpu()->getOam();
	for (int i = 0; i < 40; i++) {
		SDL_Rect dstRect = { i * 8 + oamDrawXOffset, 8 + oamDrawYOffset, 8, 8 };
		SDL_RenderCopy(renderer, tiles[sprites[i].tileIndex].getTexture(), NULL, &dstRect);
	}
	SDL_RenderPresent(renderer);

}

Gameboy* VramWindow::getGameboy()
{
	return gameboy;
}


