#include "MainWindowMenu.hxx"
#include "MainWindow.hxx"
#include <Windows.h>

MainWindowMenu::MainWindowMenu(MainWindow* mainWindow)
{
	this->parentWindow = mainWindow;
	this->mainWindow = mainWindow;

	width = 100;
	height = 100;

	SDLInitialize();
	SDL_RaiseWindow(window);

	buttonTexts = { "Select ROM", "Open VRAM Viewer", "Exit" };
	
	addButtons();
}

MainWindowMenu::~MainWindowMenu()
{
	//Deallocate surface
	SDL_FreeSurface(surface);
	surface = NULL;

	//Destroy window
	SDL_DestroyWindow(window);
	window = NULL;
}



void MainWindowMenu::clickButton(int buttonID)
{
	switch (buttonID)
	{

	case 0:
	{
		wchar_t szFile[512];
		OPENFILENAME inputFile;
		// open a file name
		ZeroMemory(&inputFile, sizeof(inputFile));
		inputFile.lStructSize = sizeof(inputFile);
		inputFile.hwndOwner = NULL;
		inputFile.lpstrFile = szFile;
		inputFile.lpstrFile[0] = '\0';
		inputFile.nMaxFile = sizeof(szFile);
		inputFile.lpstrFilter = L".gb\0*.gb*\0";
		inputFile.nFilterIndex = 1;
		inputFile.lpstrFileTitle = NULL;
		inputFile.nMaxFileTitle = 0;
		inputFile.lpstrInitialDir = NULL;
		inputFile.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
		bool status = GetOpenFileName(&inputFile);

		if (!status)
		{
			break;
		}
		if (mainWindow->getGameboy()->isRomLoaded())
		{
			mainWindow->restartGameboy();
		}
		
		// Open file
		std::ifstream infile;
		infile.open(szFile, std::ios::binary);
		// Get length of file
		infile.seekg(0, std::ios::end);
		size_t length = infile.tellg();
		infile.seekg(0, std::ios::beg);
		// Read file
		char* romContents = new char[length];
		infile.read(romContents, length);
		unsigned_two_byte* romContentsConverted = reinterpret_cast<unsigned_two_byte*>(romContents);
		mainWindow->getGameboy()->readRom(romContentsConverted, static_cast<int>(length));
		delete[] romContents;
		break;
	}	
	case 1:
		mainWindow->setVramWindow(new VramWindow(mainWindow->getGameboy()));
		break;
	case 2:
		break;
	}
	

}


