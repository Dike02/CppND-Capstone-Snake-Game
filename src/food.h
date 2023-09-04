#ifndef FOOD_H
#define FOOD_H

#include "snake.h"

class Food {
public:
    Food(int x, int y) : x(x), y(y) {}
    int x; 
    int y;
    virtual void ApplyEffect(Snake &snake) = 0;  //  Pure virtual function
    // Other common food properties and methods ....
};

class SpeedBoostFood : public Food {
public:
    SpeedBoostFood(int x, int y) : Food(x, y) {}
    
    void ApplyEffect(Snake &snake) override {
        //snake.speed += 0.05;  // Boost the snake's speed by 0.05
        snake.IncreaseSpeed(0.1f);
    }
    // Other properties and methods specific to speed boost food ...
};

#endif  // FOOD_H