//Made by Valtteri Ahlstr√∂m

#include <string>
#include <cstdlib>
#include <sstream>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
//#include "SDL/SDL_mixer.h"
#include "Timer.h"
#include <iostream>
#include <cmath>

#define SCREENMODE SDL_SWSURFACE
#define SURFACETYPE SDL_SWSURFACE

using namespace std;

//The attributes of the screen
const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 750;
const int SCREEN_BPP = 32;

//The surfaces that will be used
SDL_Surface *background = NULL;
SDL_Surface *screen = NULL;
SDL_Surface *playArea = NULL;
SDL_Surface *mela = NULL;
SDL_Surface *ball = NULL;
SDL_Surface *matrix[17][13];
SDL_Surface *blocksMatrix = NULL;

//Clipping rectangles
SDL_Rect clip[12];

SDL_PixelFormat *pxlformat = NULL;

//Timer
Timer myTimerX;
Timer myTimerY;

//TTF Fonts
//TTF_Font *font = NULL;

//Font Colors
//SDL_Color fontColorWhite = {255, 255, 255};

//The sound effect

//The event structure that will be used
SDL_Event event;

//Variables
bool quit;
int BallX;
int BallY;
int prevBallX;
int prevBallY;
int MelaX;
int prevMelaX;
double BallSpeedX;
double BallSpeedY;
double BallSpeedResultant;
Uint32 time1Y = 0;
Uint32 time2Y = 0;
double tmpTime1Y;
double tmpTime2Y;
Uint32 time1X = 0;
Uint32 time2X = 0;
double tmpTime1X;
double tmpTime2X;
double tmpY = 0;
double tmpX = 0;
int intX = 0;
int intY = 0;
const double PI = 3.141592;
int matrixColor[17][13];


//Methods and functions
bool init();
bool loadFiles();
void applySurface(int x, int y, SDL_Surface* source, SDL_Surface* destination);
void clearScreen();
void clearPlayArea(); //todo
void loadMenu(); //todo
void gameOn();
void printText(string text, SDL_Surface *surface, int X, int Y, bool justified, SDL_Color fontColor, bool clear, TTF_Font *fontti, bool menu);
void handleEvents();
bool calculateMelaPosition(int x);
bool calculateBallPosition();
bool checkBallCollision(int x, int y, bool checkX);
void drawBall();
void drawMela();
SDL_Surface *load_image(string filename);
void fillMatrix(int row, int col);
void clipBlocks();
void applyBlock(int row, int col, SDL_Rect *clip);
void loadLevel(int lvl);
void explodeBlock(int row, int col);

SDL_Surface *load_image(string filename)
{
  //Temporary storage for the image that's loaded
  SDL_Surface* loadedImage = NULL;
  //The optimized image that will be used
  SDL_Surface* optimizedImage = NULL;
  //Load the image
  loadedImage = IMG_Load(filename.c_str());
  //If nothing went wrong in loading the image

  if(loadedImage != NULL)
    {
      //Create an optimized image
      //optimizedImage = SDL_DisplayFormat(loadedImage);
      //optimizedImage = SDL_ConvertSurface(loadedImage, screen->format, SDL_HWSURFACE|SDL_DOUBLEBUF);
      SDL_PixelFormat format = {NULL, 32, 4, 0, 0, 0, 0, 0, 8, 16, 24, 0x000000FF, 0x0000FF00, 0x00FF0000, 0xFF000000, 0, 255};
      optimizedImage = SDL_ConvertSurface(loadedImage, &format, 0);
      //Free the old image
      SDL_FreeSurface(loadedImage);
    }

  //Return the optimized image
  return optimizedImage;
}

bool init()
{
  //Initialize all SDL subsystems
  if(SDL_Init(SDL_INIT_EVERYTHING) == -1)
    {
      return false;
    }

  //Set up the screen
  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SDL_FULLSCREEN | SCREENMODE);

  //If there was an error in setting up the screen
  if(screen == NULL)
    {
      return false;
    }

  //Define playArea
  pxlformat = screen->format;
  playArea = SDL_CreateRGBSurface(SURFACETYPE, 520, 600, pxlformat->BitsPerPixel, 0, 0, 0, 0);

  //clear_playArea(0);

  //Check if playArea is still NULL
  if (playArea == NULL)
    return false;

  //Set the window caption
  SDL_WM_SetCaption("Balls", NULL);

  /*
  //Set font
  if (TTF_Init() == -1)
  return false;

  font = TTF_OpenFont("NASALIZA.TTF", 24);
  fontMenu = TTF_OpenFont("NASALIZA.TTF", 36);
  fontGameOver = TTF_OpenFont("NASALIZA.TTF", 52);
  if (font == NULL || fontMenu == NULL || fontGameOver == NULL)
  return false;
  */

  //Random number generator seed
  srand(time(0));

  /*
  //Initialize SDL_mixer
  if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 2, 4096) == -1

  return false

  */

  for (int row = 0; row < 10; row++)
    {
      for (int col = 0; col < 13; col++)
	{
	  matrix[row][col] = NULL;
	  matrixColor[row][col] = 0;
	}
    }

  clipBlocks();

  //If everything initialized fine
  return true;
}

void applySurface(int x, int y, SDL_Surface* source, SDL_Surface* destination)
{
  //Make a temporary rectangle to hold the offsets
  SDL_Rect offset;
  //Give the offsets to the rectangle
  offset.x = x;
  offset.y = y;
  //Blit the surface
  SDL_BlitSurface(source, NULL, destination, &offset);
}

void clearScreen()
{
  SDL_Rect rect;
  rect.x = 0;
  rect.y = 0;
  rect.w = SCREEN_WIDTH;
  rect.h = SCREEN_HEIGHT;
  SDL_FillRect(screen, &rect, 0x888888);
}

void clearPlayArea()
{
  int row;
  int col;

  for (row = 0; row < 17; row++)
    {
      for (col = 0; col < 13; col++)
	{
	  applyBlock(row, col, &clip[matrixColor[row][col]]);
	}
    }
}

bool loadFiles()
{
  //Load the image
  background = load_image("background.png");
  mela = load_image("mela.png");
  ball = load_image("ball.png");
  blocksMatrix = load_image("blocks.png");

  //If there was an error in loading the image
  if(background == NULL || mela == NULL || ball == NULL)
    return false;

  //If everything loaded fine
  return true;
}

void clean_up()
{
  //Free the image
  SDL_FreeSurface(background);
  SDL_FreeSurface(playArea);
  SDL_FreeSurface(mela);
  SDL_FreeSurface(ball);
  //SDL_FreeSurface(matrix);

  //Close fonts
  //TTF_CloseFont(font);

  //Quit TTF
  //TTF_Quit();

  //Free the sound effect
  //Mix_FreeChunk(bang1);

  //Quit SDL_mixer
  //Mix_CloseAudio();

  //Quit SDL
  SDL_Quit();
}

void fillMatrix(int row, int col)
{
  SDL_Rect tmpRect;

  tmpRect.x = 40 * col;
  tmpRect.y = 0 + 20*row;
  tmpRect.w = 40;
  tmpRect.h = 20;

  SDL_FillRect(playArea, &tmpRect, 0x666666);

  tmpRect.x = 40 * col + 2;
  tmpRect.y = 0 + 20 * row + 2;
  tmpRect.w = 36;
  tmpRect.h = 36;

  SDL_FillRect(playArea, &tmpRect, matrixColor[row][col]);
}

void print_text(string text, SDL_Surface *surface, int X, int Y, bool justified, SDL_Color fontColor, bool clear, TTF_Font *fontti, bool menu)
{
  const char *p;
  p = text.c_str();

  surface = TTF_RenderText_Solid(fontti, p, fontColor);

  if (clear == true)
    {
      SDL_Rect rect;
      if (menu == false)
	{
	  rect.x = 90;
	  rect.w = 200;
	}
      else
	{
	  rect.x = 0;
	  rect.w = SCREEN_WIDTH;
	}

      rect.y = Y;
      rect.h = surface->h;
      SDL_FillRect(screen, &rect, 0x000000);
    }

  if (justified == true)
    {
      applySurface(X - surface->w/2, Y, surface, screen);
    }
  else
    {
      applySurface(X, Y, surface, screen);
    }
}

bool calculateMelaPosition(int x)
{
  if (x > 0 && x < 375 + 40)
    {
      SDL_ShowCursor(SDL_ENABLE);
      MelaX = 40;
      return false;
    }
  else if (x >= 375 + 40 && x <= 375 + 520 - 40)
    {
      SDL_ShowCursor(SDL_ENABLE);
      MelaX = 40 + x - (375 + 40);
      return true;
    }
  else
    {
      SDL_ShowCursor(SDL_ENABLE);
      MelaX = 520 - 40;
      return false;
    }

}

void handleEvents()
{
  SDL_Delay(1);
  if (SDL_PollEvent(&event))
    {
      if(event.type == SDL_KEYDOWN)
	{
	  if (event.key.keysym.sym == SDLK_ESCAPE)
	    {
	      quit = true;
	    }
	}

      else if(event.type == SDL_QUIT)
	{
	  //Quit the program
	  quit = true;
	}

      else if (event.type == SDL_MOUSEMOTION)
	{
	  if (calculateMelaPosition(event.motion.x) == true)
	    {
	      drawMela();
	    }
	}
    }
}

bool calculateBallPosition()
{
  tmpTime1Y = time1Y;
  tmpTime2Y = time2Y;
  tmpTime1X = time1X;
  tmpTime2X = time2X;
  tmpY = (tmpTime2Y - tmpTime1Y) / 1000 * BallSpeedY;
  tmpX = (tmpTime2X - tmpTime1X) / 1000 * BallSpeedX;

  if (tmpY < 0)
    intY = (int)ceil(tmpY);
  else
    intY = (int)floor(tmpY);

  if (tmpX < 0)
    intX = (int)ceil(tmpX);
  else
    intX = (int)floor(tmpX);

  bool returnValue = false;
  bool collision = false;
  int i = 0;

  //hoplaa jatka t‰st‰
  if (intY >= 1 || intY <= -1)
    {
      for (i = 1; i <= abs(intY); i++)
	{
	  if (BallSpeedY > 0)
	    {
	      if (checkBallCollision(BallX, BallY + i, false) == true)
		{
		  collision = true;
		  BallY = BallY + i;
		  break;
		}
	    }
	  else
	    {
	      if (checkBallCollision(BallX, BallY - i, false) == true)
		{
		  collision = true;
		  BallY = BallY - i;
		  break;
		}
	    }
	}

      if (collision == false)
	{
	  BallY = BallY + (int)tmpY;
	}

      time1Y = time2Y;
      returnValue = true;
    }

  collision = false;
  if ((int)tmpX >= 1 || (int)tmpX <= -1)
    {
      for (i = 1; i <= abs((int)tmpX); i++)
	{
	  if (BallSpeedX > 0)
	    {
	      if (checkBallCollision(BallX + i, BallY, true) == true)
		{
		  collision = true;
		  BallX = BallX + i;
		  break;
		}
	    }
	  else
	    {
	      if (checkBallCollision(BallX - i, BallY, true) == true)
		{
		  collision = true;
		  BallX = BallX - i;
		  break;
		}
	    }
	}

      if (collision == false)
	{
	  BallX = BallX + (int)tmpX;
	}

      time1X = time2X;
      returnValue = true;
    }

  return returnValue;
}

void drawBall()
{
  SDL_Rect rect;
  rect.w = 12;
  rect.h = 12;
  rect.x = 375 + prevBallX - 6;
  rect.y = 618 - prevBallY;
  SDL_FillRect(screen, &rect, 0x000000);
  prevBallY = BallY;
  prevBallX = BallX;
  applySurface(375 + BallX - 6, 618 - BallY, ball, screen);
  //  std::cout << "M: " << MelaX << " B: " << BallX << " D: " << abs(MelaX - BallX) << std::endl;
}

void drawMela()
{
  SDL_Rect rect;
  rect.w = 80;
  rect.h = 13;
  rect.x = 375 + prevMelaX - 40;
  rect.y = 630;
  SDL_FillRect(screen, &rect, 0x000000);
  prevMelaX = MelaX;
  applySurface(375 + MelaX - 40, 630, mela, screen);
}

bool checkBallCollision(int x, int y, bool checkX)
{
  int tmpRow;
  int tmpCol;
  tmpRow = (540 - y) / 20;
  tmpCol = x / 40;

  if (checkX == true)
    {
      if (x <= 0 + 6 || x > (520 - 12)) //hits side walls
	{
	  BallSpeedX = BallSpeedX * -1;
	  return true;
	}

      if (y >= 540 - 17 * 20) //hits block left or right
	{
	  if (matrixColor[tmpRow][tmpCol] != 0)
	    {
	      explodeBlock(tmpRow, tmpCol);
	      BallSpeedX = BallSpeedX * -1;
	      return true;
	    }
	}
    }
  else
    {
      if (y >= 540 && BallSpeedY > 0) //hits upper wall
	{
	  BallSpeedY = BallSpeedY * -1;
	  return true;
	}

      if (y >= 540 - 17 * 20) //hits block top or bottom
	{
	  if (matrixColor[tmpRow][tmpCol] != 0)
	    {
	      explodeBlock(tmpRow, tmpCol);
	      BallSpeedY = BallSpeedY * -1;
	      return true;
	    }
	}

      double dif = MelaX - x;
      if (y == 0 && abs(dif) <= 40 + 6 && BallSpeedY < 0)
	{
	  BallSpeedResultant = sqrt(pow(BallSpeedX,2) + pow(BallSpeedY,2));
	  if (dif > 0) //Ball hits on the left side of Mela
	    {
	      if (dif > 40)
		{
		  BallSpeedX = -1 * BallSpeedResultant / sqrt(1 + pow(tan((90 - 80) / 180 * PI),2));
		  BallSpeedY = sqrt(pow(BallSpeedResultant,2) - pow(BallSpeedX,2));
		}
	      else
		{
		  BallSpeedX = -1 * BallSpeedResultant / sqrt(1 + pow(tan((90 - dif * 2) / 180 * PI),2));
		  BallSpeedY = sqrt(pow(BallSpeedResultant,2) - pow(BallSpeedX,2));
		}

	    }

	  if (dif < 0) //Ball hits on the right side of Mela
	    {
	      if (dif < -40)
		{
		  BallSpeedX = BallSpeedResultant / sqrt(1 + pow(tan((90 - 80) / 180 * PI),2));
		  BallSpeedY = sqrt(pow(BallSpeedResultant,2) - pow(BallSpeedX,2));
		}
	      else
		{
		  BallSpeedX = BallSpeedResultant / sqrt(1 + pow(tan((90 - abs(dif) * 2) / 180 * PI),2));
		  BallSpeedY = sqrt(pow(BallSpeedResultant,2) - pow(BallSpeedX,2));
		}

	    }

	  if (dif == 0) //Ball hits the center of Mela
	    {
	      BallSpeedX = 0;
	      BallSpeedY = BallSpeedResultant;
	    }

	  time1X = time2X;
	  return true;
	}
    }

  return false;
}

void explodeBlock(int row, int col)
{
  switch (matrixColor[row][col])
    {
    case 0:
      break;
    case 1:
    case 2:
    case 3:
    case 4:
    case 5:
    case 6:
    case 7:
    case 8:
    case 10:
      matrixColor[row][col] = 0;
      break;
    case 9:
      matrixColor[row][col] = 10;
      break;
    default:
      break;
    }
  applyBlock(row, col, &clip[matrixColor[row][col]]);
  applySurface(375, 75, playArea, screen);
}

void clipBlocks()
{
  for (int i = 0; i < 12; i++)
    {
      clip[i].x = 0;
      clip[i].y = i * 20;
      clip[i].w = 40;
      clip[i].h = 20;
    }
}

void applyBlock(int row, int col, SDL_Rect *clip)
{
  SDL_Rect tmpRect;

  tmpRect.x = col * 40;
  tmpRect.y = row * 20;
  tmpRect.w = 40;
  tmpRect.h = 20;

  SDL_BlitSurface(blocksMatrix, clip, playArea, &tmpRect);
}

void loadLevel(int lvl)
{
  switch (lvl)
    {
    case 1:
      for (int i = 0; i < 13; i++)
	{
	  matrixColor[4][i] = 9;
	  matrixColor[5][i] = 5;
	  matrixColor[6][i] = 6;
	  matrixColor[7][i] = 8;
	  matrixColor[8][i] = 7;
	  matrixColor[9][i] = 4;
	}
      break;
    }

  clearPlayArea();
}

void gameOn()
{

  //Set initial values
  BallSpeedX = 0;
  BallSpeedY = 360;
  BallSpeedResultant = sqrt(pow(BallSpeedX,2) + pow(BallSpeedY,2));
  BallX = 520/2;
  prevBallX = BallX;
  BallY = 1;
  prevBallY = BallY;
  MelaX = 520/2;
  prevMelaX = MelaX;

  //Apply the background to the screen
  applySurface(0, 0, background, screen);
  applySurface(375, 75, playArea, screen);
  applySurface(375 + MelaX, 630, mela, screen);
  applySurface(375 + BallX - 12/2, 618, ball, screen);

  //Apply texts to the screen
  //print_text("NEXT", nextText, 800, 85, true, fontColorWhite, false, font, false);

  //set matrix colors
  loadLevel(1);
  applySurface(375, 75, playArea, screen);

  //Update the screen
  SDL_Flip(screen);

  //Start Time
  myTimerX.start();
  myTimerY.start();
  time1Y = myTimerY.get_ticks();
  time1X = myTimerX.get_ticks();

  //While the user hasn't quit
  while (quit == false)
    {
      handleEvents();
      time2Y = myTimerY.get_ticks();
      time2X = myTimerX.get_ticks();
      if (calculateBallPosition() == true)
	{
	  //applySurface(375, 75, playArea, screen);
	  drawBall();
	  drawMela();
	  SDL_Flip(screen);
	}
    } //quit == false && game_over == false

  myTimerX.stop();
  myTimerY.stop();

  //If the user has Xed out the window

}

int main(int argc, char* args[])
{
  //Initialize
  if(init() == false)
    {
      return 1;
    }

  //Load the files
  if(loadFiles() == false)
    {
      return 1;
    }

  //Load the menu
  while (quit == false)
    {
      gameOn();
    }

  //Free the surface and quit SDL
  clean_up();

  return 0;
}
