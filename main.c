#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define to_ind(r, c) (r) * MAZE_ROWS + (c)
#define in_bound(num, low, high) ((num) >= (low) && (num) < (high))

typedef struct {
    size_t id;
    bool visited;
} Cell;

Cell cell_init(size_t id) {
    return (Cell) {
        .id = id,
        .visited = false,
    };
}

typedef enum {
    CENTER,
    NORTH,
    SOUTH,
    WEST,
    EAST,
} Dir;

typedef enum {
    VERTICAL,
    HORIZONTAL
} WallType;

typedef struct {
    size_t start;
    size_t target;
    WallType type;
} Wall;

Wall wall_init(size_t start, size_t target, WallType type) {
    return (Wall) {
        .start = start,
        .target = target,
        .type = type
    };
}

#define MAZE_ROWS 30
#define MAZE_COLS 30

#define STACK_TYPE Cell*
#define STACK_H_IMPLEMENTATION
#include "stack.h"

#define VEC_TYPE Wall
#define VEC_H_IMPLEMENTATION
#include "vec.h"

typedef struct {
    Cell grid[MAZE_ROWS*MAZE_COLS];
    Stack stack;
    // Source: https://math.stackexchange.com/questions/4350136/how-many-adjacent-edges-in-an-n-times-n-grid-of-squares
    // For any nxn grid, the number of adjacent edges is defined by the formula: (2*n)*(n-1)
    Vec removed_walls;
} Env;

Env env_init() {
    Env env = {0};
    // Reset grid
    for (size_t r = 0; r < MAZE_ROWS; r++) {
        for (size_t c = 0; c < MAZE_COLS; c++) {
            env.grid[to_ind(r, c)] = cell_init(to_ind(r, c));
        }
    }
    env.removed_walls = vec_init();
    env.stack = stack_init();
    return env;
}

void env_deinit(Env* env) {
    stack_deinit(&env->stack);
    vec_deinit(&env->removed_walls);
}

void shuffle(Dir sides[4]) {
    size_t n = 4;
    for (size_t i = n - 1; i >= 1; i--) {
        size_t j = rand() % i;
        Dir temp = sides[i];
        sides[i] = sides[j];
        sides[j] = temp;
    }
}

Dir unvisited_neighbors(Cell* grid, int row, int col) {
    (void) grid;
    (void) row;
    (void) col;
    Dir sides[4] =  {NORTH, SOUTH, EAST, WEST};
    shuffle(sides);
    int new_row = row;
    int new_col = col;
#if 1
    for (size_t i = 0; i < 4; i++) {
        switch (sides[i]) {
            case NORTH: new_row = row - 1; break;
            case SOUTH: new_row = row + 1; break;
            case  WEST: new_col = col - 1; break;
            case  EAST: new_col = col + 1; break;
            default: break; // CENTER
        }
        if (in_bound(new_row, 0, MAZE_ROWS) &&
            in_bound(new_col, 0, MAZE_COLS) &&
            !grid[to_ind(new_row, new_col)].visited) {
            return sides[i];
        }

        new_row = row;
        new_col = col;
    }

    // exit(0);
#else
    int new_row = row - 1;
    if (in_bound(new_row, 0, MAZE_ROWS) && !grid[to_ind(new_row, col)].visited) {
        return NORTH;
    }
    new_row = row + 1;
    if (in_bound(new_row, 0, MAZE_ROWS) && !grid[to_ind(new_row, col)].visited) {
        return SOUTH;
    }
    int new_col = col + 1;
    if (in_bound(new_col, 0, MAZE_COLS) && !grid[to_ind(row, new_col)].visited) {
        return EAST;
    }
    new_col = col - 1;
    if (in_bound(new_col, 0, MAZE_COLS) && !grid[to_ind(row, new_col)].visited) {
        return WEST;
    }
#endif
    return CENTER;
}

void remove_wall(Vec* walls, size_t start, size_t target) {
    assert(start != target);
    int row_diff = (start / MAZE_ROWS) - (target / MAZE_ROWS);
    WallType type = row_diff != 0 ? HORIZONTAL : VERTICAL;
    // WallType type = row_diff != 0 ? VERTICAL : HORIZONTAL;
    int col_diff = (start % MAZE_ROWS) - (target % MAZE_ROWS);
    if (row_diff > 0 || col_diff > 0) {
        vec_append(walls, wall_init(target, start, type));
    } else if (row_diff < 0 || col_diff < 0) {
        vec_append(walls, wall_init(start, target, type));
    }
}

Env env;

void gen_maze() {
    // Random initial cell
    int row = rand() % MAZE_ROWS;
    int col = rand() % MAZE_COLS;
    Cell* current = &env.grid[to_ind(row, col)];
    // Mark current as visited
    current->visited = true;
    // Push random initial cell to the stack
    stack_push(&env.stack, current);

    int removed = 0;
    while (env.stack.count > 0) {
        // Pop cell from the stack
        current = stack_pop(&env.stack);
        // Updating the current cell's row and column
        row = current->id / MAZE_ROWS;
        col = current->id % MAZE_ROWS;
        // Unvisited neighbors of the current cell
        Dir unvisited = unvisited_neighbors(env.grid, row, col);
        if (unvisited == CENTER) continue;
        // Push the current cell to the stack
        stack_push(&env.stack, current);

        int chosen_row = row;
        int chosen_col = col;
        switch (unvisited) {
            case NORTH: chosen_row = row - 1; break;
            case SOUTH: chosen_row = row + 1; break;
            case  WEST: chosen_col = col - 1; break;
            case  EAST: chosen_col = col + 1; break;
            default: // CENTER
                fprintf(stderr, "Unreachable!\n");
                assert(false);
                break;
        }
        Cell* chosen = &env.grid[to_ind(chosen_row, chosen_col)]; 
        // Remove wall between current and chosen cell
        remove_wall(&env.removed_walls, to_ind(row, col), to_ind(chosen_row, chosen_col));
        removed++;
        // ---- printf("Wall from [r=%d, c=%d] to [r=%d, c=%d] is\tREMOVED\n", row, col, chosen_row, chosen_col);
        // Mark chosen cell as visited
        chosen->visited = true;
        stack_push(&env.stack, chosen);
    }
}

#define SOLID 0x32A852
#if 1
#define OPEN 0x0 // BLACK Color
#else
#define OPEN 0x2856A1 // BLUE Color
#endif

#define OPEN_WIDTH 10
#define OPEN_HEIGHT 10
#define BORDER_THICKNESS 1

#define IMG_WIDTH (MAZE_COLS * OPEN_WIDTH) + ((MAZE_COLS+1) * BORDER_THICKNESS)
#define IMG_HEIGHT (MAZE_ROWS * OPEN_HEIGHT) + ((MAZE_ROWS+1) * BORDER_THICKNESS)
uint32_t pixels[IMG_HEIGHT][IMG_WIDTH];

void fill_rect(size_t rx, size_t ry, size_t rw, size_t rh, uint32_t color) {
    assert(rx + rw <= IMG_WIDTH);
    assert(ry + rh <= IMG_HEIGHT);
    for (size_t y = ry; y < (ry + rh); y++) {
        for (size_t x = rx; x < (rx + rw); x++) {
            pixels[y][x] = color;
        }
    }
}

void draw_maze() {
    size_t y, x;
    for (size_t r = 0; r < MAZE_ROWS; r++) {
        for (size_t c = 0; c <= MAZE_COLS; c++) {
            y = (r * OPEN_HEIGHT) + (r * BORDER_THICKNESS);
            x = (c * OPEN_WIDTH) + (c * BORDER_THICKNESS);
            fill_rect(x, y, BORDER_THICKNESS, OPEN_HEIGHT + (2*BORDER_THICKNESS), SOLID);
        }
    }

    for (size_t r = 0; r <= MAZE_ROWS; r++) {
        for (size_t c = 0; c < MAZE_COLS; c++) {
            y = (r * OPEN_HEIGHT) + (r * BORDER_THICKNESS);
            x = (c * OPEN_WIDTH) + (c * BORDER_THICKNESS);
            fill_rect(x, y, OPEN_WIDTH + (2*BORDER_THICKNESS), BORDER_THICKNESS, SOLID);
        }
    }

    for (size_t i = 0; i < env.removed_walls.length; i++) {
        // size_t start[2] = {
        //     env.removed_walls.items[i].start / MAZE_ROWS,
        //     env.removed_walls.items[i].start % MAZE_ROWS
        // };
        size_t target[2] = {
            env.removed_walls.items[i].target / MAZE_ROWS,
            env.removed_walls.items[i].target % MAZE_ROWS
        };
        // printf("REMOVED\t(r = %lu, c = %lu)\t(r = %lu, c = %lu)\n", start[0], start[1], target[0], target[1]);
        WallType type = env.removed_walls.items[i].type;
        switch (type) {
            case VERTICAL:
                fill_rect(
                    target[1] * OPEN_WIDTH + target[1] * BORDER_THICKNESS, 
                    target[0] * OPEN_HEIGHT + target[0] * BORDER_THICKNESS + BORDER_THICKNESS,
                    BORDER_THICKNESS, OPEN_HEIGHT, OPEN);
                break;
            case HORIZONTAL:
                fill_rect(
                    target[1] * OPEN_WIDTH + target[1] * BORDER_THICKNESS + BORDER_THICKNESS,
                    target[0] * OPEN_HEIGHT + target[0] * BORDER_THICKNESS,
                    OPEN_WIDTH, BORDER_THICKNESS, OPEN);
                break;
            default: break;
        }
    }
}

void save_as_ppm(const char* filename) {
    FILE* fp = fopen(filename, "w");
    if (fp == NULL) {
        fprintf(stderr, "ERROR: Failed to open '%s' for writing\n", filename);
        exit(1);
    }

    fprintf(fp, "P6\n%d %d 255\n", IMG_WIDTH, IMG_HEIGHT);
    for (size_t r = 0; r < IMG_HEIGHT; r++) {
        for (size_t c = 0; c < IMG_WIDTH; c++) {
            uint32_t pixel = pixels[r][c];
            // Color HEX code format: 0xRRGGBB
            uint8_t bytes[3] = {
                (pixel >> 8*2) & 0xFF, //     0xRR & 0xFF
                (pixel >> 8*1) & 0xFF, //   0x__GG & 0xFF
                (pixel >> 8*0) & 0xFF, // 0x____BB & 0xFF
            };
            fwrite(bytes, sizeof(bytes), 1, fp);
        }
    }

    fclose(fp);
}

int main(void) {
    srand(time(NULL));
    env = env_init();
    gen_maze();
    draw_maze();
    save_as_ppm("out.ppm");
    env_deinit(&env);
    return 0;
}
