#pragma once

#include "Window.hxx"
#include "Gameboy.hxx"
#include "MainWindowMenu.hxx"
#include "VramWindow.hxx"

class MainWindow : public Window
{
private:
	Gameboy* gameboy;
	MainWindowMenu* mainWindowMenu = nullptr;
	VramWindow* vramWindow;

public:
	MainWindow();

	void update(SDL_Event event);
	Gameboy* getGameboy();
	void setVramWindow(VramWindow* vramWindow);
	VramWindow* getVramWindow();
	void drawViewport();
	Gameboy* restartGameboy();
	int getButtonCode(int key);
};