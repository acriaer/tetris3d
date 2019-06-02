
#include "gameplay.h"
#include "config.h"

// clang-format off
static const std::vector<std::array<uint32_t, BLOCK_SIZE*BLOCK_SIZE>> tetris_shapes = {
    {
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 1, 1, 1, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0
    },
    {
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0
    },
    {
        0, 0, 0, 0, 0,
        0, 1, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 1, 0,
        0, 0, 0, 0, 0
    },
    {
        0, 0, 0, 0, 0,
        0, 1, 1, 0, 0,
        0, 1, 1, 0, 0,
        0, 0, 0, 0, 0,
        0, 0, 0, 0, 0
    },
    {
        0, 0, 0, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 0, 0,
        0, 0, 1, 1, 0,
        0, 0, 0, 0, 0
    },
};

// clang-format on

static Geometry<BLOCK_SIZE, BLOCK_SIZE>
ShapeToGeometry(const std::array<uint32_t, BLOCK_SIZE * BLOCK_SIZE> &shape)
{
    Geometry<BLOCK_SIZE, BLOCK_SIZE> ret;

    for (int i = 0; i < BLOCK_SIZE / 2; i++)
        ret.AddEmptyLayer();
    ret.AddLayer(shape);
    for (int i = 0; i < BLOCK_SIZE / 2; i++)
        ret.AddEmptyLayer();

    return ret;
}

Gameplay::Gameplay(Visualisation &vis)
    : last_time_(0.0f), random_device_(), random_generator_(random_device_()),
      // fixme: hardcoded stuff
      color_distribution_(0x60, 0xA0), block_distribution_(0, tetris_shapes.size() - 1),
      trajectory_movement_x_(), trajectory_movement_z_(),
      accumulated_speed_(Config::inst().GetOption<float>("initial_speed")),
      max_speed_(Config::inst().GetOption<float>("max_speed")),
      boost_speed_(Config::inst().GetOption<float>("boost_speed")),
      speed_increment_(Config::inst().GetOption<float>("speed_increment")),
      speed_increment_peroid_(Config::inst().GetOption<float>("speed_increment_peroid")),
      height_(Config::inst().GetOption<int>("height"))
{
    for (auto &shape : tetris_shapes)
    {
        auto geometry = ShapeToGeometry(shape);
        auto object = vis.CreateObject();
        object->LoadGeometry(geometry, true);
        blocks_.emplace_back(std::make_pair(geometry, object));
    }

    heap_object_ = vis.CreateObject();

    // Don't bother with collisions with virtual floot, lets use normal blocks for this.
    heap_.AddFullLayer();
    heap_.AddFullLayer();
    heap_.AddFullLayer();

    heap_.Repaint(0x40, 0x40, 0x40); // fixme: hardcoded stuff
    heap_object_->LoadGeometry(heap_);
    heap_object_->SetVisibility(true);

    InitNewFallingBlock();
}

void Gameplay::InitNewFallingBlock()
{
    blocks_[falling_block_.type].second->SetVisibility(false);
    blocks_[falling_block_.type].second->ResetRotation();

    falling_block_.type = block_distribution_(random_generator_);
    log_.Info() << "Spawning new block of shape: " << falling_block_.type;

    falling_block_.geometry_ = blocks_[falling_block_.type].first;

    falling_block_.geometry_.Repaint(color_distribution_(random_generator_),
                                     color_distribution_(random_generator_),
                                     color_distribution_(random_generator_));

    blocks_[falling_block_.type].second->LoadGeometry(falling_block_.geometry_, true);
    blocks_[falling_block_.type].second->SetVisibility(true);
    falling_block_.target_position_x_ = BOARD_SIZE / 2 - BLOCK_SIZE / 2; // fixme
    falling_block_.target_position_z_ = BOARD_SIZE / 2 - BLOCK_SIZE / 2; // fixme

    trajectory_movement_x_ =
        Trajectory(0.0f, 1.0f, 0.0f, falling_block_.target_position_x_);
    trajectory_movement_z_ =
        Trajectory(0.0f, 1.0f, 0.0f, falling_block_.target_position_z_);

    falling_block_.height_ = height_; // fixme: rename hight_
}

void Gameplay::Update(float running_time)
{
    float delta_time = running_time - last_time_;

    // For debugging
    delta_time = std::min(delta_time, 0.0166f);

    if (boost_on_)
        falling_block_.height_ -= boost_speed_ * delta_time;
    else
        falling_block_.height_ -= accumulated_speed_ * delta_time;

    if (int(last_time_ / speed_increment_peroid_) !=
            int(running_time / speed_increment_peroid_) &&
        accumulated_speed_ <= max_speed_)
    {
        accumulated_speed_ *= speed_increment_;
        log_.Info() << "Increasing speed!";
    }

    if (heap_.CheckCollision(falling_block_.geometry_, falling_block_.target_position_x_,
                             falling_block_.target_position_z_, falling_block_.height_))
    {
        heap_.Merge(falling_block_.geometry_, falling_block_.target_position_x_,
                    falling_block_.target_position_z_, falling_block_.height_ + 1.0f);

        for (int i = std::max(3, int(falling_block_.height_ - (BLOCK_SIZE / 2 + 1)));
             i < int(falling_block_.height_ + (BLOCK_SIZE / 2 + 1)); i++)
        {
            while (heap_.CheckFullLayer(i))
            {
                heap_.RemoveLayer(i);
                log_.Info() << "Layer full.";
            }
        }

        heap_object_->LoadGeometry(heap_);
        InitNewFallingBlock();
    }

    blocks_[falling_block_.type].second->SetPostion(
        glm::vec3(trajectory_movement_x_.GetPoint(running_time), falling_block_.height_,
                  trajectory_movement_z_.GetPoint(running_time)));

    last_time_ = running_time;
}

// fixme: too big, move this logic somewhere lese
void Gameplay::HandleAction(Visualisation::Action action, float running_time)
{
    bool target_changed = false;
    static Geometry<BLOCK_SIZE, BLOCK_SIZE> new_geometry;

    switch (action)
    {
    case Visualisation::Action::MoveNorth:
        if (!heap_.CheckCollision(
                falling_block_.geometry_, falling_block_.target_position_x_,
                falling_block_.target_position_z_ + 1, falling_block_.height_))
        {
            falling_block_.target_position_z_ += 1;
            target_changed = true;
        }
        break;
    case Visualisation::Action::MoveSouth:
        if (!heap_.CheckCollision(
                falling_block_.geometry_, falling_block_.target_position_x_,
                falling_block_.target_position_z_ - 1, falling_block_.height_))
        {
            falling_block_.target_position_z_ -= 1;
            target_changed = true;
        }
        break;
    case Visualisation::Action::MoveEast:
        if (!heap_.CheckCollision(
                falling_block_.geometry_, falling_block_.target_position_x_ + 1,
                falling_block_.target_position_z_, falling_block_.height_))
        {
            falling_block_.target_position_x_ += 1;
            target_changed = true;
        }
        break;
    case Visualisation::Action::MoveWest:
        if (!heap_.CheckCollision(
                falling_block_.geometry_, falling_block_.target_position_x_ - 1,
                falling_block_.target_position_z_, falling_block_.height_))
        {
            falling_block_.target_position_x_ -= 1;
            target_changed = true;
        }
        break;
    case Visualisation::Action::RotatetLeft:
        new_geometry = falling_block_.geometry_.Rotate(
            Geometry<BLOCK_SIZE, BLOCK_SIZE>::RotationDirection::Left);

        if (!heap_.CheckCollision(new_geometry, falling_block_.target_position_x_,
                                  falling_block_.target_position_z_,
                                  falling_block_.height_))
        {
            // We must be careful to always keep rotations of falling_block_.geometry_ and
            // its Visualisation::Object* in sync
            falling_block_.geometry_ = new_geometry;
            blocks_[falling_block_.type].second->Rotate(
                glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f), running_time);
        }

        break;
    case Visualisation::Action::RotatetRight:

        new_geometry = falling_block_.geometry_.Rotate(
            Geometry<BLOCK_SIZE, BLOCK_SIZE>::RotationDirection::Right);

        if (!heap_.CheckCollision(new_geometry, falling_block_.target_position_x_,
                                  falling_block_.target_position_z_,
                                  falling_block_.height_))
        {
            falling_block_.geometry_ = new_geometry;
            blocks_[falling_block_.type].second->Rotate(
                -glm::half_pi<float>(), glm::vec3(0.0f, 1.0f, 0.0f), running_time);
        }

        break;
    case Visualisation::Action::RotateForward:
        new_geometry = falling_block_.geometry_.Rotate(
            Geometry<BLOCK_SIZE, BLOCK_SIZE>::RotationDirection::Forward);

        if (!heap_.CheckCollision(new_geometry, falling_block_.target_position_x_,
                                  falling_block_.target_position_z_,
                                  falling_block_.height_))
        {
            falling_block_.geometry_ = new_geometry;
            blocks_[falling_block_.type].second->Rotate(
                glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f), running_time);
        }

        break;
    case Visualisation::Action::RotateBackward:
        new_geometry = falling_block_.geometry_.Rotate(
            Geometry<BLOCK_SIZE, BLOCK_SIZE>::RotationDirection::Backward);

        if (!heap_.CheckCollision(new_geometry, falling_block_.target_position_x_,
                                  falling_block_.target_position_z_,
                                  falling_block_.height_))
        {
            falling_block_.geometry_ = new_geometry;
            blocks_[falling_block_.type].second->Rotate(
                -glm::half_pi<float>(), glm::vec3(0.0f, 0.0f, 1.0f), running_time);
        }
        break;

    case Visualisation::Action::StartBoost:
        boost_on_ = true;
        log_.Info() << "Boost on!";
        break;
    case Visualisation::Action::StopBoost:
        boost_on_ = false;
        log_.Info() << "Boost off!";
        break;

    default:
        ASSERT(false);
    }

    if (target_changed)
    {
        trajectory_movement_x_.UpdateTrajectory(running_time + 0.1f,
                                                falling_block_.target_position_x_);
        trajectory_movement_z_.UpdateTrajectory(running_time + 0.1f,
                                                falling_block_.target_position_z_);
    }
}
