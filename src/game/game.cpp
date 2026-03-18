#include "game/game.h"

#include "core/debug/logging.h"
#include "platform/win32.h"

#include <raylib.h>
#include <raymath.h>

// low resolution 16:9
constexpr int SCREEN_WIDTH = 768;
constexpr int SCREEN_HEIGHT = 432;
constexpr char SCREEN_TITLE[] = "Program";

Game* Game_initialize(int argc, char** argv) {
	/* Parse command line */
	bool start_fullscreen = false;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--fullscreen") == 0) {
			start_fullscreen = true;
		} else {
			LOG_ERROR("Unrecognized command: %s", argv[i]);
		}
	}

	/* Initialize Raylib */
	Raylib_SetTraceLogLevel(LOG_WARNING); // disable verbose raylib output
	Raylib_InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);
	Raylib_SetExitKey(KEY_NULL);
	Raylib_SetTargetFPS(120);

	/* Initialize game */
	Game* game = new Game {
		.scene = Scene(SceneID::MainMenu),
		.window = Window::initialize(SCREEN_WIDTH, SCREEN_HEIGHT),
	};
	if (start_fullscreen) {
		game->window.toggle_fullscreen();
	}

	LOG_INFO("Game initialized");

	return game;
}

void Game_update(Game* game) {
	/* Check input */
	game->should_quit = Raylib_WindowShouldClose();

	if (Raylib_IsKeyPressed(KEY_F11)) {
		game->window.toggle_fullscreen();
	}

	/* Update systems */
	game->window.update();

	/* Update scene */
	game->scene.update(game);
}

void Game_render(const Game& game) {
	/* Draw game onto viewport */
	Raylib_BeginTextureMode(game.window.viewport());
	{
		Raylib_ClearBackground(Color { 0, 0, 0, 255 });
		game.scene.render(game);
	}
	Raylib_EndTextureMode();

	/* Draw viewport onto application window */
	Raylib_BeginDrawing();
	{
		Raylib_ClearBackground(Color { 0, 0, 0, 255 });
		RenderTexture viewport = game.window.viewport();
		Rectangle viewport_rect = { .width = (float)viewport.texture.width, .height = (float)-viewport.texture.height };
		Rectangle letterbox = game.window.letterbox();
		DrawTexturePro(viewport.texture, viewport_rect, letterbox, Vector2Zero(), 0, WHITE);
	}
	Raylib_EndDrawing();
}

void Game_shutdown(Game* game) {
	game->window.deinitialize();
	Raylib_CloseWindow();
	LOG_INFO("Game shutdown");
	delete game;
}
