
#pragma once
#include <tuple>

#include <SDL2pp/SDL2pp.hh>
#include <boost/optional/optional.hpp>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <queue>

#include "log.h"
#include "mesh.h"
#include "shader.h"
#include "trajectory.h"

class Visualisation
{
  public:
    enum Action
    {
        Exit
    };

  private:
    SDL2pp::SDL sdl_;
    SDL2pp::Window window_;
    SDL_GLContext main_context_;
    const uint32_t rx_, ry_;
    GLuint mvp_id_;

    glm::vec3 camera_pos_;
    float fov_;
    float camera_dist_, camera_h_, camera_angle_;

    Trajectory camera_trajectory_;

    std::queue<Action> action_queue_;
    boost::optional<Mesh> mesh_;

    void HandleKeyDown(SDL_KeyboardEvent key, float running_time);
    void HandleMouseKeyDown(SDL_MouseButtonEvent btn, float running_time);
    glm::mat4 UpdateCamera(float running_time);

  public:
    Visualisation();

    bool Render(float running_time);
    Log log_{"Visualisation"};

    boost::optional<Visualisation::Action> DequeueAction();
};