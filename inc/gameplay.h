
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

  private:
    Visualisation &vis_;

    std::vector<Geometry<BLOCK_SIZE, BLOCK_SIZE>> blocks_;
    Geometry<BLOCK_SIZE, BLOCK_SIZE> current_block_;
    glm::vec3 current_block_position_;
};