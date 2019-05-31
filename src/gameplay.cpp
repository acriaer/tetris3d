
#include "gameplay.h"

// clang-format off
static const std::vector<std::array<uint8_t, BLOCK_SIZE*BLOCK_SIZE>> tetris_shapes = {
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
ShapeToGeometry(const std::array<uint8_t, BLOCK_SIZE * BLOCK_SIZE> &shape)
{
    Geometry<BLOCK_SIZE, BLOCK_SIZE> ret;

    for (int i = 0; i < BLOCK_SIZE / 2; i++)
        ret.AddEmptyLayer();
    ret.AddLayer(shape);
    for (int i = 0; i < BLOCK_SIZE / 2; i++)
        ret.AddEmptyLayer();

    return ret;
}

Gameplay::Gameplay(Visualisation &vis) : vis_(vis), last_time_(0.0f)
{
    for (auto &shape : tetris_shapes)
    {
        auto geometry = ShapeToGeometry(shape);
        auto object = vis.CreateObject();
        object->LoadGeometry(geometry);
        blocks_.emplace_back(std::make_pair(geometry, object));
    }

    heap_object_ = vis.CreateObject();
    InitNewFallingBlock();
}

void Gameplay::InitNewFallingBlock()
{
    blocks_[falling_block_.type].second->SetVisibility(false);

    falling_block_.type = 0; // FIXME

    falling_block_.geometry_ = blocks_[falling_block_.type].first;
    blocks_[falling_block_.type].second->SetVisibility(true);
    falling_block_.target_position_x_ = BOARD_SIZE / 2;
    falling_block_.target_position_z_ = BOARD_SIZE / 2;
    falling_block_.height_ = 50.0f;
}

void Gameplay::HandleAction(Visualisation::Action action)
{
    switch (action)
    {
    case Visualisation::Action::MoveNorth:
        if (!heap_.CheckCollision(
                falling_block_.geometry_, falling_block_.target_position_x_,
                falling_block_.target_position_z_ + 1, falling_block_.height_))
            falling_block_.target_position_z_ += 1;
        break;
    case Visualisation::Action::MoveSouth:
        if (!heap_.CheckCollision(
                falling_block_.geometry_, falling_block_.target_position_x_,
                falling_block_.target_position_z_ - 1, falling_block_.height_))
            falling_block_.target_position_z_ -= 1;
        break;
    case Visualisation::Action::MoveEast:
        if (!heap_.CheckCollision(
                falling_block_.geometry_, falling_block_.target_position_x_ + 1,
                falling_block_.target_position_z_, falling_block_.height_))
            falling_block_.target_position_x_ += 1;
        break;
    case Visualisation::Action::MoveWest:
        if (!heap_.CheckCollision(
                falling_block_.geometry_, falling_block_.target_position_x_ - 1,
                falling_block_.target_position_z_, falling_block_.height_))
            falling_block_.target_position_x_ -= 1;
        break;
    }
}

void Gameplay::Update(float running_time)
{
    float delta_time = running_time - last_time_;

    falling_block_.height_ -= 10.0f * delta_time;

    blocks_[falling_block_.type].second->SetPostion(
        glm::vec3(falling_block_.target_position_x_, falling_block_.height_,
                  falling_block_.target_position_z_));

    if (heap_.CheckCollision(falling_block_.geometry_, falling_block_.target_position_x_,
                             falling_block_.target_position_z_, falling_block_.height_))
    {
        heap_.Merge(falling_block_.geometry_, falling_block_.target_position_x_,
                    falling_block_.target_position_z_, falling_block_.height_ + 1.0f);

        heap_object_->LoadGeometry(heap_);
        heap_object_->SetVisibility(true);
        InitNewFallingBlock();
    }

    last_time_ = running_time;
}