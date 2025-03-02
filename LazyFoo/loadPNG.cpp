#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <string>

SDL_Window* g_mainWindow{nullptr};

namespace parameters {
constexpr int width{800};
constexpr int height{600};
}  // namespace parameters

namespace surface {
SDL_Surface* mainSurface{nullptr};
SDL_Surface* imgSurface{nullptr};  // back buffer image
}  // namespace surface

void close();
bool init();
SDL_Surface* loadSurface(std::string path);
bool loadMedia();

int main(int argc, char* argv[]) {
  if (!init()) {
    std::cerr << "ERROR INITIALIZING SDL\n";
  } else {
    if (!loadMedia()) {
      std::cerr << "ERROR LOADING MEDIA\n";
    } else {
      SDL_BlitSurface(surface::imgSurface, NULL, surface::mainSurface, NULL);
      SDL_UpdateWindowSurface(g_mainWindow);

      // Hack to get window to stay up
      SDL_Event e;
      bool quit = false;
      while (quit == false) {
        while (SDL_PollEvent(&e)) {
          if (e.type == SDL_QUIT) quit = true;
        }
      }
    }
  }
  close();
  return 0;
}

bool loadMedia() {
  bool isSuccess{true};

  // try to open the image
  surface::imgSurface = loadSurface("../img/4default.png");

  if (surface::imgSurface == NULL) {
    std::cerr << SDL_GetError() << '\n';
    isSuccess = false;
  }

  return isSuccess;
}

SDL_Surface* loadSurface(std::string path) {
  SDL_Surface* optimized {nullptr};

  SDL_Surface* temp {IMG_Load(path.c_str())};
  if(temp == nullptr){
    std::cerr << '\n' << IMG_GetError();  
  } else {
    optimized = SDL_ConvertSurface(temp, surface::mainSurface->format, 0);
    SDL_FreeSurface(temp);
  }
  return optimized;
}

void close() {
  // delete main window
  SDL_DestroyWindow(g_mainWindow);

  // delete image surface
  SDL_FreeSurface(surface::imgSurface);

  // de initalize the SDL
  SDL_Quit();
}

bool init() {
  bool isSuccess{false};

  // initialize the SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "\n" << SDL_GetError();
  } else {
    // make window
    g_mainWindow = SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED,
                                    SDL_WINDOWPOS_UNDEFINED, parameters::width,
                                    parameters::height, SDL_WINDOW_SHOWN);
  }

  if (g_mainWindow == NULL) {
    std::cerr << "\n" << SDL_GetError();
  } else {
    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
      std::cerr << "\n" << SDL_GetError();
    } else {
      surface::mainSurface = SDL_GetWindowSurface(g_mainWindow);
      isSuccess = true;
    }
  }

  return isSuccess;
}
