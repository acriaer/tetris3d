#pragma once
#include <glm/glm.hpp>

class Trajectory
{
  public:
    // This class generates smooth transition between y0 and y1 over "time" x1 - x0

    Trajectory(float x0 = 0.0f, float x1 = 1.0f, float y0 = 0.0f, float y1 = 1.0f);
    float GetPoint(float x);
    void UpdateTrajectory(float x1, float y1); // x0 and y0 come from the last GetPoint()

  private:
    float x0_; // fixme
    float x1_;
    float y0_;
    float y1_;

    float last_x_;
};