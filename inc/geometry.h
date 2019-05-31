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
    void Merge(Geometry<OTHER_W, OTHER_H> &other, int offset_x, int offset_z,
               int offset_height)
    {
        for (int h = 0; h < other.heap_.size(); h++)
        {
            bool layer_touched = false;

            for (int x = 0; x < OTHER_W; x++)
            {
                for (int z = 0; z < OTHER_H; z++)
                {
                    if (x + offset_x < 0 || x + offset_x >= W)
                        continue;
                    if (z + offset_z < 0 || z + offset_z >= H)
                        continue;
                    if (h + offset_height < 0)
                        continue;

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
    bool CheckCollision(Geometry<OTHER_W, OTHER_H> &other, int offset_x, int offset_z,
                        int offset_height)
    {
        for (int h = 0; h < other.heap_.size(); h++)
        {
            for (int x = 0; x < OTHER_W; x++)
            {
                for (int z = 0; z < OTHER_H; z++)
                {
                    if (other.Element(x, z, h))
                    {
                        if (x + offset_x < 0 || x + offset_x >= W)
                            return true;
                        if (z + offset_z < 0 || z + offset_z >= H)
                            return true;
                        if (h + offset_height < 0)
                            return true;

                        if (h + offset_height >= heap_.size())
                            continue;

                        if (other.Element(x, z, h) &&
                            Element(x + offset_x, z + offset_z, h + offset_height))
                        {
                            return true;
                        }
                    }
                }
            }
        }

        return false;
    }

    template <int OTHER_W, int OTHER_H>
    bool Collides(const Geometry<OTHER_W, OTHER_H> &other, int offset_x, int offset_z,
                  int offset_height);
};