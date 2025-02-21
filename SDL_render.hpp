#include <SDL3/SDL.h>

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static SDL_Texture* earth_tx = nullptr;
static SDL_Texture* sat_tx = nullptr;
static SDL_Texture* moon_tx = nullptr;

static unsigned int win_width = 0;
static unsigned int win_height = 0;

constexpr unsigned int tex_margin = 15;
constexpr unsigned int earth_r = 30;
constexpr unsigned int moon_r = 10;
constexpr unsigned int sat_r = 5;

constexpr double scale = 0.0000022;
constexpr double xOffset = -400;

bool initializeSDL(unsigned int window_width, unsigned int window_height);

void drawObjects(double earthX, double moonX, double satX, double satY);

bool processEventsAndQuit();



