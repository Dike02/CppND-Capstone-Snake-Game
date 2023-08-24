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
  //Uint32 frame_start;
 // Uint32 frame_end;
  //Uint32 frame_duration;
  Uint32 frame_count = 0;
  bool running = true;
  PromptGameMode();  // Add this line to prompt the user for game mode
  
  // Create a seperate thread for the game logic
  std::thread gameLogicThread([&](){
    while (running) {
      Uint32 frame_start = SDL_GetTicks();

      // Only Update - the main game logic
      Update();


      // If the time for this frame is too small (i.e. frame_duration is smaller
      // than the target ms_per_frame), delay the loop to achieve the correct frame rate
      if (SDL_GetTicks() - frame_start < target_frame_duration) {
        SDL_Delay(target_frame_duration - (SDL_GetTicks() - frame_start));
      }
    }
  });
  while (running) {
    //Uint32 frame_start = SDL_GetTicks();

    // Input, Update, Render - the main game loop.
    controller.HandleInput(running, snake);
    //Update();
    mtx.lock();
    renderer.Render(snake, food);
    mtx.unlock();

    //frame_end = SDL_GetTicks();

    // Keep track of how long each loop through the input/update/render cycle
    // takes.
    //frame_count++;
    //frame_duration = frame_end - frame_start;

    // After every second, update the window title.
    if (SDL_GetTicks() - title_timestamp >= 1000) {
      renderer.UpdateWindowTitle(score, frame_count);
      frame_count = 0;
      title_timestamp = SDL_GetTicks();
    }

    // If the time for this frame is too small (i.e. frame_duration is
    // smaller than the target ms_per_frame), delay the loop to
    // achieve the correct frame rate.
    /*if (frame_duration < target_frame_duration) {
      SDL_Delay(target_frame_duration - frame_duration);
    }*/
  }

  // Join the thread when finished
  gameLogicThread.join();
}

void Game::PlaceFood() {
  int x, y;
  while (true) {
    x = random_w(engine);
    y = random_h(engine);
    // Check that the location is not occupied by a snake item before placing
    // food.
    /*if (!snake.SnakeCell(x, y)) {
      food.x = x;
      food.y = y;
      return;
    }*/
    if (!snake.SnakeCell(x, y)) {
      if (std::uniform_int_distribution<>(0, 4)(engine) == 0) { // 20% chanc
        food = std::make_unique<SpeedBoostFood>(x, y);
      } else {
        //food = std::make_unique<Food>(x, y);
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
  //if (food.x == new_x && food.y == new_y) 
  if (food->x == new_x && food->y == new_y)
  {
    score++;
    food->ApplyEffect(snake);   // Apply the effect of the food
    //PlaceFood();
    // Grow snake and increase speed.
   snake.GrowBody();
   PlaceFood();
    /*snake.speed += 0.02;*/
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

