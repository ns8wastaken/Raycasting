#pragma once
#include <raylib.h>
#include <raymath.h>
#include <rayoverloads.h>


struct Player
{
    Player(Vector2 _pos, Vector2 _direction, float _fov)
        : pos(_pos), direction(_direction), fov(_fov)
    {}

    void rotate(float angle)
    {
        // float c = std::cos(angle);
        // float s = std::sin(angle);

        // direction.x = direction.x * c - direction.y * s;
        // direction.y = direction.x * s + direction.y * c;

        this->direction = Vector2Rotate(this->direction, angle);

        this->direction = Vector2Normalize(direction);
    }

    void walk(float distance)
    {
        this->pos += direction * distance;
    }

    Vector2 pos;
    Vector2 direction;
    float fov;
};
