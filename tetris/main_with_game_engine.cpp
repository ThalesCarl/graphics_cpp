#include "game_engine.h"

int main()
{
	GameEngine tetris();

	bool doIhaveAconsole;
	doIhaveAconsole = tetris.contructConsole();
	
	cout << doIhaveAconsole << endl;

	return 0;
}
