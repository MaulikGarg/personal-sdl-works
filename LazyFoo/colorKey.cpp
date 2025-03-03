#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <string>

namespace parameters {
int height{600};
int width{800};
};  // namespace parameters

// wrapper class for a texture
class Texture {
 public:
  Texture();
  ~Texture();

  // load from a file path
  bool load(std::string path);

  // render the texture
  void render(int x, int y);

  // deallocates the currently loaded texture
  void deallocate();

  // getters
  int getHeight() { return m_height; }
  int getWidth() { return m_width; }

 private:
  SDL_Texture* m_texture;
  int m_height{0};
  int m_width{0};
};

Texture::Texture() : m_texture{nullptr}, m_height{0}, m_width{0} {}

// contains textures, window, renderer
namespace data {

// the main window of the program
SDL_Window* mainWindow{nullptr};
// the main renderer
SDL_Renderer* mainRenderer{nullptr};
// the two textures
Texture background;
Texture entity;

};  // namespace data

Texture::~Texture() { deallocate(); }

bool Texture::load(std::string path) {
  // first we de allocate any previously loaded texture
  deallocate();

  SDL_Surface* temp{IMG_Load(path.c_str())};

  if (!temp) {
    std::cerr << "Error opening file: " << IMG_GetError() << '\n';
    return false;
  }

  SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0x00, 0xff, 0xff));

  m_texture = SDL_CreateTextureFromSurface(data::mainRenderer, temp);

  if (!m_texture) {
    std::cerr << "Error creating texture: " << SDL_GetError() << '\n';
    return false;
  }

  // fill in the information
  SDL_QueryTexture(m_texture, NULL, NULL, &m_width, &m_height);

  // free the temporary surface
  SDL_FreeSurface(temp);

  return true;
}

void Texture::deallocate() {
  if (!m_texture) return;
  SDL_DestroyTexture(m_texture);
  m_texture = nullptr;
  m_height = 0;
  m_width = 0;
}

void Texture::render(int x, int y) {  
  SDL_Rect renderRect{x, y, m_width, m_height};
  SDL_RenderCopy(data::mainRenderer, m_texture, NULL, &renderRect);
}

bool loadMedia();
void close();
bool init();

int main(int argc, char* argv[]) {
  if (!init()) {
    std::cerr << "INITIALIZATION ERROR\n";
    return -1;
  }
  if (!loadMedia()) {
    std::cerr << "MEDIA LOAD ERROR\n";
    return -1;
  }

  SDL_Event event;
  bool quit{false};
  while (!quit) {
    while (SDL_PollEvent(&event) != 0)
      if (event.type == SDL_QUIT) quit = true;

    SDL_SetRenderDrawColor(data::mainRenderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(data::mainRenderer);

    // render bg
    data::background.render(0, 0);

    // render entity
    data::entity.render(40, 390 - data::entity.getHeight());

    SDL_RenderPresent(data::mainRenderer);
  }
  close();
  return 0;
}

bool loadMedia() {
  if (!data::background.load("../img/5_bg.png")) {
    std::cerr << "Error loading texture background\n";
    return false;
  }

  if (!data::entity.load("../img/5_man.png")) {
    std::cerr << "Error loading texture man\n";
    return false;
  }

  return true;
}

void close() {
  using namespace data;

  // free textures
  background.deallocate();
  entity.deallocate();

  // destroy renderer
  SDL_DestroyRenderer(mainRenderer);
  mainRenderer = nullptr;

  // destroy window
  SDL_DestroyWindow(mainWindow);
  mainWindow = nullptr;

  // quit sdl
  IMG_Quit();
  SDL_Quit();
}

bool init() {
  using namespace data;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL Init Failure: " << SDL_GetError() << '\n';
    return false;
  }

  mainWindow = SDL_CreateWindow("DOUBLE TEXTURE TEST", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, parameters::width,
                                parameters::height, SDL_WINDOW_SHOWN);

  if (mainWindow == NULL) {
    std::cerr << "SDL Window Creation Failure: " << SDL_GetError() << '\n';
    return false;
  }

  mainRenderer = SDL_CreateRenderer(mainWindow, -1, SDL_RENDERER_ACCELERATED);

  if (mainRenderer == NULL) {
    std::cerr << "SDL Renderer Creation Failure: " << SDL_GetError() << '\n';
    return false;
  }

  if (SDL_SetRenderDrawColor(mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF) != 0) {
    std::cerr << "SDL RENDERER DRAW ERROR: " << SDL_GetError() << '\n';
    return false;
  }

  return true;
}
