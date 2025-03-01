#include <SDL3\SDL.h>

#include <iostream>
#include <string>

namespace parameters {
constexpr int scrnWidth{1000};
constexpr int scrnHeight{1000};
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
// loads an image surface
SDL_Surface* loadSurface(std::string filename);

int main(int argc, char* argv[]) {
  if (!initialize()) {
    std::cout << "\nINITIALIZATION ERROR";
  } else {
    if (!loadMedia()) {
      std::cout << "\nMEDIA LOAD ERROR";
    } else {
      using namespace windows;
      // initialize the stretched rectangle at (0,0) with dimensions of window
      SDL_Rect stretchRectangle{0,0,parameters::scrnWidth,parameters::scrnHeight};
      
      // stretch the image
      SDL_BlitSurfaceScaled(imageFile, NULL, windowSurface, &stretchRectangle, SDL_SCALEMODE_NEAREST);
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

  windows::imageFile = loadSurface("../img/t_border.bmp");

  if(windows::imageFile == NULL){
    std::cout << '\n' << SDL_GetError();
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

SDL_Surface* loadSurface(std::string filename) {
  SDL_Surface* optimizedSurface{nullptr};

  SDL_Surface* img = SDL_LoadBMP(filename.c_str());

  if (img == NULL) {
    std::cout << '\n' << SDL_GetError();
  }

  // convert the loaded img to screensurface format to avoid redundant blitting
  optimizedSurface = SDL_ConvertSurface(img, windows::windowSurface->format);

  // delete the unoptimized surface
  SDL_DestroySurface(img);

  return optimizedSurface;
}



