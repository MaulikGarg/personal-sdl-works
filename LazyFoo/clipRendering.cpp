#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <string>

bool init();
bool loadMedia();
void close();

namespace parameters {
int width{800};
int height{600};
}  // namespace parameters

class Texture {
 public:
  Texture();
  ~Texture();
  bool load(std::string);
  void render(int x, int y, SDL_Rect* clip = NULL);
  void deallocate();

  // getters
  int getHeight() { return m_height; }
  int getWidth() { return m_width; }

 private:
  SDL_Texture* m_texture{nullptr};
  int m_height{};
  int m_width{};
};

namespace data {
SDL_Window* mainWindow{nullptr};
SDL_Renderer* mainRenderer{nullptr};
SDL_Rect spriteClips[4];
Texture spriteTexture;
}  // namespace data

Texture::Texture() : m_texture{}, m_height{}, m_width{} {}

Texture::~Texture() { deallocate(); }

bool Texture::load(std::string path) {
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

void Texture::render(int x, int y, SDL_Rect* clip) {
  SDL_Rect renderRect{x, y, m_width, m_height};
  if (clip) {
    renderRect.w = clip->w;
    renderRect.h = clip->h;
  }
  SDL_RenderCopy(data::mainRenderer, m_texture, clip, &renderRect);
}

void Texture::deallocate() {
  if (!m_texture) return;
  SDL_DestroyTexture(m_texture);
  m_texture = nullptr;
  m_height = 0;
  m_width = 0;
}

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
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }
      using namespace data;

      //clrscrn
      SDL_SetRenderDrawColor(mainRenderer, 0xff, 0x45, 0xff, 0xff);
      SDL_RenderClear(mainRenderer);

      {
        using namespace parameters;
        // top left
      spriteTexture.render(0, 0, &spriteClips[0]);
      // top right
      spriteTexture.render(width-spriteClips[1].w, 0, &spriteClips[1]);
      // bottom left
      spriteTexture.render(0, height - spriteClips[2].h, &spriteClips[2]);
      // bottom right
      spriteTexture.render(width - spriteClips[3].w, height - spriteClips[3].h, &spriteClips[3]);
      }
      

      SDL_RenderPresent(mainRenderer);
    }
  }
  close();
  return 0;
}

bool init() {
  data::mainWindow = SDL_CreateWindow(
      "Clip Rendering", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      parameters::width, parameters::height, SDL_WINDOW_SHOWN);

  if (!data::mainWindow) {
    std::cerr << "Error creating window: " << SDL_GetError() << '\n';
    return false;
  }

  data::mainRenderer =
      SDL_CreateRenderer(data::mainWindow, -1, SDL_RENDERER_ACCELERATED);

  if (!data::mainRenderer) {
    std::cerr << "Renderer creation error: " << SDL_GetError();
    return false;
  }

  if (SDL_SetRenderDrawColor(data::mainRenderer, 0xFF, 0xFF, 0xFF, 0xFF) != 0) {
    std::cerr << "SDL RENDERER DRAW ERROR: " << SDL_GetError() << '\n';
    return false;
  }

  return true;
}

bool loadMedia() {
  if (!data::spriteTexture.load("../img/sprites.png")) {
    std::cerr << "IMG LOAD ERROR\n";
    return false;
  }

  data::spriteClips[0] = {0, 0, 100, 100};
  data::spriteClips[1] = {100, 0, 100, 100};
  data::spriteClips[2] = {0, 100, 100, 100};
  data::spriteClips[3] = {100, 100, 100, 100};

  return true;
}

void close() {
  using namespace data;

  // free textures
  spriteTexture.deallocate();

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
