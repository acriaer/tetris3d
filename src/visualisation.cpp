
#include <SDL2/SDL.h>

#include "config.h"
#include "consts.h"
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
      camera_dist_(20.0f), camera_h_(20.0f), camera_angle_(0.0f),
      target_angle_(glm::half_pi<float>() / 2.0f),
      camera_trajectory_(0.0f, 1.0f, 0.0, target_angle_)
{
    SDL_GL_SetSwapInterval(1);
    SDL_GL_ResetAttributes();

    glewExperimental = GL_TRUE;
    ASSERT(glewInit() == GLEW_OK);

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    GLuint programID = LoadShaders("shaders/SimpleTransform.vertexshader",
                                   "shaders/SingleColor.fragmentshader");

    mvp_id_ = glGetUniformLocation(programID, "MVP");

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    glUseProgram(programID);
}

glm::mat4 Visualisation::UpdateCamera(float running_time)
{
    glm::vec3 lookat_h = glm::vec3(0.0f, 0.0f, 0.0f);
    camera_angle_ = camera_trajectory_.GetPoint(running_time);

    camera_pos_.x = glm::cos(camera_angle_) * camera_dist_;
    camera_pos_.z = glm::sin(camera_angle_) * camera_dist_;
    camera_pos_.y = camera_h_;

    return glm::lookAt(camera_pos_, lookat_h, glm::vec3(0, 1, 0));
}

bool Visualisation::Render(float running_time)
{
    glm::mat4 projection =
        glm::perspective(glm::radians(fov_), float(rx_) / float(ry_), 0.1f, 10000.0f);

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0, -2.0f));

    glm::mat4 view = UpdateCamera(running_time);

    glm::mat4 mvp = projection * view * model;
    // Clear the screen
    glUniformMatrix4fv(mvp_id_, 1, GL_FALSE, &mvp[0][0]);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto &obj : objects_)
    {
        obj->Render();
    }

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
        target_angle_ = target_angle_ + glm::half_pi<float>();
        camera_trajectory_.UpdateTrajectory(running_time + 0.5f, target_angle_);
        break;
    case SDLK_RIGHT:
        target_angle_ = target_angle_ - glm::half_pi<float>();
        camera_trajectory_.UpdateTrajectory(running_time + 0.5f, target_angle_);

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

Visualisation::Object *Visualisation::CreateObject()
{
    auto ret = new Object(*this);
    objects_.push_back(ret);
    return ret;
}

template <int W, int H> void Visualisation::Object::LoadGeometry(Geometry<W, H> &geometry)
{

    std::vector<Vertex> vertices;
    std::vector<glm::u32> indices;
    int vertices_counter = 0;

    // clang-format off
    auto place_wall =
        [&](float x, float y, float z, float xt, float yt, float zt, float xo1, float yo1, float zo1, float xo2, float yo2, float zo2) mutable {
            vertices.emplace_back(glm::vec3(x + xt + xo1 - xo2, y + yt + yo1 - yo2, z + zt + zo1 - zo2), glm::vec2(1, 0), glm::vec3(xt, yt, zt), glm::vec3(1, 1, 1));
            vertices.emplace_back(glm::vec3(x + xt - xo1 + xo2, y + yt - yo1 + yo2, z + zt - zo1 + zo2), glm::vec2(0, 1), glm::vec3(xt, yt, zt), glm::vec3(1, 1, 1));
            vertices.emplace_back(glm::vec3(x + xt - xo1 - xo2, y + yt - yo1 - yo2, z + zt - zo1 - zo2), glm::vec2(0, 0), glm::vec3(xt, yt, zt), glm::vec3(1, 1, 1));
            vertices.emplace_back(glm::vec3(x + xt + xo1 + xo2, y + yt + yo1 + yo2, z + zt + zo1 + zo2), glm::vec2(1, 1), glm::vec3(xt, yt, zt), glm::vec3(1, 1, 1));

            indices.push_back(vertices_counter);
            indices.push_back(vertices_counter + 1);
            indices.push_back(vertices_counter + 2);
            indices.push_back(vertices_counter);
            indices.push_back(vertices_counter + 1);
            indices.push_back(vertices_counter + 3);
            vertices_counter += 4;
        };
    // clang-format on

    const float U = 0.5;
    for (int x = 0; x < W; x++)
    {
        for (int z = 0; z < H; z++)
        {
            for (int h = 0; h < geometry.heap_.size(); h++)
            {
                if (geometry.Element(x, z, h))
                {
                    if (x - 1 < 0 || !geometry.Element(x - 1, z, h))
                        place_wall(x, h, z, -U, 0, 0, 0, U, 0, 0, 0, U);
                    if (x + 1 == W || !geometry.Element(x + 1, z, h))
                        place_wall(x, h, z, U, 0, 0, 0, U, 0, 0, 0, U);
                    if (h - 1 < 0 || !geometry.Element(x, z, h - 1))
                        place_wall(x, h, z, 0, -U, 0, U, 0, 0, 0, 0, U);
                    if (h + 1 == geometry.heap_.size() || !geometry.Element(x, z, h + 1))
                        place_wall(x, h, z, 0, U, 0, U, 0, 0, 0, 0, U);
                    if (z - 1 < 0 || !geometry.Element(x, z - 1, h))
                        place_wall(x, h, z, 0, 0, -U, U, 0, 0, 0, U, 0);
                    if (z + 1 == H || !geometry.Element(x, z + 1, h))
                        place_wall(x, h, z, 0, 0, U, U, 0, 0, 0, U, 0);
                }
            }
        }
    }

    glGenBuffers(1, &vertex_buffer_);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), &vertices[0],
                 GL_STATIC_DRAW);

    glGenBuffers(1, &index_buffer_);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::u32) * indices.size(), &indices[0],
                 GL_STATIC_DRAW);

    indices_count_ = indices.size();
}

Visualisation::Object::Object(Visualisation &vis) : vis_(vis), visible_(false), pos_() {}

void Visualisation::Object::SetVisibility(bool v) { visible_ = v; }

void Visualisation::Object::Render()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer_);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid *)offsetof(Vertex, pos_));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid *)offsetof(Vertex, tex_));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid *)offsetof(Vertex, diffuse_));
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex),
                          (const GLvoid *)offsetof(Vertex, norm_));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer_);
    glDrawElements(GL_TRIANGLES, indices_count_, GL_UNSIGNED_INT, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(3);
}

// Explicitly instantiate Object ctor to avoid stuffing this logic into header
// file.
template void
Visualisation::Object::LoadGeometry(Geometry<BOARD_SIZE, BOARD_SIZE> &geometry);
template void
Visualisation::Object::LoadGeometry(Geometry<BLOCK_SIZE, BLOCK_SIZE> &geometry);