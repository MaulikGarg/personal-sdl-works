#include <SDL2/SDL.h>

#include <iostream>
#include <string>
#include <vector>

namespace parameters {
constexpr int scrnWidth{800};
constexpr int scrnHeight{600};
}  // namespace parameters

// used to sync key press with an image
enum keyPressImageLink {
  keyPressLink_default,
  keyPressLink_up,
  keyPressLink_down,
  keyPressLink_right,
  keyPressLink_left,
  keyPressLink_max
};

// window information pointers
namespace windowsData {
// the main window to open
SDL_Window* mainWindow{nullptr};
// primary rendering surface
SDL_Surface* mainSurf{nullptr};
// surface of the currently back buffered image
SDL_Surface* imgSurf{nullptr};
// synced array for keypress images
SDL_Surface* keyPressImg[keyPressLink_max];

}  // namespace windowsData

// initialize the window
bool initialize();

// close the media and the window
void close();

// load all the image files
bool loadMedia();

// update the image surf based on keypress
void updateImageSurf(const SDL_Event& event);

// loads a specific image
SDL_Surface* loadSurface(std::string&&);

int main(int argc, char* argv[]) {
  if (!initialize()) {
    std::cout << "\nINITIALIZATION ERROR";
  } else {
    if (!loadMedia()) {
      std::cout << "\nMEDIA LOAD ERROR";
    }
    // media has been loaded, we can do the loading.
    else {
      bool quit{false};
      SDL_Event event;
      // set current window to default
      windowsData::imgSurf = windowsData::keyPressImg[keyPressLink_default];
      // get events and loops
      while (!quit) {
        while (SDL_PollEvent(&event) != 0) {
          // check if user wishes to exit the program
          if (event.type == SDL_QUIT) {
            quit = true;
          }
          // the user presses a key
          else if (event.type == SDL_KEYDOWN) {
            updateImageSurf(event);
          }
          SDL_BlitSurface(windowsData::imgSurf, NULL, windowsData::mainSurf,
                          NULL);
          SDL_UpdateWindowSurface(windowsData::mainWindow);
        }
      }
    }
  }
  close();
  return 0;
}

bool initialize() {
  // success flag
  bool isSuccess{false};

  // attempt to initialize the VIDEO SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cout << "Error initializing SDL Window: " << SDL_GetError();
  }
  // OK, Now attempt to draw the window
  else {
    using namespace windowsData;
    using namespace parameters;
    mainWindow =
        SDL_CreateWindow("UP DOWN LEFT RIGHT UWU", SDL_WINDOWPOS_UNDEFINED,
                         SDL_WINDOWPOS_UNDEFINED, scrnWidth, scrnHeight, 0);

    // check if window creation was success
    if (mainWindow == NULL) {
      std::cout << "Error creating window: " << SDL_GetError();
    }
    // OK, Now make main window surface
    else {
      mainSurf = SDL_GetWindowSurface(mainWindow);
      isSuccess = true;
    }
  }
  return isSuccess;
}

// loads a specific image and returns a pointer to its surface
SDL_Surface* loadSurface(std::string&& path) {
  SDL_Surface* loadedSurf = SDL_LoadBMP(path.c_str());

  // check if the surface has been loaded successfully
  if (loadedSurf == NULL) {
    std::cout << SDL_GetError() << '\n';
  }
  return loadedSurf;
}

bool loadMedia() {
  // indicates if the operation is a success
  bool isSuccess{true};

  std::string relativePath{"../img/"};
  // all the file names we need
  std::vector<std::string> fileNames{
      "3default.bmp", "3up.bmp", "3down.bmp", "3right.bmp", "3left.bmp",
  };

  // loads all the images and stores pointers unto the array
  for (size_t i = 0; i < keyPressLink_max; i++) {
    windowsData::keyPressImg[i] = loadSurface(relativePath + fileNames[i]);
    // check if current loading is failure
    if (windowsData::keyPressImg[i] == NULL) {
      isSuccess = false;
    }
  }

  return isSuccess;
}

// update the image surf based on keypress
void updateImageSurf(const SDL_Event& event) {
  using namespace windowsData;
  switch (event.key.keysym.sym) {
    case SDLK_UP:
      imgSurf = keyPressImg[keyPressLink_up];
      break;
    case SDLK_DOWN:
      imgSurf = keyPressImg[keyPressLink_down];
      break;
    case SDLK_LEFT:
      imgSurf = keyPressImg[keyPressLink_left];
      break;
    case SDLK_RIGHT:
      imgSurf = keyPressImg[keyPressLink_right];
      break;
    default:
      imgSurf = keyPressImg[keyPressLink_default];
      break;
  }
}

void close() {
  using namespace windowsData;

  // deallocate surface memory
  imgSurf = nullptr;

  // free all the images
  for (size_t i = 0; i < keyPressLink_max; i++) {
    SDL_FreeSurface(keyPressImg[i]);
    keyPressImg[i] = nullptr;
  }

  // delete the main window
  SDL_DestroyWindow(mainWindow);
  mainWindow = nullptr;

  // close the SDL
  SDL_Quit();
}
