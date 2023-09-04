#include "game.h"
#include <iostream>
#include "SDL.h"
#include "SDL_messagebox.h"
#include <thread>

Game::Game(std::size_t grid_width, std::size_t grid_height)
    : snake(grid_width, grid_height),
      engine(dev()),
      random_w(0, static_cast<int>(grid_width - 1)),
      random_h(0, static_cast<int>(grid_height - 1)) {
  PlaceFood();
}

void Game::Run(Controller const &controller, Renderer &renderer,
               std::size_t target_frame_duration) {
  Uint32 title_timestamp = SDL_GetTicks();

  Uint32 frame_count = 0;
  bool running = true;
  PromptGameMode();  // Add this line to prompt the user for game mode
  
  // Create a seperate thread for the game logic "lambda function"
  std::thread gameLogicThread([&](){
    while (running) {
      Uint32 frame_start = SDL_GetTicks();
      float elapsed_time = (frame_start - last_time) / 1000.0f;  // Time in seconds
      last_time = frame_start;

      // Accumulate elapsed time 
      time_accumulator += elapsed_time;

      // Only Update - the main game logic
      // Update the game logic for each time step that fits in the accumulator
      while (time_accumulator >= time_step) {
        Update();
        time_accumulator -= time_step;
      }


      // If the time for this frame is too small (i.e. frame_duration is smaller
      // than the target ms_per_frame), delay the loop to achieve the correct frame rate
      if (SDL_GetTicks() - frame_start < target_frame_duration) {
        SDL_Delay(target_frame_duration - (SDL_GetTicks() - frame_start));
      }
    }
  });
  while (running) {

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    //Update();
    mtx.lock();
    renderer.Render(snake, food);
    mtx.unlock();

    // After every second, update the window title.
    if (SDL_GetTicks() - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = SDL_GetTicks();
    }

  }

  // Join the thread when finished
  gameLogicThread.join();
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);

    if (!snake.SnakeCell(x, y)) {
      if (std::uniform_int_distribution<>(0, 4)(engine) == 0) { // 20% chanc
        food = std::make_unique<SpeedBoostFood>(x, y);
      } else {
        food = std::make_unique<SpeedBoostFood>(x, y);
      }
      return;
    }
  }
}

void Game::Update() {
  if (!snake.alive) return;

  snake.Update();

  int new_x = static_cast<int>(snake.head_x);
  int new_y = static_cast<int>(snake.head_y);

  // Check if there's food over here
  if (food->x == new_x && food->y == new_y)
  {
    score++;
    food->ApplyEffect(snake);   // Apply the effect of the food
    // Grow snake and increase speed.
   snake.GrowBody();
   PlaceFood();
  }
}

int Game::GetScore() const { return score; }
int Game::GetSize() const { return snake.size; }

void Game::PromptGameMode() {
  const SDL_MessageBoxButtonData buttons[] = {
      { /* .flags, .buttonid, .text */        0, 0, "Wall Mode" },
      { SDL_MESSAGEBOX_BUTTON_RETURNKEY_DEFAULT, 1, "Teleportation Mode" },
  };

  const SDL_MessageBoxData messageboxdata = {
      SDL_MESSAGEBOX_INFORMATION, /* .flags */
      NULL, /* .window */
      "Game Mode", /* .title */
      "Choose a game mode:", /* .message */
      SDL_arraysize(buttons), /* .numbuttons */
      buttons, /* .buttons */
      NULL /* .colorScheme */
  };

  int buttonid;
  if (SDL_ShowMessageBox(&messageboxdata, &buttonid) < 0) {
      SDL_Log("error displaying message box");
      return;
  }
  if (buttonid == -1) {
      SDL_Log("no selection");
  } else {
      if(buttonid == 0) {
          snake.gameMode = Snake::GameMode::kWall;
      } else if(buttonid == 1) {
          snake.gameMode = Snake::GameMode::kTeleportation;
      }
  }
}

