#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <time.h>

#define red           0xFF0000
#define green         0x00FF00
#define blue          0x0000FF
#define sizeX_predef  800
#define sizeY_predef  600

SDL_Surface *screen;
int sizeX; //= 1024;
int sizeY; //= 576;

struct complex
{
  int real, img;
};

// void drawSet() {
//
//   int x0 = round(sizeX / 2);
//   int y0 = round(sizeY / 2);
//   int iteration = 0;
//   int max_iteration = 0;
//
//   double i = (iteration * 255 / maxIteration);
//   r = round(sin(0.024 * i + 0) * 127 + 128);
//   g = round(sin(0.024 * i + 2) * 127 + 128);
//   b = round(sin(0.024 * i + 4) * 127 + 128);
//
//   for (x = 0; x < sizeX; x++) {
//     for (y = 0; y < sizeY; y++) {
//
//     }
//   }
//
// }

void putpixel(int x, int y, int color)
{
  unsigned int *ptr = (unsigned int*)screen->pixels;
  int lineoffset = y * (screen->pitch / 4);
  ptr[lineoffset + x] = color;
}


void render()
{

  // Lock surface if needed
  if (SDL_MUSTLOCK(screen))
    if (SDL_LockSurface(screen) < 0)
      return;

  int x,y,color;
  //just set some pixels to red, white, green and blue:
  // x=30; y=40; putpixel(x, y, red);
  // x=35; y=40; putpixel(x, y, 0xFFFFFF);
  // x=35; y=45; putpixel(x, y, green);
  // x=40; y=45; putpixel(x, y, green);
  // x=45; y=50; putpixel(x, y, 0x000000);
  // x=50; y=55; putpixel(x, y, blue);
  for (x = 0; x < sizeX; x += 1) {
    for (y = 0; y < sizeY; y += 1) {
      color = rand() & 0xff;
      color |= (rand() & 0xff) << 8;
      color |= (rand() & 0xff) << 16;
      color |= (rand() & 0xff) << 24;
      putpixel(x, y, color);
    }
  }

  // Unlock if needed
  if (SDL_MUSTLOCK(screen))
      SDL_UnlockSurface(screen);

  // update the whole screen
  SDL_UpdateRect(screen, 0, 0, sizeX, sizeY);

}

int oneSetInitialization() {

  // Initialize SDL's subsystems - in this case, only video.
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
  {
    fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
    exit(1);
  }

  // Register SDL_Quit to be called at exit; makes sure things are
  // cleaned up when we quit.
  atexit(SDL_Quit);

  // Attempt to create a 640x480 window with 32bit pixels.
  screen = SDL_SetVideoMode(sizeX, sizeY, 32, SDL_SWSURFACE);

  // If we fail, return error.
  if ( screen == NULL )
  {
    fprintf(stderr, "Unable to set 640x480 video: %s\n", SDL_GetError());
    exit(1);
  }

  while (1) //loop forever
    {
    // Render stuff
    render();
    
    // Check for events.
    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
      switch (event.type)
      {
      case SDL_KEYDOWN:
        break;
      case SDL_KEYUP:
        // If escape is pressed, return (and thus, quit)
        if (event.key.keysym.sym == SDLK_ESCAPE)
          return 0;
        break;
      case SDL_QUIT:
        return 0;
      }
    }
  }

}

int main( int argc, char* args[] )
{
  int user_input;
  sizeX = sizeX_predef;
  sizeY = sizeY_predef;

  /* Asks user for window size. */
  // printf("Please give the dimension of the X axis: \n");
  // scanf("%d", &sizeX);
  // printf("Please give the dimension of the Y axis: \n");
  // scanf("%d", &sizeY);

  /* Loops until the user makes a valid choice */
  while(1) {
    printf("Please press 1 to see one set, or 2 to zoom in continusouly. Press 0 to exit.\n");
    scanf("%d", &user_input);
    if (user_input == 1) {
      oneSetInitialization();
      break;
    } else if (user_input == 2) {
      oneSetInitialization();
      break;
    } else if (user_input == 0) {
      printf("Exiting...\n");
      break;
    } else {
      printf("Invalid choice, please try again.\n");
    }
  }

  return 0;
}
