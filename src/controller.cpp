#include "controller.h"
#include <iostream>
#include "SDL.h"
#include "snake.h"

// Rule of Three
Controller::Controller(const Controller& source) {
    
}

Controller& Controller::operator=(const Controller& source) {
    if (this == &source) return *this; 
    return *this;
}

Controller::~Controller() {
   
}

// Rule of Five
Controller::Controller(Controller&& source) {
    
}

Controller& Controller::operator=(Controller&& source) {
    if (this == &source) return *this; 
    return *this;
}

void Controller::ChangeDirection(Snake &snake, Snake::Direction input,
                                 Snake::Direction opposite) const {
  if (snake.direction != opposite || snake.size == 1) snake.direction = input;
  return;
}

void Controller::HandleInput(bool &running, Snake &snake) const {
  SDL_Event e;
  while (SDL_PollEvent(&e)) {
    if (e.type == SDL_QUIT) {
      running = false;
    } else if (e.type == SDL_KEYDOWN) {
      switch (e.key.keysym.sym) {
        case SDLK_UP:
          ChangeDirection(snake, Snake::Direction::kUp,
                          Snake::Direction::kDown);
          break;

        case SDLK_DOWN:
          ChangeDirection(snake, Snake::Direction::kDown,
                          Snake::Direction::kUp);
          break;

        case SDLK_LEFT:
          ChangeDirection(snake, Snake::Direction::kLeft,
                          Snake::Direction::kRight);
          break;

        case SDLK_RIGHT:
          ChangeDirection(snake, Snake::Direction::kRight,
                          Snake::Direction::kLeft);
          break;
      }
    }
  }
}