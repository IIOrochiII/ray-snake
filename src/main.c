/*
ray-snake

by Daniel Postolowicz

*/

#include "raylib.h"
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "struc_dec.h"
#include <time.h>
#define WINDOW_HEIGHT 800
#define WINDOW_WIDTH 800
#define BOX_SIZE WINDOW_HEIGHT/8
#define RADIUS 50
#define CIRCLE_OFFSET RADIUS
#define CELL_SIZE 100
#define GRID_COUNT (WINDOW_HEIGHT / CELL_SIZE)

int main()
{
	SetTargetFPS(5);
	// Figures used in game 
	Box box = { { BOX_SIZE, BOX_SIZE, BOX_SIZE, BOX_SIZE }, YELLOW };
	Circle circle = { { BOX_SIZE, BOX_SIZE }, RADIUS, BLUE };
	enum direction {
		right,
		left,
		up,
		down
	} direction;
	enum direction dir_snake = right;
	// Seed for generating new postion of "apple"
	SetRandomSeed((unsigned int)time(NULL));

	// Score variable for displaying numbers
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	// Create the window and OpenGL context
	InitWindow(WINDOW_HEIGHT, WINDOW_WIDTH, "ray_snake");

	// Declaration of grid for snake to move 
	RenderTexture2D grid = LoadRenderTexture(WINDOW_HEIGHT, WINDOW_HEIGHT);
	BeginTextureMode(grid);
	ClearBackground(BLACK);
	for (int x = 0; x <= WINDOW_WIDTH; x += BOX_SIZE) {
		DrawLine(x, 0, x, WINDOW_WIDTH, WHITE);
	}
	for (int y = 0; y <= WINDOW_HEIGHT; y += BOX_SIZE) {
		DrawLine(0, y, WINDOW_HEIGHT, y, WHITE);
	}

	EndTextureMode();
	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");
	int score = 0;
	int apple_eaten = 1;
	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		while (!WindowShouldClose()) {
			// move mechanics
			if (IsKeyPressed(KEY_A) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_W) && !IsKeyDown(KEY_D)) {
				dir_snake = left;
			}
			if (IsKeyPressed(KEY_D) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_W) && !IsKeyDown(KEY_A)) {
				dir_snake = right;
			}
			if (IsKeyPressed(KEY_S) && !IsKeyDown(KEY_D) && !IsKeyDown(KEY_W) && !IsKeyDown(KEY_A)) {
				dir_snake = up;
			}
			if (IsKeyPressed(KEY_W) && !IsKeyDown(KEY_A) && !IsKeyDown(KEY_S) && !IsKeyDown(KEY_D)) {
				dir_snake = down;
			}
			switch (dir_snake) {
			case left:
				box.rec.x -= 100;
				break;
			case right:
				box.rec.x += 100;
				break;
			case up:
				box.rec.y += 100;
				break;
			case down:
				box.rec.y -= 100;
				break;
			}
			
			// collision 

			// right edge (bierzemy pod uwagę szerokość boxa)
			if (box.rec.x + box.rec.width > WINDOW_WIDTH) {
				box.rec.x = WINDOW_WIDTH - box.rec.width;
			}
			// left edge
			if (box.rec.x < 0) {
				box.rec.x = 0;
			}
			// down edge (bierzemy pod uwagę wysokość boxa)
			if (box.rec.y + box.rec.height > WINDOW_HEIGHT) {
				box.rec.y = WINDOW_HEIGHT - box.rec.height;
			}
			// up edge
			if (box.rec.y < 0) {
				box.rec.y = 0;
			}
			// apple managment

			if (apple_eaten == 1) {
				Vector2 boxCenter = (Vector2){
					box.rec.x + box.rec.width / 2.0f,
					box.rec.y + box.rec.height / 2.0f
				};

				do {
					int col = GetRandomValue(0, GRID_COUNT - 1);   // 0..7
					int row = GetRandomValue(0, GRID_COUNT - 1);   // 0..7
					circle.center.x = col * CELL_SIZE + CIRCLE_OFFSET;
					circle.center.y = row * CELL_SIZE + CIRCLE_OFFSET;
				} while (Vector2Distance(boxCenter, (Vector2) { circle.center.x, circle.center.y }) < 100.0f);

				apple_eaten = 0; 
			}
			if (CheckCollisionCircleRec((Vector2){circle.center.x, circle.center.y }, RADIUS, box.rec)) {
				apple_eaten = 1;
				score += 1;
			}


			// drawing
			BeginDrawing();
			// Setup the back buffer for drawing (clear color and depth buffers)
			ClearBackground(BLACK);
			DrawTextureRec(grid.texture,
				(Rectangle) {
				0, 0, (float)grid.texture.width, -(float)grid.texture.height
			},
				(Vector2) {
				0, 0
			}, WHITE);
			// draw some text using the default font
			DrawText("ray-snake", 0, 0, 0, WHITE);

			DrawRectangleRec(box.rec, box.color);
			DrawCircle((int)circle.center.x , (int)circle.center.y ,
				circle.radius, circle.color);

			// end the frame and get ready for the next one  (display frame, poll input, etc...)
			EndDrawing();
		}

		// cleanup
		// unload our texture so it can be cleaned up
		// destroy the window and cleanup the OpenGL context
		CloseWindow();
		return 0;
	}
}
