//Made by Valtteri Ahlstr�m

#include <string>
#include <cstdlib>
#include <sstream>
#include "SDL/SDL.h"
#include "SDL/SDL_image.h"
#include "SDL/SDL_ttf.h"
//#include "SDL/SDL_mixer.h"
#include "Timer.h"
#include <iostream>

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

SDL_PixelFormat *pxlformat = NULL;

//Timer
Timer myTimer;

//TTF Fonts
//TTF_Font *font = NULL;

//Font Colors
//SDL_Color fontColorWhite = {255, 255, 255};

//The sound effect

//The event structure that will be used
SDL_Event event;

//Variables
bool quit;
double BallX;
double BallY;
double prevBallY;
double prevBallX;
double MelaX;
double BallSpeedX;
double BallSpeedY;
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
bool checkBallCollision();
void drawBall();
void drawMela();
SDL_Surface *load_image(string filename);

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
  screen = SDL_SetVideoMode(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_BPP, SCREENMODE);
  
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
  SDL_WM_SetCaption("oo00 Balls 00oo", NULL);
  
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
  SDL_Rect rect;
  rect.x = 300;
  rect.y = 300;
  rect.w = 200;
  rect.h = 200;
  SDL_FillRect(screen, &rect, 0x000000);
  
}

bool loadFiles()
{
  //Load the image
  background = load_image("background.png");
  mela = load_image("mela.png");
  ball = load_image("ball.png");
  
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
      MelaX = 375 + 40;
      return false;
    }
  else if (x >= 375 + 40 && x <= 375 + 520 - 40)
    {
      SDL_ShowCursor(SDL_ENABLE);
      MelaX = x;
      return true;
    }
  else
    {
      SDL_ShowCursor(SDL_ENABLE);
      MelaX = 375 + 520 - 40;
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
  bool returnValue = false;

  if ((int)tmpY >= 1 || (int)tmpY <= -1)
    {
      BallY = BallY + (int)tmpY;
      //std::cout << "BallY: ";
      //std::cout << BallY << std::endl;
      time1Y = time2Y;
      returnValue = true;
    }

  if ((int)tmpX >= 1 || (int)tmpX <= -1)
    {
      BallX = BallX + (int)tmpX;
      time1X = time2X;
      returnValue = true;
    }
    
  return returnValue;
}

void drawBall()
{
  applySurface(375 + (int)BallX, 620 - (int)BallY, ball, screen);
}

void drawMela()
{
  applySurface((int)MelaX - 80/2, 630, mela, screen);
}

bool checkBallCollision()
{
  if (BallX <= 0 || BallX >= 520 - 5)
    {
      BallSpeedX = BallSpeedX * -1;
      return true;
    }
  
  if (BallY >= 545)
    {
      BallSpeedY = BallSpeedY * -1;
      return true;
    }

  int tmpInt = (int)MelaX - (int)BallX + 375;
  if (BallY <= 5 && BallY > 0 && abs(tmpInt) <= 100)
    {
      BallSpeedY = BallSpeedY * -1;
      return true;
    }

  return false;
}

void gameOn()
{

  //Set initial values
  BallSpeedX = 240;
  BallSpeedY = 240;
  BallX = 520/2;
  BallY = 0;
  prevBallX = BallX;
  prevBallY = BallY;
  MelaX = 375 + 520/2;


  //Apply the background to the screen
  applySurface(0, 0, background, screen);
  applySurface(375, 75, playArea, screen);
  applySurface((int)MelaX - 80/2, 630, mela, screen);
  applySurface(375 + 520/2 - 10/2, 620, ball, screen);
  applySurface(375 + (int)BallX, 620 - (int)BallY, ball, screen);
  
  //Apply texts to the screen
  //print_text("NEXT", nextText, 800, 85, true, fontColorWhite, false, font, false);
  
  //clear screen
  
  //Update the screen
  SDL_Flip(screen);
  
  //Start Time
  myTimer.start();
  time1Y = myTimer.get_ticks();
  time1X = myTimer.get_ticks();
  
  //While the user hasn't quit
  while (quit == false)
    {
      handleEvents();
      time2Y = myTimer.get_ticks();
      time2X = myTimer.get_ticks();
      if (calculateBallPosition() == true)
	{

	  if (checkBallCollision() == true)
	    {
	
	    }

	  applySurface(375, 75, playArea, screen);
	  drawBall();
	  drawMela();
	  SDL_Flip(screen);
	}
    } //quit == false && game_over == false
  
  myTimer.stop();
  
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
