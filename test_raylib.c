#include "raylib.h"

int main(void) {
    InitWindow(900, 600, "Maze 3D");

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(SKYBLUE);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
