/*
ray-snake

by Daniel Postolowicz

*/

#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include <time.h>

// Macros 
#define CELL_SIZE     100
#define GRID_COUNT    8
#define WINDOW_SIZE   (CELL_SIZE * GRID_COUNT)
#define MAX_SNAKE     (GRID_COUNT * GRID_COUNT)   // snake can never be longer than the board
#define APPLE_RADIUS  (CELL_SIZE / 2)
#define MOVE_INTERVAL 0.2f                        // seconds per step; lower = faster

// structs 
typedef struct {
	int x, y;
} Cell;

typedef enum {
	DIR_RIGHT,
	DIR_LEFT,
	DIR_UP,
	DIR_DOWN
} Direction;

typedef struct {
	Cell body[MAX_SNAKE]; // body[0] is the head
	int length;
	Direction dir;        // direction used in the last step
	Direction nextDir;    // direction requested by the player, applied on the next step
} Snake;

typedef struct {
	Snake snake;
	Cell apple;
	int hiscore;
	float moveTimer;
} Game;


// Helpers

static bool CellsEqual(Cell a, Cell b)
{
	return a.x == b.x && a.y == b.y;
}

static Direction Opposite(Direction d)
{
	switch (d) {
	case DIR_LEFT:  return DIR_RIGHT;
	case DIR_RIGHT: return DIR_LEFT;
	case DIR_UP:    return DIR_DOWN;
	default:        return DIR_UP;
	}
}

static Cell DirectionDelta(Direction d)
{
	switch (d) {
	case DIR_LEFT:  return (Cell) { -1, 0 };
	case DIR_RIGHT: return (Cell) { 1, 0 };
	case DIR_UP:    return (Cell) { 0, -1 }; // screen Y grows downwards
	default:        return (Cell) { 0, 1 };
	}
}

// Does any of the first `count` segments of the snake sit on `c`?
static bool SnakeOccupies(const Snake* s, Cell c, int count)
{
	for (int i = 0; i < count; i++) {
		if (CellsEqual(s->body[i], c)) return true;
	}
	return false;
}

static bool InsideBoard(Cell c)
{
	return c.x >= 0 && c.x < GRID_COUNT && c.y >= 0 && c.y < GRID_COUNT;
}


// game logic
// Picks a random FREE cell, so there is no "retry until it works" loop.
// Returns false when the board is completely full (the player won).
static bool SpawnApple(Game* g)
{
	Cell freeCells[MAX_SNAKE];
	int freeCount = 0;

	for (int y = 0; y < GRID_COUNT; y++) {
		for (int x = 0; x < GRID_COUNT; x++) {
			Cell c = { x, y };
			if (!SnakeOccupies(&g->snake, c, g->snake.length)) {
				freeCells[freeCount++] = c;
			}
		}
	}
	if (freeCount == 0) return false;

	g->apple = freeCells[GetRandomValue(0, freeCount - 1)];
	return true;
}

static void ResetGame(Game* g)
{
	g->snake = (Snake){ 0 };
	g->snake.body[0] = (Cell){ GRID_COUNT / 2, GRID_COUNT / 2 };
	g->snake.length = 1;
	g->snake.dir = DIR_RIGHT;
	g->snake.nextDir = DIR_RIGHT;
	g->moveTimer = 0.0f;
	SpawnApple(g);
}

// One single place for "the round ended" (wall, self-collision, full board).
static void GameOver(Game* g)
{
	int score = g->snake.length - 1;
	if (score > g->hiscore) g->hiscore = score;
	ResetGame(g);
}

static void HandleInput(Snake* s)
{
	Direction wanted = s->nextDir;
	if (IsKeyPressed(KEY_W)) wanted = DIR_UP;
	if (IsKeyPressed(KEY_S)) wanted = DIR_DOWN;
	if (IsKeyPressed(KEY_A)) wanted = DIR_LEFT;
	if (IsKeyPressed(KEY_D)) wanted = DIR_RIGHT;

	// Compared with `dir` (last executed move), NOT with `nextDir`,
	// so two quick key presses can't sneak in a 180-degree turn.
	if (wanted != Opposite(s->dir)) s->nextDir = wanted;
}

static void StepSnake(Game* g)
{
	Snake* s = &g->snake;

	s->dir = s->nextDir;
	Cell delta = DirectionDelta(s->dir);
	Cell head = { s->body[0].x + delta.x, s->body[0].y + delta.y };

	if (!InsideBoard(head)) {
		GameOver(g);
		return;
	}
	// The last segment moves away in this step, so the head may enter its cell.
	if (SnakeOccupies(s, head, s->length - 1)) {
		GameOver(g);
		return;
	}

	Cell oldTail = s->body[s->length - 1];
	for (int i = s->length - 1; i > 0; i--) {
		s->body[i] = s->body[i - 1];
	}
	s->body[0] = head;

	if (CellsEqual(head, g->apple)) {
		s->body[s->length++] = oldTail; // grow: new segment appears where the tail was
		if (!SpawnApple(g)) GameOver(g);
	}
}

static void UpdateGame(Game* g)
{
	HandleInput(&g->snake);

	g->moveTimer += GetFrameTime();
	if (g->moveTimer >= MOVE_INTERVAL) {
		g->moveTimer -= MOVE_INTERVAL; // keep the remainder, don't throw it away
		StepSnake(g);
	}
}

//drawing
static Rectangle CellToRect(Cell c)
{
	return (Rectangle) { (float)(c.x * CELL_SIZE), (float)(c.y * CELL_SIZE), CELL_SIZE, CELL_SIZE };
}

static void DrawGrid(void)
{
	for (int i = 0; i <= GRID_COUNT; i++) {
		DrawLine(i * CELL_SIZE, 0, i * CELL_SIZE, WINDOW_SIZE, WHITE);
		DrawLine(0, i * CELL_SIZE, WINDOW_SIZE, i * CELL_SIZE, WHITE);
	}
}

static void DrawGame(const Game* g)
{
	DrawGrid();

	for (int i = 0; i < g->snake.length; i++) {
		DrawRectangleRec(CellToRect(g->snake.body[i]), YELLOW);
	}

	DrawCircle(g->apple.x * CELL_SIZE + CELL_SIZE / 2,
		g->apple.y * CELL_SIZE + CELL_SIZE / 2,
		APPLE_RADIUS, BLUE);

	DrawText(TextFormat("Score: %i", g->snake.length - 1), WINDOW_SIZE - 100, WINDOW_SIZE - 100, 15, WHITE);
	DrawText(TextFormat("HiScore: %i", g->hiscore), 0, WINDOW_SIZE - 100, 15, WHITE);
}
// Game Loop
int main(void)
{
	// Config flags must be set BEFORE InitWindow.
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	InitWindow(WINDOW_SIZE, WINDOW_SIZE, "ray_snake");
	SetTargetFPS(60);
	SearchAndSetResourceDir("resources");
	SetRandomSeed((unsigned int)time(NULL));

	Game game = { 0 };
	ResetGame(&game);
	while (!WindowShouldClose()) {
		UpdateGame(&game);
		BeginDrawing();
		ClearBackground(BLACK);
		DrawGame(&game);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}