#pragma once

#include "exceptions.h"
#include <array>

template <int W, int H> class Geometry
{
  public:
    std::vector<std::array<uint8_t, W * H>> heap_;

    uint8_t &Element(int x, int z, int h) { return heap_[h][z * W + x]; }

    void AddEmptyLayer() { heap_.emplace_back(); }

    void AddLayer(std::array<uint8_t, W * H> layer) { heap_.emplace_back(layer); }

    template <int OTHER_W, int OTHER_H>
    void Merge(const Geometry<OTHER_W, OTHER_H> &other, int offset_x, int offset_z,
               int offset_height)
    {
        for (int h = 0; h < other.heap_size(); h++)
        {
            bool layer_touched = false;

            for (int x = 0; x < OTHER_W; x++)
            {
                for (int z = 0; z < OTHER_H; z++)
                {
                    while (h + offset_height >= heap_.size())
                        AddEmptyLayer();

                    if (other.Element(x, z, h))
                    {
                        Element(x + offset_x, z + offset_z, h + offset_height) =
                            other.Element(x, z, h);

                        layer_touched = true;
                    }
                }
            }
        }
    }

    template <int OTHER_W, int OTHER_H>
    bool Collides(const Geometry<OTHER_W, OTHER_H> &other, int offset_x, int offset_z,
                  int offset_height);
};