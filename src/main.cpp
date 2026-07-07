// include these as cpp files since they're not part of the public DLL interface
#include "core/debug/assert.cpp"
#include "core/debug/logging.cpp"
#include "platform/win32.cpp"

#include "core/debug/profiling.h"
#include "game/game.h"
#include "platform/lean_mean_windows.h"

#include <string>

struct GameLibrary {
	HMODULE handle;
	decltype(&Game_initialize) initialize;
	decltype(&Game_shutdown) shutdown;
	decltype(&Game_update) update;
	decltype(&Game_render) render;
};

static GameLibrary load_library(const std::string& library_path) {
	HMODULE handle = LoadLibraryA(library_path.c_str());
	if (!handle) {
		DWORD error_code = GetLastError();
		char error_detail[256] = { 0 };
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error_code, 0, error_detail, sizeof(error_detail), nullptr);
		char message[512] = { 0 };
		snprintf(message, sizeof(message), "Couldn't load %s\nError code: %lu\nReason: %s", library_path.c_str(), error_code, error_detail);
		Win32_show_error_message_box(message);
		exit(1);
	}

	return GameLibrary {
		.handle = handle,
		.initialize = (decltype(&Game_initialize))GetProcAddress(handle, "Game_initialize"),
		.shutdown = (decltype(&Game_shutdown))GetProcAddress(handle, "Game_shutdown"),
		.update = (decltype(&Game_update))GetProcAddress(handle, "Game_update"),
		.render = (decltype(&Game_render))GetProcAddress(handle, "Game_render"),
	};
}

static bool g_library_has_been_rebuilt = false;
static void on_build_command_done(int exit_code) {
	g_library_has_been_rebuilt = exit_code == 0;
	if (exit_code != 0) {
		LOG_ERROR("Build finished with errors!");
	}
}

int main(int argc, char** argv) {
	Win32_enable_crash_handler();
	Win32_set_process_dpi_aware();
	initialize_logging();

	/* Initialize Tracy */
	PROFILING_STARTUP_PROFILER();

	/* Get executable directory */
	std::string executable_directory = Win32_get_executable_directory();

	/* Get library path */
	std::string library_name = "GameLib.dll";
	std::string library_path = executable_directory + "\\" + library_name;

	/* Check library exists */
	if (!Win32_file_exists(library_path)) {
		ABORT("Can't find %s", library_name.c_str());
	}

	/* Copy library */
	std::string library_copy_name = "GameLib-hot-reload.dll";
	std::string library_copy_path = executable_directory + "\\" + library_copy_name;
	Win32_copy_file(library_path, library_copy_path);

	/* Load copy */
	GameLibrary game_library = load_library(library_copy_name);

	/* State */
	Game* game_state = game_library.initialize(argc, argv);

	/* Run program */
	while (!game_state->should_quit) {
		/* Check hot reload */
		{
			/* Trigger library build */
			if (Raylib_IsKeyPressed(KEY_F5)) {
				LOG_INFO("Rebuilding game library");
				Win32_run_command("cmake --build build --target Library", on_build_command_done);
			}

			/* Reload library when modified */
			if (g_library_has_been_rebuilt) {
				g_library_has_been_rebuilt = false;

				/* Unload library */
				FreeLibrary(game_library.handle);

				/* Copy rebuilt library */
				Win32_copy_file(library_path, library_copy_path);

				/* Reload copied library */
				game_library = load_library(library_copy_name);
				LOG_INFO("Game library reloaded");
				PROFILING_LOG("Game library reloaded");
			}
		}

		/* Run game */
		game_library.update(game_state);
		game_library.render(*game_state);
	}

	/* Shutdown */
	game_library.shutdown(game_state);
	FreeLibrary(game_library.handle);
	PROFILING_SHUTDOWN_PROFILER();
	return 0;
}
