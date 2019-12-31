#include<string>
#include<ncurses.h>
#include<thread>
#include<vector>
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
  WINDOW *win2 = newwin(3,5,20,20);
  refresh();
  keypad(stdscr,TRUE); //Allow to use extended ASCII, to get acess to arrow keys.
  nodelay(stdscr,TRUE); 


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
  bool bRotateHold = false;

  //Initialize game
  int nCurrentPiece = 3;
  int nCurrentRotation = 0;
  int nCurrentX = nFieldWidth /2;
  int nCurrentY = 0;
  int keyInput;
  
  int nSpeed = 20;
  int nSpeedCounter = 0;
  bool bForceDown = false;
  
  vector<int> vLines;
  while(!bGameOver)
  {
    // Game Timing ===================================================
    this_thread::sleep_for(50ms);
    nSpeedCounter++;
    bForceDown = (nSpeedCounter == nSpeed);
    
    // User input ===================================================
    keyInput = getch();
    wclear(win2);
    wprintw(win2,"%d",keyInput);
    wrefresh(win2);
    
    //Game Logic  ===================================================
    if(keyInput != ERR)
    {
      switch(keyInput)
      {
        case(KEY_LEFT):
        {
          if(doesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX - 1,nCurrentY))
          {
            nCurrentX--;
          }
        }
        break;
        case(KEY_RIGHT):
        {
          if(doesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX + 1,nCurrentY))
          {
            nCurrentX++;
          }
        }
        break;
        case(KEY_DOWN):
        {
          if(doesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX,nCurrentY+1))
          {
            nCurrentY++;
          }
        }
        break;
        case(KEY_UP):
        {
          if(!bRotateHold && doesPieceFit(nCurrentPiece,nCurrentRotation + 1,nCurrentX,nCurrentY))
          {
            nCurrentRotation++;
            bRotateHold = true;
          }
          else
          {
            bRotateHold = false;
          }
        }
        break;
        case(KEY_END):
        {
          bGameOver = true;        
        }
        break;
      }
    }

    if(bForceDown)
    {
      nSpeedCounter = 0;
      if(doesPieceFit(nCurrentPiece,nCurrentRotation,nCurrentX,nCurrentY+1))
      {
        nCurrentY++;
      }
      else
      {
        //Lock the current piece in the field
        for(int px = 0; px < 4; px++)
          for(int py = 0; py < 4; py++)
            if(tetromino[nCurrentPiece][rotate(px,py,nCurrentRotation)] == 'X')
              pField[(nCurrentX+px) + (nCurrentY + py)*nFieldWidth] = nCurrentPiece + 1;
        
        //Check have we got any lines
        for(int py = 0; py < 4; py++)
        {
          if(nCurrentY + py < nFieldHeight - 1)
          {
            bool isThereALine  = true;
            for(int px = 1; px < nFieldWidth - 1; px++)
							isThereALine &= (pField[(nCurrentY + py) * nFieldWidth + px]) != 0;

            if(isThereALine)
            {
              for(int px = 1; px < nFieldWidth - 1; px++)
                pField[(nCurrentY + py)*nFieldWidth + px] = 8;
              vLines.push_back(nCurrentY+py);
            }
          }
        }
        //Pick the new piece
        nCurrentX = nFieldWidth / 2 - 1;
				nCurrentY = 0;
				nCurrentRotation = 0;
				nCurrentPiece = rand() % 7;

				bGameOver = !doesPieceFit(nCurrentPiece, nCurrentRotation, nCurrentX, nCurrentY);
      }
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

    if(!vLines.empty())
    {
      wclear(win);
      wprintw(win,screen);
      wrefresh(win);
      this_thread::sleep_for(400ms);

      for(auto  &v : vLines)
      {
        for(int px = 1; px < nFieldWidth - 1; px++)
        {
          for(int py = v; py > 0; py--)
            pField[py * nFieldWidth + px] = pField[(py-1)*nFieldWidth + px];
          pField[px] = 0;
        }
      }
			vLines.clear();
    }

    wclear(win);
    wprintw(win,screen);
    wrefresh(win);
    //bGameOver = true;
  }
  endwin();
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

