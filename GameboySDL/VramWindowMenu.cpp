#include "VramWindowMenu.hxx"
#include "VramWindow.hxx"

VramWindowMenu::VramWindowMenu(VramWindow* vramWindow)
{
	this->parentWindow = vramWindow;
	this->vramWindow = vramWindow;

	width = 100;
	height = 100;

	SDLInitialize();
	SDL_RaiseWindow(window);

	buttonTexts = { "Address Mode: 0x8000", "Background Source: 0x9800", "Exit" };

	addButtons();
}

VramWindowMenu::~VramWindowMenu()
{
	//Deallocate surface
	SDL_FreeSurface(surface);
	surface = NULL;

	//Destroy window
	SDL_DestroyWindow(window);
	window = NULL;
}


void VramWindowMenu::clickButton(int buttonID)
{
	switch (buttonID)
	{

	case 0:
	{
		unsigned_four_byte backgroundAddress;
		
		
		if (buttonTexts[0] == "Address Mode: 0x8000")
		{
			buttonTexts[0] = "Address Mode: 0x8800";
			backgroundAddress = 0x9000;
		}
		else
		{
			buttonTexts[0] = "Address Mode: 0x8000";
			backgroundAddress = 0x8000;
		}
		this->vramWindow->getGameboy()->setBackgroundSettings(backgroundAddress, NULL);
		
		addButtons();
		break;
	}
	case 1:
	{
		unsigned_four_byte tilemapAddress;


		if (buttonTexts[1] == "Background Source: 0x9800")
		{
			buttonTexts[1] = "Background Source: 0x9C00";
			tilemapAddress = 0x9C00;
		}
		else
		{
			buttonTexts[1] = "Background Source: 0x9800";
			tilemapAddress = 0x9800;
		}
		this->vramWindow->getGameboy()->setBackgroundSettings(NULL, tilemapAddress);

		addButtons();
		break;
	}
	}
}
