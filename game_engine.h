#include<iostream>
#include<string>
#include<ncurses.h>
#include<thread>
#include<vector>
using namespace std;

class GameEngine
{
public:
	GameEngine(int screenWidth, int screenHeight);

	bool contructConsele();

	~GameEngine();

private:
	int screenWidth;
	int screenHeight;
};
