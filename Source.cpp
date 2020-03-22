#include <iostream>
#include "my_graphics.h"




int main(int argc, char* args[]) // end me pls
{
  //The window we'll be rendering to
  SDL_Window* window = NULL;

  //The renderer contained by the window
  SDL_Renderer* Screen = NULL;




  //Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    cout << "SDL could not initialize! SDL_Error: " << SDL_GetError() << endl;
  }
  else
  {
    //Create window
    window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
      cout << "Window could not be created! SDL_Error: " << SDL_GetError() << endl;
    }
    else
    {
      //Get window surface
      //Screen = SDL_CreateRenderer(window, -1, 0);

      //Fill the surface white
      //SDL_FillRect(Screen, NULL, SDL_MapRGB(Screen->format, 0xFF, 0xFF, 0xFF));
      line *a = new line(coordinates(), 0.1, 2, coordinates(-0.1, -0.1), 1, 0.3);// = line(coordinates(), coordinates(1, 1));
      scene S(window);
      S.PushObject(a); // FIND OUT WHATS HAPPENIN
      
    test:

      
      //Update the surface
      SDL_UpdateWindowSurface(window);
      SDL_RenderPresent(S.GetRen());
      //SDL_T
      // SDL_Up

       //Wait two seconds
      SDL_Delay(2000);
      while (1)
      {
        S.DrawScene();
        SDL_UpdateWindowSurface(window);
        SDL_RenderPresent(S.GetRen());
      }
    }
  }
  //Destroy window
  SDL_DestroyWindow(window);

  //Quit SDL subsystems
  SDL_Quit();


  //std::cin.get();
  return 0;
}