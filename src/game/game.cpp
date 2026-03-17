#include "game/game.h"

#include "core/debug/logging.h"
#include "platform/win32.h"

#include <raylib.h>

// low resolution 16:9
constexpr int SCREEN_WIDTH = 768;
constexpr int SCREEN_HEIGHT = 432;
constexpr char SCREEN_TITLE[] = "Program";

void Game_initialize(Game* /*game*/, int argc, char** argv) {
	/* Initialize systems */
	Raylib_SetTraceLogLevel(LOG_WARNING); // disable verbose raylib output
	Raylib_InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);
	Raylib_SetExitKey(KEY_NULL);
	Raylib_SetTargetFPS(120);

	/* Parse command line */
	bool start_fullscreen = false;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--fullscreen") == 0) {
			start_fullscreen = true;
		} else {
			LOG_ERROR("Unrecognized command: %s", argv[i]);
		}
	}

	LOG_INFO("Game initialized");
}

void Game_update(Game* game) {
	game->should_quit = Raylib_WindowShouldClose();
	if (Raylib_IsKeyPressed(KEY_ESCAPE)) {
		game->should_quit = true;
	}
}

void Game_render(const Game& /*game*/) {
	Raylib_BeginDrawing();
	{
		Raylib_ClearBackground(Color { 0, 127, 127, 255 });
	}
	Raylib_EndDrawing();
}

void Game_shutdown(Game* /*game*/) {
	Raylib_CloseWindow();
	LOG_INFO("Game shutdown");
}
