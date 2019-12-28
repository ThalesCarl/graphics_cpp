#include<iostream>
#include<string>
#include<ncurses.h>
#include<thread>
using namespace std;

int nFieldWidth  = 12;
int nFieldHeight = 18;
int  *pField = nullptr; 
char *screen = nullptr;

string tetromino[7];

int rotate(int x, int y, int r);
bool doesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY);

int main()
{
  initscr();
  cbreak();
  noecho();
  // Create Screen Bufffer using nCurses
  WINDOW *win = newwin(nFieldHeight,nFieldWidth,1,1);
  refresh();
  keypad(win,true); //Allow to use extended ASCII, to get acess to arrow keys.
  notimeout(win, true);
  


  // Building the shapes
  tetromino[0].append("..X.");
  tetromino[0].append("..X.");
  tetromino[0].append("..X.");
  tetromino[0].append("..X.");

	tetromino[1].append("..X..XX...X.....");
	tetromino[2].append(".....XX..XX.....");
	tetromino[3].append("..X..XX..X......");
	tetromino[4].append(".X...XX...X.....");
	tetromino[5].append(".X...X...XX.....");
	tetromino[6].append("..X...X..XX.....");

  //Creating the empty field
	pField = new  int[nFieldWidth*nFieldHeight]; // Create play field identifier
	screen = new char[nFieldWidth*nFieldHeight]; // Create play field buffer
	for (int x = 0; x < nFieldWidth; x++) // Board Boundary
		for (int y = 0; y < nFieldHeight; y++) 
			pField[y*nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0; // 9 is the border and 0 is empty field
  bool bGameOver = false;

  //Initialize game
  int nCurrentPiece = 0;
  int nCurrentRotation = 0;
  int nCurrentX = nFieldWidth /2;
  int nCurrentY = 0;
  int keyInput = 0;
  
  while(!bGameOver)
  {
    // Game Timing ===================================================
    this_thread::sleep_for(50ms);
    
    // User input ===================================================
    keyInput = wgetch(win);

    //Game Logic  ===================================================
    if(keyInput == KEY_UP)
    {
      screen[0] = 'P';
    }
    else
    {
      screen[0] = '#';
    }

    //Display     ===================================================
    
    //Draw static field
    for(int x = 0; x < nFieldWidth; x++)
      for(int y = 0; y < nFieldHeight; y++)
        screen[y*nFieldWidth + x] = " ABCDEFG=#"[pField[y*nFieldWidth + x]];

    //Draw current piece
		for (int px = 0; px < 4; px++)
			for (int py = 0; py < 4; py++)
				if (tetromino[nCurrentPiece][rotate(px, py, nCurrentRotation)] != '.')
					screen[(nCurrentY + py)*nFieldWidth + (nCurrentX + px)] = nCurrentPiece + 65;

    wclear(win);
    wprintw(win,screen);
    wrefresh(win);
    bGameOver = true;
  }
  endwin();
  cout << keyInput << endl;
  return(0);
}

int rotate(int x, int y, int r)
{
  int pi = 0;
  switch(r%4)
  {
    case 0: pi = x + y*4;        break;//   0 degrees
    case 1: pi = 12 + y - (x*4); break;//  90 degrees
    case 2: pi = 15 - x - (y*4); break;// 180 degrees
    case 3: pi =  3 - y + (x*4); break;// 270 degrees
  }
  return pi;
}

bool doesPieceFit(int nTetromino, int nRotation, int nPosX, int nPosY)
{
  for(int px = 0; px < 4; px++)
    for(int py = 0; py < 4; py++)
    {
      //Get index into piece
      int pi = rotate(px,py,nRotation);

      //Get index into field
      int fi = (nPosY + py) * nFieldWidth + (nPosX + px);

      //Segment fault check
      if(nPosX + px >= 0 && nPosX + px < nFieldWidth)
      {
        if(nPosY + py >=0 && nPosY + py < nFieldHeight)
        {
          if(tetromino[nTetromino][pi] == 'X' && pField[fi] != 0)
            return false; //fail on first hit
        }
      }
    }
  

  return true;
}

