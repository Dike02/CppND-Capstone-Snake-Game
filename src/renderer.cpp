#include "renderer.h"
#include <iostream>
#include <string>
#include "food.h"

Renderer::Renderer(const std::size_t screen_width,
                   const std::size_t screen_height,
                   const std::size_t grid_width, const std::size_t grid_height)
    : screen_width(screen_width),
      screen_height(screen_height),
      grid_width(grid_width),
      grid_height(grid_height) {
  // Initialize SDL
  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    std::cerr << "SDL could not initialize.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create Window
  sdl_window = SDL_CreateWindow("Snake Game", SDL_WINDOWPOS_CENTERED,
                                SDL_WINDOWPOS_CENTERED, screen_width,
                                screen_height, SDL_WINDOW_SHOWN);

  if (nullptr == sdl_window) {
    std::cerr << "Window could not be created.\n";
    std::cerr << " SDL_Error: " << SDL_GetError() << "\n";
  }

  // Create renderer
  //sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED);
  sdl_renderer = SDL_CreateRenderer(sdl_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (nullptr == sdl_renderer) {
    std::cerr << "Renderer could not be created.\n";
    std::cerr << "SDL_Error: " << SDL_GetError() << "\n";
  }
}

Renderer::~Renderer() {
  SDL_DestroyWindow(sdl_window);
  SDL_Quit();
}


void Renderer::Render(Snake const snake, std::unique_ptr<Food> const &food)
{
  SDL_Rect block;
  block.w = screen_width / grid_width;
  block.h = screen_height / grid_height;

  // Clear screen
  SDL_SetRenderDrawColor(sdl_renderer, 0x1E, 0x1E, 0x1E, 0xFF);
  SDL_RenderClear(sdl_renderer);

  // Render food with differentiation based on its type
  if (dynamic_cast<SpeedBoostFood *>(food.get())) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0xFF, 0x00, 0xFF);  // Green for speed boost food
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xCC, 0x00, 0xFF);  // Yellow for normal food
  }
  block.x = food->x * block.w;
  block.y = food->y * block.h;
  SDL_RenderFillRect(sdl_renderer, &block);

  // Render snake's body
  SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0xFF, 0xFF, 0xFF);
  for (SDL_Point const &point : snake.body) {
    block.x = point.x * block.w;
    block.y = point.y * block.h;
    SDL_RenderFillRect(sdl_renderer, &block);
  }

  // Render snake's head
  block.x = static_cast<int>(snake.head_x) * block.w;
  block.y = static_cast<int>(snake.head_y) * block.h;
  if (snake.alive) {
    SDL_SetRenderDrawColor(sdl_renderer, 0x00, 0x7A, 0xCC, 0xFF);
  } else {
    SDL_SetRenderDrawColor(sdl_renderer, 0xFF, 0x00, 0x00, 0xFF);
  }
  SDL_RenderFillRect(sdl_renderer, &block);

  // Update Screen
  SDL_RenderPresent(sdl_renderer);
}


void Renderer::UpdateWindowTitle(int score, int fps) {
  std::string title{"Snake Score: " + std::to_string(score) + " FPS: " + std::to_string(fps)};
  SDL_SetWindowTitle(sdl_window, title.c_str());
}


// Copy Constructor
// Renderer::Renderer(const Renderer &source) {
//     // You might not want to allow copying for this class because of the SDL resources.
//     // If so, you can leave the body of this function empty or throw an exception.
//     throw std::runtime_error("Copy constructor for Renderer class is disabled.");
// }

Renderer::Renderer(const Renderer &source)
    : screen_width(source.screen_width),
      screen_height(source.screen_height),
      grid_width(source.grid_width),
      grid_height(source.grid_height) {

    // You might not want to allow copying for this class because of the SDL resources.
    // If so, you can leave the body of this function empty or throw an exception.
    throw std::runtime_error("Copy constructor for Renderer class is disabled.");
}


// Copy Assignment Operator
Renderer &Renderer::operator=(const Renderer &source) {
    if (this == &source) {
        return *this; // Return *this to deal with self-assignment
    }
    // Like the copy constructor, you can decide to disallow copying.
    throw std::runtime_error("Copy assignment for Renderer class is disabled.");
}

// Move Constructor
Renderer::Renderer(Renderer &&source) noexcept
    : screen_width(source.screen_width),
      screen_height(source.screen_height),
      grid_width(source.grid_width),
      grid_height(source.grid_height) {

    sdl_window = source.sdl_window;
    sdl_renderer = source.sdl_renderer;

    source.sdl_window = nullptr;
    source.sdl_renderer = nullptr;
}

// Move Assignment Operator
Renderer &Renderer::operator=(Renderer &&source) noexcept {
    if (this == &source) {
        return *this;
    }

    SDL_DestroyRenderer(sdl_renderer);
    SDL_DestroyWindow(sdl_window);

    sdl_window = source.sdl_window;
    sdl_renderer = source.sdl_renderer;

    source.sdl_window = nullptr;
    source.sdl_renderer = nullptr;

    // No need to reassign const members in the move assignment.

    return *this;
}
