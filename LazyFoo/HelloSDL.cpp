#include <SDL2/SDL.h>

#include <iostream>

namespace parameters {
constexpr int scrnWidth{800};
constexpr int scrnHeight{600};
}  // namespace parameters

int main(int argc, char* argv[]) {
  // the window our stuff will be rendered to
  SDL_Window* window{nullptr};

  // the surface/image in/on the window
  SDL_Surface* screenSurface{nullptr};

  // initialize the video and check for success
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "Error Initializing SDL:\n" << SDL_GetError();
  }
  // safe case if initializing success
  else {
    window = SDL_CreateWindow("Test Window", SDL_WINDOWPOS_UNDEFINED,
                              SDL_WINDOWPOS_UNDEFINED, parameters::scrnWidth,
                              parameters::scrnHeight, SDL_WINDOW_SHOWN);

    // check if window had been created successfully
    if (window == NULL) {
      std::cout << "Error opening window: " << SDL_GetError();
    }
    // if not, we can safely start drawing
    else {
      // get the surface so we cant start the drawing
      screenSurface = SDL_GetWindowSurface(window);
      // fill the surface white
      SDL_FillRect(screenSurface, NULL,
                   SDL_MapRGB(screenSurface->format, 0xFA, 0xFB, 0x98));
      // update the color fill onto the window
      SDL_UpdateWindowSurface(window);
      // keep the window up

      // Hack to get window to stay up
      SDL_Event e;
      bool quit = false;
      while (quit == false) {
        while (SDL_PollEvent(&e)) {
          if (e.type == SDL_QUIT) quit = true;
        }
      }
      //hack end
      //----------------
    }
  }

  // delete the window
  SDL_DestroyWindow(window);

  // De-initialize SDL
  SDL_Quit();

  return 0;
}
