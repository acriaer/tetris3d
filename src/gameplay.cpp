
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

Gameplay::Gameplay(Visualisation &vis) : vis_(vis)
{
    auto geometry = ShapeToGeometry(tetris_shapes[0]);
    auto obj = new Visualisation::Object(geometry, vis);
    obj->SetVisibility(true);
}