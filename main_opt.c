#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <SDL/SDL.h>
#include <time.h>

#define sizeX_predef  1280
#define sizeY_predef  800

SDL_Surface *screen;
int sizeX;
int sizeY;
int max_iteration = 500;
double scaleX, scaleY;
double zoom_factor = 1;
double offset = 0;

struct complex
{
  double real;
  double imag;
};



/* Converts the three r, g and b values to a single hexadecimal number. */
int createRGB(int r, int g, int b)
{
    return ((r & 0xff) << 16) + ((g & 0xff) << 8) + (b & 0xff);
}

void createColormap(double *prgb)
{
  int i, j;
  double r,g,b;
  for (i = 0; i < max_iteration; i++)
  {
    double j = (i * 255 / max_iteration);
    r = round(sin(0.2 * j + 0) * 127 + 128);
    g = round(sin(0.2 * j + 2) * 127 + 128);
    b = round(sin(0.2 * j + 4) * 127 + 128);
    *(prgb+i) = createRGB(r,g,b);
  }
}

void putpixel(int x, int y, int color)
{
  unsigned int *ptr = (unsigned int*)screen->pixels;
  int lineoffset = y * (screen->pitch / 4);
  ptr[lineoffset + x] = color;
}

int mandelbrotCalc(struct complex c)
{
  struct complex z = {0,0};
  double z_real_sq, z_imag_sq;
  int iteration = 0;

  z_real_sq = z.real * z.real;
  z_imag_sq = z.imag * z.imag;

  /* Comparing with 4 in order to avoid using sqrt */
  while ( (z_real_sq + z_imag_sq <= 4) && (iteration <= max_iteration) )
  {
    z.imag = z.real * z.imag;
    z.imag += z.imag;
    z.imag += c.imag;
    z.real = z_real_sq - z_imag_sq + c.real;
    z_real_sq = z.real * z.real;
    z_imag_sq = z.imag * z.imag;
    iteration++;
  }

  return iteration;
}

void drawSet(double zoom_factor, double offset, double *prgb)
{
  clock_t start = clock();
  /* (x,y) is the pixel coordinate with respect to the center (x0,y0)
  (xp,yp) is the pixel coordinate with respect to the image corner. */
  double xp, yp;
  int iteration = 0;
  struct complex c;
  int color;

  /* Complex plane scaling */
  double xp_max = ( 1.25 * scaleX - offset ) / zoom_factor;
  double xp_min = ( -1.75 * scaleX - offset ) / zoom_factor;
  double yp_max = ( 1.5 * scaleY - offset ) / zoom_factor;
  double yp_min = ( -1.5 * scaleY - offset ) / zoom_factor;

  // Lock surface if needed
  if (SDL_MUSTLOCK(screen))
    if (SDL_LockSurface(screen) < 0)
      return;

  for (xp = 0; xp < sizeX; xp++)
  {
    for (yp = 0; yp < sizeY; yp++)
    {
      /* Scaling */
      c.real = ( (xp_max - xp_min) * xp ) / sizeX + xp_min;
      c.imag = ( (yp_max - yp_min) * yp ) / sizeY + yp_min;

      // printf("x is: %f, y is: %f\n", x, y);
      iteration = mandelbrotCalc(c);
      // printf("iteration: %d\n", iteration);

      /* Define the color sets that are going to be used. */
      if (iteration > max_iteration) {
        color = 0x000000;
      } else {
        color = *(prgb+iteration);
      }
      putpixel(xp,yp,color);
      iteration = 0;
    }
  }

  // Unlock if needed
  if (SDL_MUSTLOCK(screen))
      SDL_UnlockSurface(screen);

  // update the whole screen
  SDL_UpdateRect(screen, 0, 0, sizeX, sizeY);
  clock_t end = clock();
  clock_t millis = end - start;
  printf("Elapsed time: %lu\n", millis);
}


int initialization(int user_input, double *prgb)
{
  // Initialize SDL's subsystems - in this case, only video.
  if ( SDL_Init(SDL_INIT_VIDEO) < 0 )
  {
    fprintf(stderr, "Unable to init SDL: %s\n", SDL_GetError());
    exit(1);
  }

  // Register SDL_Quit to be called at exit; makes sure things are
  // cleaned up when we quit.
  atexit(SDL_Quit);

  // Attempt to create a sizeXxsizeY window with 32bit pixels.
  screen = SDL_SetVideoMode(sizeX, sizeY, 32, SDL_SWSURFACE);

  // If we fail, return error.
  if ( screen == NULL )
  {
    fprintf(stderr, "Unable to set video: %s\n", SDL_GetError());
    exit(1);
  }

  offset = 0;
  zoom_factor = 1;
  while (1) //loop forever
  {
    if (user_input == 1) {
      drawSet(zoom_factor, offset, prgb);
    } else if (user_input == 2) {
      drawSet(zoom_factor, offset, prgb);
      zoom_factor += 0.5;
      offset += 0.28;//0.15 with zoom factor 0.25;
    }

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
  double *prgb = (double *) malloc(max_iteration * sizeof(double));
  sizeX = sizeX_predef;
  sizeY = sizeY_predef;

  /* Asks user for window size. */
  // printf("Please give the dimension of the X axis: \n");
  // scanf("%d", &sizeX);
  // printf("Please give the dimension of the Y axis: \n");
  // scanf("%d", &sizeY);

  if (sizeX > sizeY) {
      scaleX = (double) sizeX / sizeY;
      scaleY = 1;
  } else if (sizeX < sizeY) {
      scaleX = 1;
      scaleY = (double) sizeY / sizeX;
  } else {
      scaleX = 1;
      scaleY = 1;
  }

  createColormap(prgb);

  /* Loops until the user makes a valid choice */
  while(1) {
    printf("Please press 1 to see one set, or 2 to zoom in continusouly. Press 0 to exit.\n");
    scanf("%d", &user_input);
    if (user_input == 1) {
      initialization(user_input, prgb);
      break;
    } else if (user_input == 2) {
      initialization(user_input, prgb);
      break;
    } else if (user_input == 0) {
      printf("Exiting...\n");
      break;
    } else {
      printf("Invalid choice, please try again.\n");
    }
  }
  free(prgb);

  return 0;
}
