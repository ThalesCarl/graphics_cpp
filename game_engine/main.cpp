#include "game_engine.h"

int main()
{
	GameEngine game;

	bool doIhaveAconsole;
	doIhaveAconsole = game.contructConsole();

  game.destroyConsole();

  cout << doIhaveAconsole << endl;
	  
	return 0;
}
