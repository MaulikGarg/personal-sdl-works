#include <SDL3\SDL.h>

#include <iostream>
#include <string>

namespace parameters {
constexpr int scrnWidth{800};
constexpr int scrnHeight{600};
}  // namespace parameters

namespace windows {
// the main SDL Window
SDL_Window* mainWindow{nullptr};

// the window's main surface
SDL_Surface* windowSurface{nullptr};

// the OPENED image file pointer
SDL_Surface* imageFile{nullptr};
}  // namespace windows

// initializes the SDL
bool initialize();
// loads an image or other media
bool loadMedia();
// closes down the SDL window
void Close();

int main(int argc, char* argv[]) {
  if (!initialize()) {
    std::cout << "\nINITIALIZATION ERROR";
  } else {
    if (!loadMedia()) {
      std::cout << "\nMEDIA LOAD ERROR";
    } else {
      using namespace windows;
      SDL_BlitSurface(imageFile, NULL, windowSurface, NULL);
      SDL_UpdateWindowSurface(mainWindow);

      // Hack to get window to stay up
      SDL_Event e;
      bool quit = false;
      while (!quit) {
        while (SDL_PollEvent(&e)) {
          if (e.type == SDL_EVENT_QUIT) quit = true;
        }
      }
    }
  }
  Close();
  return 0;
}

bool initialize() {
  bool isSuccess{
      false};  // bool to determine if window initialization is success

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Error initializing SDL Window: " << SDL_GetError();
  } else {
    using namespace windows;
    using namespace parameters;
    // attemp to make our window
    mainWindow = SDL_CreateWindow("Test2", scrnWidth,
                                  scrnHeight, 0);

    // check for window success
    if (mainWindow == NULL) {
      std::cout << "Error Creating Window: " << SDL_GetError();
    } else {
      windowSurface = SDL_GetWindowSurface(mainWindow);
      // EVERYTHING OK, CAN SET SUCCESS
      isSuccess = true;
    }
  }
  return isSuccess;
}

bool loadMedia() {
  // success indicator flag
  bool isSuccess{true};

  // get the base path
  std::string basePath {SDL_GetBasePath()};

  std::cout << "Opening file at " << basePath << '\n';
  // try to open the image
  windows::imageFile = SDL_LoadBMP("..\\/img/test.bmp");

  // check if image file was loaded
  if (windows::imageFile == NULL) {
    std::cout << "Error opening the image file: " << SDL_GetError();
    isSuccess = false;
  }

  return isSuccess;
}

void Close() {
  using namespace windows;

  // de allocate surface memory
  SDL_DestroySurface(imageFile);
  imageFile = nullptr;

  // destroy the opened window
  SDL_DestroyWindow(mainWindow);
  mainWindow = nullptr;

  // Close the SDL
  SDL_Quit();
}
