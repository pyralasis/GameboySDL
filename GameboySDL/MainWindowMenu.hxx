#pragma once

#include <SDL.h>
#include "Window.hxx"
#include <vector>
#include <string>
#include "VramWindow.hxx"
#include "ContextMenu.hxx"
class MainWindow;


class MainWindowMenu : public ContextMenu
{
private:
	MainWindow* mainWindow;

public:
	MainWindowMenu(MainWindow* mainWindow);
	~MainWindowMenu();
	
	void clickButton(int buttonID);
};