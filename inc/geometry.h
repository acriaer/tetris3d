#pragma once

#include "exceptions.h"
#include <array>

template <int W, int H> class Geometry
{
  public:
    std::vector<std::array<uint32_t, W * H>> heap_;

    uint32_t &Element(int x, int z, int h) { return heap_[h][z * W + x]; }

    void AddEmptyLayer() { heap_.emplace_back(); }
    void AddFullLayer()
    {
        std::array<uint32_t, W * H> layer;
        layer.fill(1);
        heap_.push_back(layer);
    }

    void AddLayer(std::array<uint32_t, W * H> layer) { heap_.emplace_back(layer); }

    template <int OTHER_W, int OTHER_H>
    void Merge(Geometry<OTHER_W, OTHER_H> &other, int offset_x, int offset_z,
               int offset_height)
    {
        for (unsigned int h = 0; h < other.heap_.size(); h++)
        {
            for (unsigned int x = 0; x < OTHER_W; x++)
            {
                for (unsigned int z = 0; z < OTHER_H; z++)
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
                        Element(x + offset_x, z + offset_z, h + offset_height) =
                            other.Element(x, z, h);
                    
                }
            }
        }
    }

    template <int OTHER_W, int OTHER_H>
    bool CheckCollision(Geometry<OTHER_W, OTHER_H> &other, int offset_x, int offset_z,
                        int offset_height)
    {
        for (unsigned int h = 0; h < other.heap_.size(); h++)
        {
            for (unsigned int x = 0; x < OTHER_W; x++)
            {
                for (unsigned int z = 0; z < OTHER_H; z++)
                {
                    if (other.Element(x, z, h))
                    {
                        if (x + offset_x < 0 || x + offset_x >= W)
                            return true;
                        if (z + offset_z < 0 || z + offset_z >= H)
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

    enum RotationDirection
    {
        Forward,
        Backward,
        Left,
        Right
    };

    Geometry<W, H> Rotate(RotationDirection dir)
    {
        Geometry<W, H> ret;
        ret.heap_.resize(heap_.size());
        ASSERT(heap_.size() == W);
        ASSERT(heap_.size() == H);

        for (unsigned int h = 0; h < heap_.size(); h++)
            for (unsigned int x = 0; x < W; x++)
                for (unsigned int z = 0; z < H; z++)
                    switch (dir)
                    {
                    case Left:
                        ret.Element(z, (W - 1 - x), h) = Element(x, z, h);
                        break;
                    case Right:
                        ret.Element((W - z - 1), x, h) = Element(x, z, h);
                        break;
                    case Forward:
                        ret.Element((W - 1 - h), z, x) = Element(x, z, h);
                        break;
                    case Backward:
                        ret.Element(h, z, (W - 1 - x)) = Element(x, z, h);
                        break;
                    }

        return ret;
    }

    void Repaint(uint32_t r, uint32_t g, uint32_t b)
    {
        for (unsigned int h = 0; h < heap_.size(); h++)
            for (unsigned int x = 0; x < W; x++)
                for (unsigned int z = 0; z < H; z++)
                    if (Element(x, z, h))
                        Element(x, z, h) = r + (g << 8) + (b << 16);
    }

    bool CheckFullLayer(int layer)
    {
        for (unsigned int x = 0; x < W; x++)
            for (unsigned int z = 0; z < H; z++)
                if (!Element(x, z, layer))
                    return false;

        return true;
    }

    void RemoveLayer(int layer) { heap_.erase(heap_.begin() + layer); }

    template <int OTHER_W, int OTHER_H>
    bool Collides(const Geometry<OTHER_W, OTHER_H> &other, int offset_x, int offset_z,
                  int offset_height);
};