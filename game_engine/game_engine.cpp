#include "game_engine.h"

GameEngine::GameEngine(int screenWidth, int screenHeight)
{

  cout << "Creating game engine" << endl;
	this -> screenWidth  = screenWidth;
	this -> screenHeight = screenHeight;

	initscr();
	cbreak();
	noecho();
}

bool GameEngine::contructConsole()
{
  bool sucess = false;

	WINDOW *win = newwin(screenHeight,screenWidth,0,0);
	if(win != NULL)
    sucess = true;

  return sucess;
}

void GameEngine::destroyConsole()
{
  endwin();
}

GameEngine::~GameEngine()
{
  cout << "Destroying dreams T.T" << endl;
}
