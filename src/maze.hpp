#pragma once
#include <array>
#include <vector>
#include <cstdlib>
#include <random>
#include <algorithm>


template <std::size_t mazeWidth, std::size_t mazeHeight>
class Maze
{
public:
    Maze();

    int getCellAt(size_t x, size_t y);
    void setCell(size_t x, size_t y, int type);

    void generateMaze(int wallType);

private:
    std::mt19937 rng;

    std::array<int, mazeWidth * mazeHeight> maze{};
    enum class Direction : unsigned char;

    void carvePath(size_t x, size_t y);
};
