
#include <SDL2/SDL.h>

#include "config.h"
#include "visualisation.h"

using namespace SDL2pp;
using std::get;

const uint32_t points = 4;
const uint32_t floatsPerPoint = 3;
const uint32_t floatsPerColor = 4;

const GLfloat diamond[points][floatsPerPoint] = {
    {0, 1.0f, 1.0},
    {1.0f, 1.0f, 1.0},
    {1.0f, 0, 1.0},
    {0, 0, 1.0},
};

const GLfloat colors[points][floatsPerColor] = {
    {0.0, 1.0, 0.0, 1.0},
    {1.0, 1.0, 0.0, 1.0},
    {1.0, 0.0, 0.0, 1.0},
    {0.0, 0.0, 1.0, 1.0},
};

glm::vec3 HTN(glm::vec4 homo_vector)
{
    return glm::vec3(homo_vector.x / homo_vector.w, homo_vector.y / homo_vector.w,
                     homo_vector.z / homo_vector.w);
}

glm::vec4 NTH(glm::vec3 nonhomo_vector)
{
    return glm::vec4(nonhomo_vector.x, nonhomo_vector.y, nonhomo_vector.z, 1.0);
}

Visualisation::Visualisation()
    : sdl_(SDL_INIT_VIDEO),
      window_("Tetris3D", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
              Config::inst().GetOption<int>("resx"),
              Config::inst().GetOption<int>("resy"), SDL_WINDOW_OPENGL),
      main_context_(SDL_GL_CreateContext(window_.Get())),
      rx_(Config::inst().GetOption<int>("resx")),
      ry_(Config::inst().GetOption<int>("resy")), camera_pos_(0, 0, 0), fov_(65.0f),
      camera_dist_(200.0f), camera_h_(200.0f), camera_angle_(0.0f),
      camera_trajectory_(0.0f, 1.0f, 9, glm::half_pi<float>() / 2.0f)
{
    SDL_GL_SetSwapInterval(1);
    SDL_GL_ResetAttributes();

    glewExperimental = GL_TRUE;
    ASSERT(glewInit() == GLEW_OK);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    GLuint programID = LoadShaders("shaders/SimpleTransform.vertexshader",
                                   "shaders/SingleColor.fragmentshader");

    mvp_id_ = glGetUniformLocation(programID, "MVP");
    mesh_ = Mesh("res/phoenix/phoenix_ugv.md2");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glUseProgram(programID);
}

glm::mat4 Visualisation::UpdateCamera(float running_time)
{
    glm::vec3 lookat_h = glm::vec3(0.0f, 0.0f, 0.0f);
    camera_angle_ = camera_trajectory_.GetPoint(running_time);

    camera_pos_.x = glm::cos(camera_angle_) * camera_dist_;
    camera_pos_.y = camera_h_;
    camera_pos_.z = glm::sin(camera_angle_) * camera_dist_;

    return glm::lookAt(camera_pos_, lookat_h, glm::vec3(0, 1, 0));
}

bool Visualisation::Render(float running_time)
{
    glm::mat4 projection =
        glm::perspective(glm::radians(fov_), float(rx_) / float(ry_), 0.1f, 10000.0f);

    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 view = UpdateCamera(running_time);

    glm::mat4 mvp = projection * view * model;
    // Clear the screen
    glUniformMatrix4fv(mvp_id_, 1, GL_FALSE, &mvp[0][0]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mesh_->Render();

    // Swap buffers
    SDL_GL_SwapWindow(window_.Get());

    SDL_Event event;
    while (SDL_PollEvent(&event))
    {
        switch (event.type)
        {
        case SDL_KEYDOWN:
            HandleKeyDown(event.key, running_time);
            break;
        case SDL_MOUSEBUTTONDOWN:
            HandleMouseKeyDown(event.button, running_time);
            break;
        }
    }

    return 0;
}

void Visualisation::HandleKeyDown(SDL_KeyboardEvent key, float running_time)
{
    switch (key.keysym.sym)
    {
    case SDLK_UP:
        break;
    case SDLK_DOWN:
        break;
    case SDLK_LEFT:
        camera_trajectory_.UpdateTrajectory(running_time + 500.0f,
                                            camera_angle_ + glm::half_pi<float>());
        break;
    case SDLK_RIGHT:
        camera_trajectory_.UpdateTrajectory(running_time + 500.0f,
                                            camera_angle_ - glm::half_pi<float>());

        break;
    case SDLK_w:
        break;
    case SDLK_s:
        break;
    case SDLK_a:
        break;
    case SDLK_d:
        break;
    case SDLK_q:
        break;
    case SDLK_e:
        break;
    case SDLK_KP_PLUS:
        fov_ *= 1.1f;
        break;
    case SDLK_KP_MINUS:
        fov_ *= 0.9f;
        break;
    case SDLK_ESCAPE:
        action_queue_.push(Action::Exit);
        break;
    case SDLK_PAUSE:
        break;
    }
}

void Visualisation::HandleMouseKeyDown(SDL_MouseButtonEvent key, float running_time)
{
    // if (key.button != SDL_BUTTON_LEFT)
    //    return;
}

boost::optional<Visualisation::Action> Visualisation::DequeueAction()
{
    if (action_queue_.empty())
        return boost::none;

    auto ret = action_queue_.front();
    action_queue_.pop();
    return ret;
}