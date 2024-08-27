#pragma once
#include "ContextMenu.hxx"

class VramWindow;
class VramWindowMenu : public ContextMenu
{
private:
    VramWindow* vramWindow = nullptr;
public:
    VramWindowMenu(VramWindow* vramWindow);
    ~VramWindowMenu();
    void clickButton(int buttonID);
};