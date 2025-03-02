#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>

SDL_Window* g_mainWin{nullptr};

namespace paramaters {
int width{800};
int height{600};
}  // namespace paramaters

namespace render_texture {
// the main texture of the window
SDL_Renderer* mainRenderer{nullptr};
// the currently loaded back texture
SDL_Texture* currentTexture{nullptr};
}  // namespace render_texture

// initializes the SDL
bool init();
// closes the SDL
void close();
// Loads individual image as texture
SDL_Texture* loadTexture(std::string path);
// loads media, uses the above loadTexture() function
bool loadMedia();

int main(int argc, char* argv[]) {
  if (!init()) {
    std::cout << "\nINITIALIZATION ERROR";
  } else {
    if (!loadMedia()) {
      std::cout << "\nMEDIA LOAD ERROR";
    } else {
      // Hack to get window to stay up
      SDL_Event e;
      bool quit = false;
      while (!quit) {
        while (SDL_PollEvent(&e)) {
          if (e.type == SDL_QUIT) quit = true;
        }
        using namespace render_texture;
        SDL_RenderClear(mainRenderer);
        SDL_RenderCopy(mainRenderer, currentTexture, NULL, NULL);
        SDL_RenderPresent(mainRenderer);
      }
    }
  }
  close();
  return 0;
}

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) > 0) {
    std::cerr << "SDL INIT ERROR: " << SDL_GetError() << '\n';
    return false;
  }

  g_mainWin =
      SDL_CreateWindow("Test", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                       paramaters::width, paramaters::height, SDL_WINDOW_SHOWN);

  if (g_mainWin == NULL) {
    std::cerr << "SDL WINDOW MAKE ERROR: " << SDL_GetError() << '\n';
    return false;
  }

  using namespace render_texture;

  mainRenderer = SDL_CreateRenderer(g_mainWin, -1, SDL_RENDERER_ACCELERATED);

  if (mainRenderer == NULL) {
    std::cerr << "SDL RENDERER CREATION ERROR: " << SDL_GetError() << '\n';
    return false;
  }

  if (SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF) != 0) {
    std::cerr << "SDL RENDERER DRAW ERROR: " << SDL_GetError() << '\n';
    return false;
  }

  if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) == 0) {
    std::cerr << "SDL IMG INITIALIZATION ERRORl" << IMG_GetError() << '\n';
    return false;
  }

  return true;
}

void close() {
  // destroy texture surface
  SDL_DestroyTexture(render_texture::currentTexture);
  // destroy main renderer
  SDL_DestroyRenderer(render_texture::mainRenderer);
  // destroy main window
  SDL_DestroyWindow(g_mainWin);
  
  // set all to nullptr
  render_texture::mainRenderer = nullptr;
  g_mainWin = nullptr;
  render_texture::currentTexture = nullptr;

  // close SDL and SDL IMG
  IMG_Quit();
  SDL_Quit();
}

SDL_Texture* loadTexture(std::string path) {
  SDL_Texture* newTexture{nullptr};

  newTexture = IMG_LoadTexture(render_texture::mainRenderer, path.c_str());

  // SDL_Surface* temporarySurface{IMG_Load(path.c_str())};

  // if (temporarySurface == NULL) {
  //   std::cerr << IMG_GetError() << '\n';
  // } else {
  //   newTexture = SDL_CreateTextureFromSurface(render_texture::mainRenderer,
  //                                             temporarySurface);
  // }
  // SDL_FreeSurface(temporarySurface);

  return newTexture;
}

bool loadMedia() {
  using namespace render_texture;

  currentTexture = loadTexture("../img/4default.png");

  if (currentTexture == NULL) {
    std::cerr << "Error making texture: " << SDL_GetError() << '\n';
    return false;
  }

  return true;
}
