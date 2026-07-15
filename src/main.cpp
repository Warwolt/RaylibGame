// include these as cpp files since they're not part of the public DLL interface
#include "core/debug/assert.h"
#include "platform/win32.h"

#include "core/debug/profiling.h"
#include "game/game.h"
#include "library.h"

#include <raylib.h>

#include <string>

int main(int argc, char** argv) {
	Win32_enable_crash_handler();
	Win32_set_process_dpi_aware();
	initialize_logging();

	/* Initialize Tracy */
	PROFILING_STARTUP_PROFILER();

	/* Load Game DLL */
	HotReloading hot_reloading = {
		.executable_directory = Win32_get_executable_directory(),
		.library_name = "GameLib.dll",
		.library_copy_name = "GameLib-hot-reload.dll",
	};
	GameLibrary game_library = {};
	std::expected<GameLibrary, std::string> result = hot_reloading.load_library();
	if (!result.has_value()) {
		Win32_show_error_message_box(result.error());
	}
	game_library = result.value();

	/* State */
	Game* game_state = game_library.initialize(argc, argv);

	/* Run program */
	while (!game_state->should_quit) {
		game_library.update(game_state);
		game_library.render(*game_state);
		hot_reloading.update(game_state, &game_library);
	}

	/* Shutdown */
	game_library.shutdown(game_state);
	FreeLibrary(game_library.handle);
	PROFILING_SHUTDOWN_PROFILER();
	return 0;
}
