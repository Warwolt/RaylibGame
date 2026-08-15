#include "game/game.h"

#include "core/debug/logging.h"
#include "core/debug/profiling.h"
#include "game/scene/scene_id.h"
#include "game_api.h"


#include <raylib.h>
#include <raymath.h>

// low resolution 16:9
constexpr int SCREEN_WIDTH = 256;
constexpr int SCREEN_HEIGHT = 144;
constexpr char SCREEN_TITLE[] = "Video Game";

Game* Game_initialize(int argc, char** argv) {
	/* Parse command line */
	bool start_fullscreen = true;
	for (int i = 1; i < argc; i++) {
		if (strcmp(argv[i], "--windowed") == 0) {
			start_fullscreen = false;
		} else {
			LOG_ERROR("Unrecognized command: %s", argv[i]);
		}
	}

	/* Initialize Raylib */
	Raylib_SetTraceLogLevel(LOG_WARNING); // disable verbose raylib output
	Raylib_InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, SCREEN_TITLE);
	Raylib_SetExitKey(KEY_NULL);
	Raylib_SetTargetFPS(144);
	Raylib_InitAudioDevice();

	/* Initialize game */
	Game* game = new Game {
		.window = Window::initialize(SCREEN_WIDTH, SCREEN_HEIGHT),
	};
	game->scenes.queue_push_scene(SceneID::MainMenu);
	game->scenes.run_queued_actions(game);
	if (start_fullscreen) {
		game->window.toggle_fullscreen();
	}
	game->resources.load_default_font("resource/font/8bitoperator_jve.ttf");

	/* Initialize systems */
	game->debug_overlay.initialize();

	LOG_INFO("Game initialized");
	return game;
}

void Game_update(Game* game) {
	PROFILING_SCOPE();

	/* Check input */
	game->should_quit = Raylib_WindowShouldClose();
	read_input(&game->input, game->window);

	if (game->input.key_pressed(KEY_F11)) {
		game->window.toggle_fullscreen();
	}

	/* Handle cursor visiblity */
	if (game->input.last_input_was_gamepad() || game->input.last_input_was_keyboard()) {
		Raylib_HideCursor();
	} else {
		Raylib_ShowCursor();
	}

	/* Update systems */
	game->window.update();

	/* Update scene */
	game->scenes.update_current_scene(game);
	game->scenes.run_queued_actions(game);
}

void Game_render(const Game& game) {
	/* Draw game onto viewport */
	Raylib_BeginTextureMode(game.window.viewport());
	{
		PROFILING_SCOPE(); // anything worth measuring is in the scene
		Raylib_ClearBackground(Color { 0, 0, 0, 255 });
		game.scenes.render_current_scene(game);
		game.debug_overlay.render(game);
	}
	Raylib_EndTextureMode();

	/* Draw viewport onto application window */
	Raylib_BeginDrawing();
	{
		const RenderTexture viewport = game.window.viewport();
		const Rectangle viewport_rect = {
			.width = (float)viewport.texture.width,
			.height = (float)-viewport.texture.height,
		};
		const Rectangle letterbox = game.window.letterbox();
		Raylib_ClearBackground(Color { 0, 0, 0, 255 });
		Raylib_DrawTexturePro(viewport.texture, viewport_rect, letterbox, Vector2Zero(), 0, WHITE);
	}
	Raylib_EndDrawing();

	PROFILING_END_FRAME();
}

void Game_shutdown(Game* game) {
	game->window.deinitialize();
	Raylib_CloseAudioDevice();
	Raylib_CloseWindow();

	LOG_INFO("Game shutdown");
	delete game;
}
