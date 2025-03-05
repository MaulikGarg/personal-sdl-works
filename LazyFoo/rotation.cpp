#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <iostream>
#include <string>

bool init();
bool loadMedia();
void close();

namespace parameters {
constexpr int width{800};
constexpr int height{800};
}  // namespace parameters

class Texture {
 public:
  Texture();
  ~Texture();
  void deallocate();

  void render(int x, int y, SDL_Rect* clip = nullptr, double angle = 0.0,
              SDL_Point* centre = nullptr,
              SDL_RendererFlip flip = SDL_FLIP_NONE);
  bool loadFile(std::string path);

  // getters
  int getLength() { return m_height; }
  int getBreadth() { return m_width; }

 private:
  SDL_Texture* m_texture{nullptr};
  int m_height{};
  int m_width{};
};

namespace data {
SDL_Window* mainWindow{nullptr};
SDL_Renderer* mainRenderer{nullptr};
SDL_Rect sprites[4];
Texture animation;
constexpr int totalFrames{4};
}  // namespace data

void mouseEventHandler(SDL_Event& event, double& degrees,
                       SDL_RendererFlip& flipType) {
  if (event.type == SDL_MOUSEBUTTONDOWN) {
    switch (event.button.button) {
      case SDL_BUTTON_LEFT:
        flipType = SDL_FLIP_HORIZONTAL;
        return;
      case SDL_BUTTON_RIGHT:
        flipType = SDL_FLIP_VERTICAL;
        return;
      case SDL_BUTTON_MIDDLE:
        flipType = SDL_FLIP_NONE;
        degrees = 0;
        return;
    }
  } else if (event.type == SDL_MOUSEWHEEL) {
    if (event.wheel.y > 0) {
      degrees += 20;
      return;
    } else if (event.wheel.y < 0) {
      degrees -= 20;
      return;
    }
  }
}

void keyEventHandle(SDL_Event& event, int& x, int& y) {
  int dP {10};
  switch (event.key.keysym.sym) {

    using namespace parameters;

    case SDLK_w:
      y = (y - dP > 0) ? y - dP : height;
      return;
    case SDLK_s:
      y = (y + dP < height) ? y + dP : 0;
      return;
    case SDLK_a:
      x = (x - dP > 0) ? x - dP : width;
      return;
    case SDLK_d:
      x = (x + dP < width) ? x + dP : 0;
      return;

    default:
      return;
  }
}

int main(int argc, char* argv[]) {
  if (!init()) {
    std::cerr << "INITIALIZATION FAILURE.\n\n";
    return -1;
  }

  if (!loadMedia()) {
    std::cerr << "MEDIA LOAD FAILURE.\n\n";
    return -1;
  }

  SDL_Event event;
  bool quit{false};

  int frame{0};

  // Angle of rotation
  double degrees{};

  // Flip type
  SDL_RendererFlip flipType{SDL_FLIP_NONE};

  // position
  int x{};
  int y{};

  while (!quit) {
    while (SDL_PollEvent(&event) != 0) {
      using namespace data;

      if (event.type == SDL_QUIT) quit = true;
      SDL_SetRenderDrawColor(mainRenderer, 0xff, 0xff, 0xff, 0xff);
      SDL_RenderClear(mainRenderer);

      SDL_Rect* current{&sprites[frame / 4]};
      animation.render(x, y, current, degrees, nullptr, flipType);

      if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEWHEEL) {
        mouseEventHandler(event, degrees, flipType);
      }

      if (event.type == SDL_KEYDOWN) {
        keyEventHandle(event, x, y);
      }

      if (event.type == SDL_MOUSEMOTION) {
        frame++;
        if (frame / 4 >= totalFrames) frame = 0;
      }

      SDL_RenderPresent(mainRenderer);
    }
  }

  close();
  return 0;
}

Texture::Texture() : m_texture{nullptr}, m_height{0}, m_width{0} {}
Texture::~Texture() { deallocate(); }

void Texture::deallocate() {
  if (m_texture) {
    SDL_DestroyTexture(m_texture);
    m_texture = nullptr;
  }
}

bool Texture::loadFile(std::string path) {
  deallocate();

  SDL_Surface* temp{IMG_Load(path.c_str())};

  if (!temp) {
    std::cerr << "Error loading temp surface: " << IMG_GetError() << '\n';
    return false;
  }

  SDL_SetColorKey(temp, SDL_TRUE, SDL_MapRGB(temp->format, 0x00, 0xff, 0xff));

  m_texture = SDL_CreateTextureFromSurface(data::mainRenderer, temp);

  SDL_FreeSurface(temp);

  if (!m_texture) {
    std::cerr << "Error loading texture: " << IMG_GetError() << '\n';
    return false;
  }

  SDL_QueryTexture(m_texture, NULL, NULL, &m_width, &m_height);
  return true;
}

void Texture::render(int x, int y, SDL_Rect* clip, double angle,
                     SDL_Point* centre, SDL_RendererFlip flip) {
  SDL_Rect renderRect{x, y, m_width, m_height};
  if (clip) {
    renderRect.w = clip->w;
    renderRect.h = clip->h;
  }
  SDL_RenderCopyEx(data::mainRenderer, m_texture, clip, &renderRect, angle,
                   centre, flip);
}

bool init() {
  using namespace data;
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL Init Failure: " << SDL_GetError() << '\n';
    return false;
  }

  mainWindow = SDL_CreateWindow("part_anim", SDL_WINDOWPOS_UNDEFINED,
                                SDL_WINDOWPOS_UNDEFINED, parameters::width,
                                parameters::height, SDL_WINDOW_SHOWN);

  if (mainWindow == NULL) {
    std::cerr << "SDL Window Creation Failure: " << SDL_GetError() << '\n';
    return false;
  }

  mainRenderer = SDL_CreateRenderer(
      mainWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

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

bool loadMedia() {
  using namespace data;

  if (!animation.loadFile("../img/6_animation.png")) {
    return false;
  }

  sprites[0].x = 0;
  sprites[0].y = 0;
  sprites[0].w = 64;
  sprites[0].h = 205;

  sprites[1].x = 64;
  sprites[1].y = 0;
  sprites[1].w = 64;
  sprites[1].h = 205;

  sprites[2].x = 128;
  sprites[2].y = 0;
  sprites[2].w = 64;
  sprites[2].h = 205;

  sprites[3].x = 192;
  sprites[3].y = 0;
  sprites[3].w = 64;
  sprites[3].h = 205;

  return true;
}

void close() {
  using namespace data;

  // delete texture
  animation.deallocate();

  // delete renderer
  SDL_DestroyRenderer(mainRenderer);
  mainRenderer = nullptr;

  // delete window
  SDL_DestroyWindow(mainWindow);
  mainWindow = nullptr;

  IMG_Quit();
  SDL_Quit();
}
