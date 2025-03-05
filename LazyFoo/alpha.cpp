#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <string>

bool init();
bool loadMedia();
void close();
void setRGB(Uint8& r, Uint8& g, Uint8& b, Uint8& a, SDL_Event& event);

namespace parameters {
int width{800};
int height{600};
}  // namespace parameters

namespace colors {
Uint8 r = 255;
Uint8 g = 255;
Uint8 b = 255;
Uint8 a = 255;
}  // namespace colors

class Texture {
 public:
  Texture();
  ~Texture();
  bool load(std::string);
  void render(int x, int y, SDL_Rect* clip = NULL);
  void deallocate();
  void setColor(Uint8 red, Uint8 green, Uint8 blue);
  void setBlendMode(SDL_BlendMode);
  void setAlpha(Uint8 alpha);

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
Texture man;
Texture bg;
}  // namespace data

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
    using namespace colors;
    using namespace data;

    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }
      // handle keypress rgb change

      else if (event.type == SDL_KEYDOWN) {
        setRGB(r, g, b, a, event);
      }

      // clrscrn
      SDL_SetRenderDrawColor(mainRenderer, 0x00, 0x00, 0xff, 0x00);
      SDL_RenderClear(mainRenderer);

      bg.render(0, 0);
      man.render(40, 390 - man.getHeight());

      bg.setColor(r, g, b);
      man.setColor(r, g, b);
      man.setAlpha(a);

      SDL_RenderPresent(mainRenderer);
    }
  }
  close();
  return 0;
}

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
    renderRect.w = clip->w * 2;
    renderRect.h = clip->h * 2;
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

void Texture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
  SDL_SetTextureColorMod(m_texture, red, green, blue);
}

void Texture::setBlendMode(SDL_BlendMode blendmode) {
  SDL_SetTextureBlendMode(m_texture, blendmode);
}

void Texture::setAlpha(Uint8 alpha) {
  SDL_SetTextureAlphaMod(m_texture, alpha);
}

void setRGB(Uint8& r, Uint8& g, Uint8& b, Uint8& a, SDL_Event& event) {
  switch (event.key.keysym.sym) {
    // Increase red
    case SDLK_q:
      r += 32;
      break;

    // Increase green
    case SDLK_w:
      g += 32;
      break;

    // Increase blue
    case SDLK_e:
      b += 32;
      break;

    // Decrease red
    case SDLK_a:
      r -= 32;
      break;

    // Decrease green
    case SDLK_s:
      g -= 32;
      break;

    // Decrease blue
    case SDLK_d:
      b -= 32;
      break;

    // Increase alpha
    case SDLK_r:
      a += 32;
      break;

    // Increase alpha
    case SDLK_f:
      a -= 32;
      break;
  }
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
  if (!data::man.load("../img/5_man.png")) {
    std::cerr << "IMG LOAD ERROR\n";
    return false;
  }

  data::man.setBlendMode(SDL_BLENDMODE_BLEND);

  if (!data::bg.load("../img/5_bg.png")) {
    std::cerr << "IMG LOAD ERROR\n";
    return false;
  }

  return true;
}

void close() {
  using namespace data;

  // free textures
  man.deallocate();
  bg.deallocate();

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
