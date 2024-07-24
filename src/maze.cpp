#include "maze.hpp"


template <size_t mazeWidth, size_t mazeHeight>
Maze<mazeWidth, mazeHeight>::Maze()
{
    std::random_device rd;
    this->rng = std::mt19937(rd());
}


template <size_t mazeWidth, size_t mazeHeight>
enum class Maze<mazeWidth, mazeHeight>::Direction : unsigned char {
    Up,
    Down,
    Left,
    Right
};


template <size_t mazeWidth, size_t mazeHeight>
int Maze<mazeWidth, mazeHeight>::getCellAt(size_t x, size_t y)
{
    if (x < mazeWidth && y < mazeHeight)
        return maze[x * mazeWidth + y];
    return 0;
}


template <size_t mazeWidth, size_t mazeHeight>
void Maze<mazeWidth, mazeHeight>::setCell(size_t x, size_t y, int type)
{
    maze[x * mazeWidth + y] = type;
}


template <size_t mazeWidth, size_t mazeHeight>
void Maze<mazeWidth, mazeHeight>::carvePath(size_t x, size_t y)
{
    this->setCell(x, y, 0);

    std::vector<Direction> directions = { Direction::Up, Direction::Down, Direction::Left, Direction::Right };

    std::shuffle(directions.begin(), directions.end(), rng);

    for (const Direction& direction : directions) {
        int dx, dy;

        switch (direction) {
            case Direction::Up:
                dx = 0;
                dy = -1;
                break;
            case Direction::Down:
                dx = 0;
                dy = 1;
                break;
            case Direction::Left:
                dx = -1;
                dy = 0;
                break;
            case Direction::Right:
                dx = 1;
                dy = 0;
                break;
        }

        size_t nx = x + dx * 2;
        size_t ny = y + dy * 2;
        if ((nx < mazeWidth) && (ny < mazeHeight) && this->getCellAt(nx, ny) == 1) {
            this->setCell(x + dx, y + dy, 0);
            carvePath(nx, ny);
        }
    }
}


template <size_t mazeWidth, size_t mazeHeight>
void Maze<mazeWidth, mazeHeight>::generateMaze(int wallType)
{
    this->maze.fill(wallType);
    this->carvePath(0, 0);
}
