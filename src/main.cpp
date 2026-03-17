// include these as cpp files since they're not part of the public DLL interface
#include "core/debug/logging.cpp"
#include "platform/win32.cpp"

#include "game/game.h"
#include "platform/lean_mean_windows.h"

#include <format>
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
		char message[256] = { 0 };
		snprintf(message, 256, "Couldn't load %s\n", library_path.c_str());
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

static void on_build_command_done(int exit_code) {
	if (exit_code != 0) {
		LOG_ERROR("Build finished with errors!");
	}
}

int main(int argc, char** argv) {
	initialize_logging();
	Win32_set_process_dpi_aware();

	/* Get executable directory */
	std::string executable_directory = Win32_get_executable_directory();

	/* Get library path */
	std::string library_name = "GameLib.dll";
	std::string library_path = executable_directory + "\\" + library_name;

	/* Check library exists */
	if (!Win32_file_exists(library_path)) {
		std::string message = std::format("Can't find {}", library_name);
		Win32_show_error_message_box(message);
		exit(1);
	}

	/* Copy library */
	std::string library_copy_name = "GameLib-hot-reload.dll";
	std::string library_copy_path = executable_directory + "\\" + library_copy_name;
	Win32_copy_file(library_path, library_copy_path);

	/* Load copy */
	uint64_t library_last_modified = Win32_get_file_last_modified(library_path);
	GameLibrary game_library = load_library(library_copy_name);

	/* State */
	Game game_state = {};
	game_library.initialize(&game_state, argc, argv);

	/* Run program */
	while (!game_state.should_quit) {
		/* Check hot reload */
		{
			/* Trigger library build */
			if (Raylib_IsKeyPressed(KEY_F5)) {
				LOG_INFO("Rebuilding game library");
				Win32_run_command("cmake --build build --target Library", on_build_command_done);
			}

			/* Reload library when modified */
			uint64_t current_last_modified = Win32_get_file_last_modified(library_path);
			if (current_last_modified != library_last_modified) {
				/* Unload library */
				FreeLibrary(game_library.handle);

				/* Copy rebuilt library */
				Win32_copy_file(library_path, library_copy_path);

				/* Reload copied library */
				game_library = load_library(library_copy_name);
				library_last_modified = Win32_get_file_last_modified(library_path);
			}
		}

		/* Run game */
		game_library.update(&game_state);
		game_library.render(game_state);
	}

	/* Shutdown */
	game_library.shutdown(&game_state);
	FreeLibrary(game_library.handle);
	return 0;
}
