#include "MainWindow.hxx"

MainWindow::MainWindow()
{
    gameboy = new Gameboy;
    gameboy->initialize();

	width = 160;
	height = 144;
	SDLInitialize();
}

void MainWindow::update(SDL_Event event)
{
    if (event.type == SDL_MOUSEBUTTONDOWN && event.button.button == SDL_BUTTON_RIGHT && event.window.windowID == windowID && mainWindowMenu == nullptr)
    {
        mainWindowMenu = new MainWindowMenu(this);
    }
    

    //Check if the popup menu exists and update it appropriatley
    if (mainWindowMenu != nullptr)
    {
        mainWindowMenu->update(event);

        //Close popup menu if the user clicks anywhere besided the menu
        if (mainWindowMenu->isFocused() == false)
        {
            delete mainWindowMenu;
            mainWindowMenu = nullptr;
        }
    }

    //Check if VRAM Window is open and update appropriately
    if (vramWindow != nullptr)
    {
        vramWindow->update(event);
    }

    if (event.type == SDL_KEYDOWN && gameboy != nullptr)
    {
        gameboy->getControls()->pressButton(getButtonCode(event.button.button));
    }
    if (event.type == SDL_KEYUP && gameboy != nullptr)
    {
        gameboy->getControls()->releaseButton(getButtonCode(event.button.button));
    }
}

Gameboy* MainWindow::getGameboy()
{
    return gameboy;
}

void MainWindow::setVramWindow(VramWindow* vramWindow)
{
    this->vramWindow = vramWindow;
}

VramWindow* MainWindow::getVramWindow()
{
    return vramWindow;
}

void MainWindow::drawViewport()
{
    std::vector<int> viewport = gameboy->getPpu()->getViewPort();
    SDL_Surface* viewportSurface = SDL_CreateRGBSurface(0, 160, 144, 32, 0xFF000000, 0x00FF0000, 0x0000FF00, 0x000000FF);

    SDL_LockSurface(viewportSurface);
    if (viewport.size() != 0)
    {
        for (int y = 0; y < 144; y++) {
            for (int x = 0; x < 160; x++) {
                Uint32* pixel = (Uint32*)((Uint8*)viewportSurface->pixels + y * viewportSurface->pitch + x * 4);
                switch (viewport[y * 160 + x])
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
    }
    SDL_UnlockSurface(viewportSurface);
    SDL_Texture* viewportTexture = SDL_CreateTextureFromSurface(renderer, viewportSurface);
    SDL_Rect dstRect = { 0, 0, 160, 144 };
    SDL_RenderCopy(renderer, viewportTexture, NULL, NULL);
    SDL_RenderPresent(renderer);
    SDL_DestroyTexture(viewportTexture);
    SDL_FreeSurface(viewportSurface);
}

Gameboy* MainWindow::restartGameboy()
{
    delete gameboy;
    gameboy = new Gameboy();
    gameboy->initialize();
    return gameboy;
}

int MainWindow::getButtonCode(int key)
{
    switch (key)
    {
    case SDL_SCANCODE_UP:
        return 0;
    case SDL_SCANCODE_DOWN:
        return 1;
    case SDL_SCANCODE_LEFT:
        return 2;
    case SDL_SCANCODE_RIGHT:
        return 3;
    case SDL_SCANCODE_Z:
        return 4;
    case SDL_SCANCODE_X:
        return 5;
    case SDL_SCANCODE_RETURN:
        return 6;
    case SDL_SCANCODE_RSHIFT:
        return 7;
    default:
        return -1;
    }
}
