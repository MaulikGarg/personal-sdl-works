#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace audio {

enum effects {
  soundEff_high,
  soundEff_low,
  soundEff_medium,
  soundEff_scratch,
  soundEff_max
};

Mix_Music* mainMusic{nullptr};

std::vector<Mix_Chunk*> soundEffects;
}  // namespace audio

bool init();
bool loadMedia();
void close();

namespace parameters {
constexpr int width{800};
constexpr int height{800};
constexpr int buttonwidth{300};
constexpr int buttonHeight{200};
constexpr int buttonCount{4};
}  // namespace parameters

enum buttonSprite { mouse_out, mouse_over, mouse_down, mouse_up, mouse_max };

class Texture {
 public:
  Texture();
  ~Texture();
  void deallocate();

  void render(int x, int y, SDL_Rect* clip = nullptr, double angle = 0.0,
              SDL_Point* centre = nullptr,
              SDL_RendererFlip flip = SDL_FLIP_NONE);

  bool loadFile(std::string path);

  bool loadText(std::string text, SDL_Color& color);

  // getters
  int getLength() { return m_height; }
  int getBreadth() { return m_width; }

 private:
  bool createTextureFromSurface(SDL_Surface* surf);
  SDL_Texture* m_texture{nullptr};
  int m_height{};
  int m_width{};
};

class Button {
 public:
  Button();
  void render();
  void setPosition(int x, int y);
  void handleEvent(SDL_Event* e);

 private:
  SDL_Point m_position{};
  buttonSprite m_sprite{};
};

namespace data {
SDL_Window* mainWindow{nullptr};
SDL_Renderer* mainRenderer{nullptr};
SDL_Rect sprites[4];
Texture animation;
constexpr int totalFrames{4};

SDL_Color textCol{0, 0, 0};
TTF_Font* mainFont{nullptr};
Texture text;
Texture timeTexture;

SDL_Rect buttonSprites[4];
Texture button;
Button buttons[4];
}  // namespace data

void mouseEventHandler(SDL_Event& event, double& degrees,
                       SDL_RendererFlip& flipType, int& x, int& y) {
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
        x = parameters::width / 2 - 80;
        y = parameters::height / 2 - 80;
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

void music() {
  if (!Mix_PlayingMusic()) {
    Mix_PlayMusic(audio::mainMusic, -1);
    return;
  }

  if (Mix_PausedMusic())
    Mix_ResumeMusic();
  else
    Mix_PauseMusic();
}

void keyEventHandle(SDL_Event& event, int& x, int& y) {
  int dP{10};
  switch (event.key.keysym.sym) {
    using namespace parameters;
    using namespace audio;
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
    case SDLK_1:
      Mix_PlayChannel(-1, soundEffects[soundEff_high], 0);
      return;
    case SDLK_2:
      Mix_PlayChannel(-1, soundEffects[soundEff_medium], 0);
      return;
    case SDLK_3:
      Mix_PlayChannel(-1, soundEffects[soundEff_low], 0);
      return;
    case SDLK_4:
      Mix_PlayChannel(-1, soundEffects[soundEff_scratch], 0);
      return;
    case SDLK_9:
      music();
      return;
    case SDLK_0:
      Mix_HaltMusic();
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
  int x{parameters::width / 2 - 80};
  int y{parameters::height / 2 - 80};

  // timer
  Uint64 startTime{};
  std::stringstream timeText;

  while (!quit) {
    using namespace data;
    SDL_Rect* current{&sprites[frame / 4]};
    while (SDL_PollEvent(&event) != 0) {
      if (event.type == SDL_QUIT) quit = true;

      if (event.type == SDL_MOUSEBUTTONDOWN || event.type == SDL_MOUSEWHEEL) {
        mouseEventHandler(event, degrees, flipType, x, y);
      }

      if (event.type == SDL_KEYDOWN) {
        if (event.key.keysym.sym == SDLK_RETURN) {
          startTime = SDL_GetTicks64();
        } else
          keyEventHandle(event, x, y);
      }

      for (int i{0}; i < parameters::buttonCount; i++) {
        buttons[i].handleEvent(&event);
      }
    }

    timeText.str("");
    timeText << SDL_GetTicks64() - startTime << " ms passed.";
    timeTexture.loadText(timeText.str().c_str(), textCol);

    SDL_SetRenderDrawColor(mainRenderer, 0xff, 0xff, 0xff, 0xff);
    SDL_RenderClear(mainRenderer);

    animation.render(x, y, current, degrees, nullptr, flipType);
    text.render(x + 80, y + 80, nullptr, degrees, nullptr, SDL_FLIP_NONE);
    timeTexture.render(x + 90, y + 10, nullptr, degrees, nullptr,
                       SDL_FLIP_NONE);

    for (int i{0}; i < parameters::buttonCount; i++) {
      buttons[i].render();
    }

    SDL_RenderPresent(mainRenderer);
    frame++;
    if (frame / 4 >= totalFrames) frame = 0;
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

  return createTextureFromSurface(temp);
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

bool Texture::loadText(std::string text, SDL_Color& color) {
  deallocate();
  SDL_Color textCol{0xff, 0xff, 0xff};
  SDL_Surface* temp{
      TTF_RenderUTF8_Shaded(data::mainFont, text.c_str(), color, textCol)};

  if (!temp) {
    std::cerr << "Error creating text: " << TTF_GetError() << '\n';
    return false;
  }

  return {createTextureFromSurface(temp)};
}

// private function
bool Texture::createTextureFromSurface(SDL_Surface* temp) {
  m_texture = SDL_CreateTextureFromSurface(data::mainRenderer, temp);

  SDL_FreeSurface(temp);
  temp = nullptr;

  if (!m_texture) {
    std::cerr << "Error loading texture: " << IMG_GetError() << '\n';
    return false;
  }

  SDL_QueryTexture(m_texture, NULL, NULL, &m_width, &m_height);
  return true;
}

Button::Button() {
  m_position.x = 0;
  m_position.y = 0;
}

void Button::setPosition(int x, int y) {
  m_position.x = x;
  m_position.y = y;
}

void Button::render() {
  data::button.render(m_position.x, m_position.y,
                      &data::buttonSprites[m_sprite]);
}

void Button::handleEvent(SDL_Event* event) {
  int x{}, y{};
  SDL_GetMouseState(&x, &y);

  if (x < m_position.x || x > m_position.x + parameters::buttonwidth ||
      y < m_position.y || y > m_position.y + parameters::buttonHeight) {
    m_sprite = mouse_out;
    return;
  }

  switch (event->type) {
    case SDL_MOUSEBUTTONDOWN:
      m_sprite = mouse_down;
      return;
    case SDL_MOUSEBUTTONUP:
      m_sprite = mouse_up;
      return;
    case SDL_MOUSEMOTION:
      m_sprite = mouse_over;
      return;
  }
}

bool init() {
  using namespace data;

  if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
    std::cerr << "Error initializing sdl: " << SDL_GetError() << '\n';
    return false;
  }

  if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
    std::cerr << "Error initializing img: " << IMG_GetError() << '\n';
    return false;
  }

  if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
    std::cerr << "Error initializing mixer: " << Mix_GetError() << '\n';
    return false;
  }

  if (TTF_Init() == -1) {
    std::cerr << "TTF Init Failure: " << TTF_GetError() << '\n';
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
  using namespace parameters;

  // load animation
  {
    if (!animation.loadFile("../img/6_animation.png")) return false;

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
  }

  // load font
  {
    mainFont = TTF_OpenFont(
        "C:/Users/HP/AppData/Local/Microsoft/Windows/Fonts/"
        "mononoki-Regular.ttf",
        28);
    if (!mainFont) {
      std::cerr << "Error opening font: " << TTF_GetError() << '\n';
      return -1;
    }

    if (!text.loadText("Enter to reset timer", textCol)) {
      return -1;
    };
  }

  // load buttons
  {
    if (!button.loadFile("../img/button.png")) return false;

    for (int i = 0; i < buttonCount; ++i) {
      buttonSprites[i].x = 0;
      buttonSprites[i].y = i * 200;
      buttonSprites[i].w = buttonwidth;
      buttonSprites[i].h = buttonHeight;
    }

    // Set buttons in corners
    buttons[0].setPosition(0, 0);
    buttons[1].setPosition(width - buttonwidth, 0);
    buttons[2].setPosition(0, height - buttonHeight);
    buttons[3].setPosition(width - buttonwidth, height - buttonHeight);
  }

  // load music
  {
    using namespace audio;

    mainMusic = Mix_LoadMUS("../sound/beat.wav");

    std::string prefix{"../sound/"};

    std::vector<std::string> effects{"high.wav", "low.wav", "medium.wav",
                                     "scratch.wav"};

    for (size_t i{0}; i < effects.size(); i++) {
      std::string temp{prefix + effects[i]};
      soundEffects.push_back(Mix_LoadWAV(temp.c_str()));
      if (!soundEffects[i]) {
        std::cerr << "Failed to load " << temp << "\n"
                  << Mix_GetError() << '\n';
        return false;
      }
    }
  }
  return true;
}

void close() {
  using namespace data;
  using namespace audio;

  /* This function deletes all currently loaded
  assets and sets all pointers to null */

  // close music
  Mix_FreeMusic(mainMusic);
  mainMusic = nullptr;

  for (auto i : soundEffects) {
    Mix_FreeChunk(i);
    i = nullptr;
  }

  // delete texture
  animation.deallocate();
  text.deallocate();
  timeTexture.deallocate();

  // close font
  TTF_CloseFont(mainFont);

  // delete renderer
  SDL_DestroyRenderer(mainRenderer);
  mainRenderer = nullptr;

  // delete window
  SDL_DestroyWindow(mainWindow);
  mainWindow = nullptr;

  TTF_Quit();
  IMG_Quit();
  SDL_Quit();
}
