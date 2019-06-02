
#pragma once
#include <tuple>

#include <SDL2pp/SDL2pp.hh>
#include <boost/optional/optional.hpp>
#include <functional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/quaternion.hpp>
#include <queue>
#include <random>

#include "geometry.h"
#include "log.h"
#include "shader.h"
#include "trajectory.h"

struct Vertex
{
    glm::vec3 pos_;
    glm::vec2 tex_;
    glm::vec3 norm_;
    glm::vec3 diffuse_;

    Vertex(glm::vec3 pos, glm::vec2 tex, glm::vec3 norm, glm::vec3 diffuse)
        : pos_(pos), tex_(tex), norm_(norm), diffuse_(diffuse){};
};

class Visualisation
{
  public:
    enum Action
    {
        Exit,
        MoveNorth,
        MoveSouth,
        MoveWest,
        MoveEast,
        RotateForward,
        RotateBackward,
        RotatetLeft,
        RotatetRight,
        StartBoost,
        StopBoost
    };

    class Object
    {
      public:
        Object(Visualisation &vis);
        template <int W, int H> void LoadGeometry(Geometry<W, H> &geometry);

        void SetVisibility(bool visible);
        void SetColor(glm::vec3 color);
        void SetPostion(glm::vec3 position);
        void Rotate(float angle, glm::vec3 axis, float running_time);
        void ResetRotation();
        glm::quat GetOrientation(float running_time);
        void Render();

      private:
        ~Object() = default; // FIXME!
        Visualisation &vis_;

        GLuint vertex_buffer_;
        GLuint index_buffer_;
        GLuint indices_count_;

        glm::vec3 color_;
        bool visible_;
        glm::vec3 pos_;

        glm::quat target_rot_;
        glm::quat initial_rot_;
        glm::quat current_rot_;
        Trajectory trajectory_rot_;

        bool inited_;

        friend class Visualisation;
    };

  private:
    SDL2pp::SDL sdl_;
    SDL2pp::Window window_;
    SDL_GLContext main_context_;
    const uint32_t rx_, ry_;
    GLuint mvp_id_;

    glm::vec3 camera_pos_;
    float fov_;
    float camera_dist_, camera_h_, camera_angle_, target_angle_;

    Trajectory camera_trajectory_;

    std::queue<Action> action_queue_;
    std::vector<Object *> objects_;

    void HandleKeyDown(SDL_KeyboardEvent key, float running_time);
    void HandleMouseKeyDown(SDL_MouseButtonEvent btn, float running_time);
    glm::mat4 UpdateCamera(float running_time);

  public:
    Visualisation();
    ~Visualisation() = default; // FIXME!

    Object *CreateObject(); // Visualisation class handles the object desctruction
    bool Render(float running_time);

    Log log_{"Visualisation"};

    boost::optional<Visualisation::Action> DequeueAction();
};