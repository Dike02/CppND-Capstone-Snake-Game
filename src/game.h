#ifndef GAME_H
#define GAME_H

#include <random>
#include "SDL.h"
#include "controller.h"
#include "renderer.h"
#include "snake.h"
#include <mutex>
#include "food.h"
#include <memory>

class Game {
 public:
  Game(std::size_t grid_width, std::size_t grid_height);
  void Run(Controller const &controller, Renderer &renderer,
           std::size_t target_frame_duration);
  int GetScore() const;
  int GetSize() const;
  ~Game();

 private:
  Snake snake;
  Controller *controller;
  //SDL_Point food;
  void PromptGameMode();
  std::mutex mtx;
  std::unique_ptr<Food> food;

  std::random_device dev;
  std::mt19937 engine;
  std::uniform_int_distribution<int> random_w;
  std::uniform_int_distribution<int> random_h;

  int score{0};

  void PlaceFood();
  void Update();

  
  Uint32 last_time{0}; // Keeps track of the last frame's time
  float time_accumulator{0.0f};  // Accumulate time between frames
  const float time_step = 0.02f;  // Fixed time step for game logic updates
  
};

#endif