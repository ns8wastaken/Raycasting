#include "maze.hpp"


template <std::size_t mazeWidth, std::size_t mazeSize>
int Maze<mazeWidth, mazeSize>::getCellAt(int x, int y)
{
    return maze[x * mazeWidth + y];
}


template <std::size_t mazeWidth, std::size_t mazeSize>
void Maze<mazeWidth, mazeSize>::setCell(int x, int y, int type)
{
    maze[x * mazeWidth + y] = type;
}
