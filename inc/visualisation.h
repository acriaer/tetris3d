
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
        // fixme: North, West, etc. names are stupid in this case
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
        template <int W, int H>
        void LoadGeometry(Geometry<W, H> &geometry, bool create_markers = false);

        void SetVisibility(bool visible);
        void SetColor(glm::vec3 color);
        void SetPostion(glm::vec3 position);
        void Rotate(float angle, glm::vec3 axis, float running_time);
        void ResetRotation();
        glm::quat GetOrientation(float running_time);
        void Render(GLuint mode_id);

      private:
        Object(Visualisation &vis);
        ~Object();
        Visualisation &vis_;

        GLuint vertex_buffer_;
        GLuint index_buffer_;
        GLuint indices_count_;
        GLuint markers_buffer_;
        GLuint markers_count_;

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

    // gl uniforms ids
    GLuint vp_id_, m_id_, mode_id_;

    glm::vec3 camera_pos_;
    float fov_;
    float camera_dist_, camera_h_, camera_angle_, target_angle_;

    // rotation trajectory
    Trajectory camera_trajectory_;
    Trajectory fov_trajectory_;

    // we need to keep track of
    int camera_action_shift_;

    std::queue<Action> action_queue_;
    std::vector<Object *> objects_;

    void HandleKeyDown(SDL_KeyboardEvent key, float running_time);
    void HandleKeyUp(SDL_KeyboardEvent key, float running_time);
    void HandleMouseKeyDown(SDL_MouseButtonEvent btn, float running_time);
    glm::mat4 UpdateCamera(float running_time);

  public:
    Visualisation();
    ~Visualisation();

    // Request new object handle.
    // Visualisation class handles the object desctruction
    // it only lives as long as the Visualisation.
    Object *CreateObject();

    bool Render(float running_time);

    Log log_{"Visualisation"};

    boost::optional<Visualisation::Action> DequeueAction();
};