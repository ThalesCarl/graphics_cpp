#include<iostream>
#include<string>
#include<ncurses.h>
#include<thread>
#include<vector>
using namespace std;

class GameEngine
{
public:
	GameEngine(int screenWidth = 20, int screenHeight = 15);

	bool contructConsole();

	~GameEngine();

private:
	int screenWidth;
	int screenHeight;
};
