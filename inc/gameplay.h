
#pragma once

#include "consts.h"
#include "geometry.h"
#include "visualisation.h"

#include <glm/glm.hpp>

class Gameplay
{
  public:
    Gameplay(Visualisation &vis);
    void Update(float running_time);
    void HandleAction(Visualisation::Action action);

  private:
    Visualisation &vis_;

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

    void InitNewFallingBlock();
};