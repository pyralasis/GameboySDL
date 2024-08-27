#pragma once
#include "Window.hxx"
#include "Gameboy.hxx"
#include "VramWindowMenu.hxx"

class tile
{
private:
	std::vector<int> pixels = {};
	int tileNumber = 0;
	int tileAddress = 0;
	SDL_Texture* texture = nullptr;
	SDL_Surface* surface = nullptr;
public:
	tile();
	tile(std::vector<int> pixels, int tileNumber, int tileAddress);
	void createTexture(SDL_Renderer* renderer);
	void freeTexture();
	SDL_Texture* getTexture();
};

class VramWindow : public Window
{
private:
	Gameboy* gameboy;
	VramWindowMenu* vramWindowMenu = nullptr;


	int tileMapDrawXOffset = 0;
	int tileMapDrawYOffset = 0;
	int backgroundDrawXOffset = 0;
	int backgroundDrawYOffset = 200;
	int oamDrawXOffset = 0;
	int oamDrawYOffset = 500;
	std::vector<tile> tiles = std::vector<tile>(384);
public:
	VramWindow(Gameboy* gameboy);
	void update(SDL_Event event);
	void drawTiles();
	void populateTileData();
	void drawBackground();
	void drawOam();
	Gameboy* getGameboy();

};