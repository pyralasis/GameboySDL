#pragma once
#include "Window.hxx"
#include "Button.hxx"
#include <vector>


class ContextMenu : public Window
{
protected:

    Window* parentWindow;

    std::vector<Button> buttons;
    std::vector<std::string> buttonTexts;
public:
    void SDLInitialize();
    void update(SDL_Event event);
    void addButtons();
    int getHoveredButtonID();
    virtual void clickButton(int buttonID);
};