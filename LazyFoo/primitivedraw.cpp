#include <SDL2/SDL.h>

#include <iostream>

SDL_Window* g_mainWin{nullptr};
SDL_Renderer* g_mainRenderer{nullptr};

namespace paramaters {
int width{800};
int height{600};
}  // namespace paramaters

// initializes the SDL
bool init();
// closes the SDL
void close();

int main(int argc, char* argv[]) {
  if (!init()) {
    std::cerr << "INITIALIZATION ERROR.\n";
    close();
    return -1;
  }

  SDL_Event event;
  bool quit{false};
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) quit = true;
      // set color to white
      SDL_SetRenderDrawColor(g_mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
      // clear renderer with last set color
      SDL_RenderClear(g_mainRenderer);
      // namespace we will need for drawing
      using namespace paramaters;

      // draw a rectangle
      SDL_Rect fillRect{width / 4, height / 4, width / 2, height / 2};
      // set rendering color
      SDL_SetRenderDrawColor(g_mainRenderer, 0xFF, 0x00, 0x00, 0x00);  // red
      SDL_RenderFillRect(g_mainRenderer, &fillRect);

      // draw green outline
      SDL_Rect outline{width / 6, height / 6, width * 2 / 3, height * 2 / 3};
      SDL_SetRenderDrawColor(g_mainRenderer, 0x00, 0xFF, 0x00, 0x00);  // red
      SDL_RenderDrawRect(g_mainRenderer, &outline);

      //draw line
      SDL_SetRenderDrawColor(g_mainRenderer, 0x00, 0x00, 0xFF, 0x00);
      SDL_RenderDrawLine(g_mainRenderer, 0, height/2, width, height/2);

      // draw dotted lines
      SDL_SetRenderDrawColor(g_mainRenderer, 0x0, 0x00, 0x00, 0x00);
      for (int i = 0; i < height; i+=5)
      {
        SDL_RenderDrawPoint(g_mainRenderer, width/2, i);
        SDL_RenderDrawPoint(g_mainRenderer, width/4, i);
        SDL_RenderDrawPoint(g_mainRenderer, (width*3)/4, i);
      }

      // update renderer
      SDL_RenderPresent(g_mainRenderer);
    }
  }

  return 0;
}

bool init() {
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL Init Failure: " << SDL_GetError() << '\n';
    return false;
  }

  g_mainWin = SDL_CreateWindow("PRIMITIVE TEST", SDL_WINDOWPOS_UNDEFINED,
                               SDL_WINDOWPOS_UNDEFINED, paramaters::width,
                               paramaters::height, SDL_WINDOW_SHOWN);

  if (g_mainWin == NULL) {
    std::cerr << "SDL Window Creation Failure: " << SDL_GetError() << '\n';
    return false;
  }

  g_mainRenderer = SDL_CreateRenderer(g_mainWin, -1, SDL_RENDERER_ACCELERATED);

  if (g_mainRenderer == NULL) {
    std::cerr << "SDL Renderer Creation Failure: " << SDL_GetError() << '\n';
    return false;
  }

  if (SDL_SetRenderDrawColor(g_mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF) != 0) {
    std::cerr << "SDL RENDERER DRAW ERROR: " << SDL_GetError() << '\n';
    return false;
  }

  return true;
}

void close() {
  // destroy renderer
  SDL_DestroyRenderer(g_mainRenderer);
  g_mainRenderer = nullptr;
  // destroy window
  SDL_DestroyWindow(g_mainWin);
  g_mainWin = nullptr;

  // exit sdl
  SDL_Quit();
}
