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
#define MAX_SIZE 100


int main()
{
	SetTargetFPS(60);
	float moveTimer = 0.0f;
	int hiscore = 0;
	float moveInterval = 0.2f; // Lower = faster. 0.2f means it moves 5 times per second.
	// Figures used in game 
	Box box = { { BOX_SIZE, BOX_SIZE, BOX_SIZE, BOX_SIZE }, YELLOW };
	Circle circle = { { BOX_SIZE, BOX_SIZE }, RADIUS, BLUE };
	Box snake_cords[MAX_SIZE];
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
	while (!WindowShouldClose()) {
		// move mechanics
		// 1. Define these variables outside your game loop (e.g., where box and dir_snake are declared)
		if (IsKeyPressed(KEY_A) && dir_snake != right) {
			dir_snake = left;
		}
		if (IsKeyPressed(KEY_D) && dir_snake != left) {
			dir_snake = right;
		}
		if (IsKeyPressed(KEY_S) && dir_snake != down) {
			dir_snake = up;
		}
		if (IsKeyPressed(KEY_W) && dir_snake != up) {
			dir_snake = down;
		}
		moveTimer += GetFrameTime();
		if (moveTimer >= moveInterval) {
			Box prev_box = box;
			Box prev_box2;
			for (int i = 0;i < score;i++) {
				prev_box2 = snake_cords[i];
				snake_cords[i] = prev_box;
				prev_box = prev_box2;
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
			for (int i = 1; i < score; i++) {
				if (box.rec.x == snake_cords[i].rec.x && box.rec.y == snake_cords[i].rec.y) {
					if (score > hiscore) {
						hiscore = score;
					}
					score = 0;
					for (int i = 0; i < MAX_SIZE; i++) {
						snake_cords[i].rec.x = -1000;
						snake_cords[i].rec.x = -1000;
					}
					box.rec.x = 0;
					box.rec.y = 0;
					dir_snake = right;
					break;
				}
			}
			moveTimer = 0.0f;
		}

		// collision 
		int hit_wall = 0;
		// right edge(we consider box width)
		if (box.rec.x + box.rec.width > WINDOW_WIDTH) {
			box.rec.x = WINDOW_WIDTH - box.rec.width;
		}
		// left edge
		if (box.rec.x < 0) {
			box.rec.x = 0;
			hit_wall = 1;
		}
		// down edge (we consider box height)
		if (box.rec.y + box.rec.height > WINDOW_HEIGHT) {
			box.rec.y = WINDOW_HEIGHT - box.rec.height;
			hit_wall = 1;
		}
		// up edge
		if (box.rec.y < 0) {
			box.rec.y = 0;
			hit_wall = 1;
		}
		if (hit_wall) {
			if (score > hiscore) {
				hiscore = score;
			}
			score = 0;
			box.rec.x = 0;
			box.rec.y = 0;
			dir_snake = right;
			for (int i = 0; i < MAX_SIZE; i++) {
				snake_cords[i].rec.x = -1000;
				snake_cords[i].rec.x = -1000;
			}
		}
		// apple managment
		if (CheckCollisionCircleRec((Vector2) { circle.center.x, circle.center.y }, 40, box.rec)) {
			apple_eaten = 1;
			score += 1;
		}

		if (apple_eaten == 1) {
			bool position_invalid = true;
			while (position_invalid) {
				// 1.Get new apple position
				int col = GetRandomValue(0, GRID_COUNT - 1);
				int row = GetRandomValue(0, GRID_COUNT - 1);
				circle.center.x = col * CELL_SIZE + CIRCLE_OFFSET;
				circle.center.y = row * CELL_SIZE + CIRCLE_OFFSET;

				// At first we assume that position of apple is good
				position_invalid = false;

				// 2. We check out the collision the snake's head
				Vector2 boxCenter = (Vector2){
					box.rec.x + box.rec.width / 2.0f,
					box.rec.y + box.rec.height / 2.0f
				};
				if (Vector2Distance(boxCenter, circle.center) < 50.0f) { // Reduced to 50.0f
					position_invalid = true;
					continue; // Head collider we draw once again without checking the head
				}

				// 3. We check collision with every tail element
				for (int i = 0; i < score; i++) {
					Vector2 tail_element_center = (Vector2){
						snake_cords[i].rec.x + snake_cords[i].rec.width / 2.0f,
						snake_cords[i].rec.y + snake_cords[i].rec.height / 2.0f
					};

					if (Vector2Distance(tail_element_center, circle.center) < 50.0f) {
						position_invalid = true; // Collision with snake's tail found
						break; // We're breaking the loop, we need to randomize apple's position from beginning
					}
				}
			}

			apple_eaten = 0;
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
		DrawText(TextFormat("Score: %i", score), WINDOW_HEIGHT - 100, WINDOW_HEIGHT - 100, 15, WHITE);
		DrawText(TextFormat("HiScore: %i", hiscore), 0, WINDOW_HEIGHT - 100, 15, WHITE);
		DrawRectangleRec(box.rec, box.color);
		for (int i = 0;i < score;i++) {
			DrawRectangleRec(snake_cords[i].rec, box.color);
		}
		DrawCircle((int)circle.center.x, (int)circle.center.y,
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
