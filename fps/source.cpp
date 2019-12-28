#include<iostream>
#include<ncurses.h>
#include<string>
#include<cmath>
using namespace std;

const float PI= 3.1415;
int screenWidth  = 120;
int screenHeight =  40;

float playerXposition   = 0.0f;
float playerYposition   = 0.0f;
float playerAngleOfView = 0.0f;

int mapHeight = 16;
int mapWidth  = 16;

float fieldOfView  = PI /4.0;
float depth = 16.0;

int main()
{
  initscr();
  cbreak();
  noecho();
  WINDOW *win = newwin(screenHeight,screenWidth,0,0);
  refresh();
  char *screen = new char[screenWidth*screenHeight];

  string map;

  map += "#################";
  map += "#               #";
  map += "#               #";
  map += "#               #";
  map += "#               #";
  map += "#               #";
  map += "#               #";
  map += "#               #";
  map += "#               #";
  map += "#               #";
  map += "#               #";
  map += "#               #";
  map += "#               #";
  map += "#               #";
  map += "#               #";
  map += "#################";

  //Game loop
  while(1)
  {
    for(int x = 0; x <screenWidth; ++x)
    {
      float distanceToWall = 0.0;
      bool  hitWall = false;
      float currentAngle = (playerAngleOfView  - fieldOfView)/2.0;
      currentAngle += ((float)x/(float)screenWidth) * fieldOfView;

      float eyeX = sin(currentAngle);
      float eyeY = cos(currentAngle);

      while(!hitWall && distanceToWall < depth)
      {

        distanceToWall += 0.1;
        int testX = (int)(playerXposition + eyeX*distanceToWall);
        int testY = (int)(playerYposition + eyeY*distanceToWall);

        if(testX < 0 || testX >= mapWidth || testY < 0 || testY >= mapHeight)
        {
          hitWall = true;
          distanceToWall = depth; //maybe 0.0?
        }
        else
        {
          if(map[testX+mapWidth*testY] == '#')
          {
            hitWall = true;
          }
        }
      }
      int ceiling = (float)(screenHeight/2.0) - screenHeight/((float)distanceToWall);
      int floor   = screenHeight - ceiling;
      for(int y = 0; y < screenHeight; ++y)
      {
        if(y < ceiling)
        {
          screen[y*screenWidth + x] = ' '; 
          cout << "Opa\n" << endl;
        }
        else if(y>ceiling && y<=floor)
          screen[y*screenWidth + x] = '#';
        else
          screen[y*screenWidth + x] = ' ';

      }
    }
    screen[screenWidth*screenHeight-1] = '\0';
    box(win,0,0);
    wprintw(win,screen);
    wrefresh(win);
  }
  getch();
  endwin();
  return(0);

}
