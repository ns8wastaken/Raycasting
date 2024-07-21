#pragma once
#include <array>


template <std::size_t mazeWidth, std::size_t mazeSize>
class Maze
{
public:
    int getCellAt(int x, int y);
    void setCell(int x, int y, int type);

private:
    std::array<int, mazeSize> maze{};
};
