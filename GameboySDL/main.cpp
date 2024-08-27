#include <SDL.h>
#include <stdio.h>
#include <iostream>
#include <Windows.h>
#include <Commdlg.h>
#include <fstream>
#include "Gameboy.hxx"
#include "MainWindow.hxx"

int main(int argc, char* args[])
{
	bool running = true;

	MainWindow mainWindow;

	while (running) {
		Uint64 start = SDL_GetPerformanceCounter();

		// Process Events
		SDL_Event e;
		while (SDL_PollEvent(&e)) {
			mainWindow.update(e);
		}

		//Gameboy Main CPU Loop
		if (mainWindow.getGameboy()->isRomLoaded())
		{
			mainWindow.getGameboy()->mainLoop();
			mainWindow.drawViewport();
			mainWindow.getGameboy()->getPpu()->clearViewPort();
		}


		//VRAM Viewer Display Loop
		if (mainWindow.getVramWindow() != nullptr && mainWindow.getGameboy()->isRomLoaded())
		{
			mainWindow.getVramWindow()->populateTileData();
			mainWindow.getVramWindow()->drawTiles();
			mainWindow.getVramWindow()->drawBackground();
			mainWindow.getVramWindow()->drawOam();
		}

		// Cap to 60 FPS
		Uint64 end = SDL_GetPerformanceCounter();
		float elapsedMS = (end - start) / (float)SDL_GetPerformanceFrequency() * 1000.0f;
		float timeRemainingInFrame = 16.666f - elapsedMS;
		if (timeRemainingInFrame > 0)
		{
			SDL_Delay(floor(timeRemainingInFrame));
		}
		std::cout << "Frame took " << elapsedMS << " milliseconds" << std::endl;
	}
	return 0;
}
