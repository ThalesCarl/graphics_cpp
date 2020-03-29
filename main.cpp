#include "game_engine.h"

int main()
{
	GameEngine game;

	bool doIhaveAconsole;
	doIhaveAconsole = game.contructConsole();
	
  int c = getch();
	cout << doIhaveAconsole << endl;

	return 0;
}
