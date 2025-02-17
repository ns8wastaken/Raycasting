#include <raylib.h>
#include <raymath.h>
#include <array>
#include <limits>

#include "src/player.hpp"
#include "src/maze.cpp"


// Must be odd numbers
#define MAZE_WIDTH  21
#define MAZE_HEIGHT 21


// Player =============================================
#define PLAYER_STARTING_POS       Vector2{ 0.5f, 0.5f }
#define PLAYER_STARTING_DIRECTION Vector2{ 0.5f, 0.5f }
#define PLAYER_FOV                90.0f

#define PLAYER_ROTATE_AMOUNT 2.0f
#define PLAYER_WALK_SPEED    1.5f
// ====================================================

#define RENDER_DISTANCE 6

#define SCREEN_WIDTH       1000
#define SCREEN_HEIGHT      800
#define HALF_SCREEN_HEIGHT (SCREEN_HEIGHT / 2.0f)
#define SCREEN_RESOLUTION  100.0f

#define WALL_WIDTH (SCREEN_WIDTH / SCREEN_RESOLUTION)

#define MINIMAP_SIZE 10.0f

#define RADIANS(x) (x * PI/180.0f)


bool coordsAreInMaze(Vector2& coords)
{
    return (coords.x < MAZE_WIDTH && coords.x >= 0 && coords.y < MAZE_HEIGHT && coords.y >= 0);
}


void DrawMinimap(Player& player, Maze<MAZE_WIDTH, MAZE_HEIGHT>& maze, float size)
{
    // Draw cells
    for (std::size_t y = 0; y < MAZE_HEIGHT; ++y) {
        for (std::size_t x = 0; x < MAZE_WIDTH; ++x) {
            if (maze.getCellAt(x, y)) {
                DrawRectangleV({ (float)x * size, (float)y * size }, { size, size }, BLUE);
            } else {
                DrawRectangleV({ (float)x * size, (float)y * size }, { size, size }, BLACK);
                DrawRectangleLinesEx({ (float)x * size, (float)y * size, size, size }, 1, GRAY);
            }
        }
    }

    // Draw player
    Vector2 p_pos = player.pos * size;
    Vector2 fov1 = p_pos + Vector2Rotate(player.direction * 2.0f, -RADIANS(PLAYER_FOV) * 0.5f) * size;
    Vector2 fov2 = p_pos + Vector2Rotate(player.direction * 2.0f, RADIANS(PLAYER_FOV) * 0.5f) * size;
    DrawLineEx(p_pos, fov1, 4, PURPLE);
    DrawLineEx(p_pos, fov2, 4, PURPLE);
    DrawLineEx(fov1, fov2, 4, PURPLE);

    DrawCircleV(p_pos, size * 0.5f, GREEN);
}


Vector2 rayIntersectPoint(Vector2& rayStart, Vector2& rayDirection, Maze<MAZE_WIDTH, MAZE_HEIGHT>& maze)
{
    // Current cell in the grid
    int mapX = (int)rayStart.x;
    int mapY = (int)rayStart.y;

    // Length of ray from one x or y-side to next x or y-side
    Vector2 deltaDist = {
        (rayDirection.x == 0) ? std::numeric_limits<float>::infinity()
                              : std::abs(1.0f / rayDirection.x),
        (rayDirection.y == 0) ? std::numeric_limits<float>::infinity()
                              : std::abs(1.0f / rayDirection.y)
    };

    // Length of ray from current position to next x or y-side
    Vector2 sideDist = { 0.0f, 0.0f };
    // Direction to step in (either +1 or -1)
    int stepX, stepY;

    if (rayDirection.x < 0) {
        stepX = -1;
        sideDist.x = (rayStart.x - mapX) * deltaDist.x;
    } else {
        stepX = 1;
        sideDist.x = (mapX + 1.0f - rayStart.x) * deltaDist.x;
    }
    if (rayDirection.y < 0) {
        stepY = -1;
        sideDist.y = (rayStart.y - mapY) * deltaDist.y;
    } else {
        stepY = 1;
        sideDist.y = (mapY + 1.0f - rayStart.y) * deltaDist.y;
    }

    bool side; // NS or EW wall hit
    for (int i = 0; i < RENDER_DISTANCE; ++i) {
        // Go to next cell in either x-direction or y-direction
        if (sideDist.x < sideDist.y) {
            sideDist.x += deltaDist.x;
            mapX += stepX;
            side = false;
        } else {
            sideDist.y += deltaDist.y;
            mapY += stepY;
            side = true;
        }

        if ((unsigned)mapY < MAZE_HEIGHT && (unsigned)mapX < MAZE_WIDTH && maze.getCellAt(mapX, mapY) != 0) {
            if (side) {
                return Vector2{ rayStart.x + (mapY - rayStart.y + (1 - stepY) / 2) / rayDirection.y * rayDirection.x, (float)(mapY + (stepY <= 0)) };
            } else {
                return Vector2{ (float)(mapX + (stepX <= 0)), rayStart.y + (mapX - rayStart.x + (1 - stepX) / 2) / rayDirection.x * rayDirection.y };
            }
        } else if (mapY == MAZE_HEIGHT || mapY == -1 || mapX == MAZE_WIDTH || mapX == -1) {
            if (side) {
                return Vector2{ rayStart.x + (mapY - rayStart.y + (1 - stepY) / 2) / rayDirection.y * rayDirection.x, (float)(mapY + (stepY <= 0)) };
            } else {
                return Vector2{ (float)(mapX + (stepX <= 0)), rayStart.y + (mapX - rayStart.x + (1 - stepX) / 2) / rayDirection.x * rayDirection.y };
            }
        }
    }

    return Vector2{ -1.0f, -1.0f };
}


void RaycastWalls(Player& player, Maze<MAZE_WIDTH, MAZE_HEIGHT>& maze)
{
    Vector2 rayDirection = Vector2Rotate(player.direction, -RADIANS(PLAYER_FOV) * 0.5f);
    for (float i = 0; i < SCREEN_RESOLUTION; ++i) {
        Vector2 rayHit = rayIntersectPoint(player.pos, rayDirection, maze);
        rayDirection = Vector2Rotate(rayDirection, RADIANS(PLAYER_FOV) / (SCREEN_RESOLUTION - 1));

        if (rayHit.x != -1.0f && rayHit.y != -1.0f) {
            float rayDist = Vector2Distance(player.pos, rayHit);
            float wallHeight = HALF_SCREEN_HEIGHT / rayDist;
            unsigned char n = (unsigned char)(255 - (rayDist / RENDER_DISTANCE * 255));
            DrawRectangleV({ i * WALL_WIDTH, HALF_SCREEN_HEIGHT - wallHeight * 0.5f }, { WALL_WIDTH, wallHeight }, Color{ 0, n, n, 255 });
        }
    }
}


int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Raycasting");

    Player player(PLAYER_STARTING_POS, PLAYER_STARTING_DIRECTION, PLAYER_FOV);

    Maze<MAZE_WIDTH, MAZE_HEIGHT> maze;

    maze.generateMaze(1);

    while (!WindowShouldClose()) {
        float deltaTime = GetFrameTime();

        BeginDrawing();
        ClearBackground(BLACK);

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D)) {
            player.rotate(PLAYER_ROTATE_AMOUNT * deltaTime);
        }
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A)) {
            player.rotate(-PLAYER_ROTATE_AMOUNT * deltaTime);
        }
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W)) {
            player.walk(PLAYER_WALK_SPEED * deltaTime);
            if (coordsAreInMaze(player.pos)) {
                if (maze.getCellAt((int)player.pos.x, (int)player.pos.y))
                    player.walk(-PLAYER_WALK_SPEED * deltaTime);
            } else {
                player.walk(-PLAYER_WALK_SPEED * deltaTime);
            }
        }
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
            player.walk(-PLAYER_WALK_SPEED * deltaTime);
            if (coordsAreInMaze(player.pos)) {
                if (maze.getCellAt((int)player.pos.x, (int)player.pos.y))
                    player.walk(PLAYER_WALK_SPEED * deltaTime);
            } else {
                player.walk(PLAYER_WALK_SPEED * deltaTime);
            }
        }

        // Floor ?
        // DrawRectangle(0, HALF_SCREEN_HEIGHT, SCREEN_WIDTH, HALF_SCREEN_HEIGHT, WHITE);
        RaycastWalls(player, maze);

        DrawMinimap(player, maze, MINIMAP_SIZE);


        EndDrawing();
    }

    CloseWindow();
    return 0;
}
