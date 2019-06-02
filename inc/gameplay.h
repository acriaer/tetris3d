
#pragma once

#include "consts.h"
#include "geometry.h"
#include "visualisation.h"

#include <glm/glm.hpp>
#include <random>

class Gameplay
{
  public:
    Gameplay(Visualisation &vis);
    void Update(float running_time);
    void HandleAction(Visualisation::Action action, float running_time);

  private:
    std::vector<std::pair<Geometry<BLOCK_SIZE, BLOCK_SIZE>, Visualisation::Object *>>
        blocks_;

    Geometry<BOARD_SIZE, BOARD_SIZE> heap_;
    Visualisation::Object *heap_object_;

    struct
    {
        int target_position_x_ = 0;
        int target_position_z_ = 0;

        float height_ = 0;
        int type = 0;
        Geometry<BLOCK_SIZE, BLOCK_SIZE> geometry_;
    } falling_block_;

    float last_time_;
    bool boost_on_;

    std::random_device random_device_;
    std::mt19937 random_generator_;
    std::uniform_int_distribution<> color_distribution_;
    std::uniform_int_distribution<> block_distribution_;

    Trajectory trajectory_movement_x_;
    Trajectory trajectory_movement_z_;

    float accumulated_speed_;
    const float max_speed_;
    const float boost_speed_;
    const float speed_increment_;
    const float speed_increment_peroid_;
    const int height_;

    Log log_{"Gameplay"};

    void InitNewFallingBlock();
};