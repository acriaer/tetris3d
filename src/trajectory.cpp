
#include "trajectory.h"
#include "log.h"

static const float tanh_x_low = -2.7f;
static const float tanh_x_high = 2.7f;

Trajectory::Trajectory(float x0, float x1, float y0, float y1)
    : x0_(x0), x1_(x1), y0_(y0), y1_(y1), last_x_(x0)
{
}

float Trajectory::GetPoint(float x)
{
    last_x_ = x;

    if (x > x1_)
        return y1_;
    else if (x < x0_)
        return y0_;

    float mx = (x - x0_) / (x1_ - x0_) * (tanh_x_high - tanh_x_low) + tanh_x_low;
    Log("T").Info() << "mx: " << mx;

    return (glm::tanh(mx) + 1.0f) / 2.0f * (y1_ - y0_) + y0_;
}

void Trajectory::UpdateTrajectory(float x1, float y1)
{
    // FIXME: find a formula to make the first derivative continous
    y0_ = GetPoint(last_x_);
    x0_ = last_x_;
    x1_ = x1;
    y1_ = y1;
}