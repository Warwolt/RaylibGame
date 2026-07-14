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

static GameLibrary load_game_library(const std::string& library_path) {
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

static Game* g_game_state = nullptr;

// FIXME: move into `HotReloading::update`
static void on_build_command_done(int exit_code) {
	if (exit_code == 0) {
		g_game_state->debug.reload_state = HotReloadState::ReadyToReload;
	} else {
		g_game_state->debug.reload_state = HotReloadState::Failed;
		LOG_ERROR("Build finished with errors!");
	}
}

struct HotReloading {
	std::string executable_directory;
	std::string library_name;
	std::string library_copy_name;

	std::string library_path() const {
		return this->executable_directory + "\\" + this->library_name;
	}

	std::string library_copy_path() const {
		return this->executable_directory + "\\" + this->library_copy_name;
	}

	GameLibrary load_library() const {
		/* Check library exists */
		if (!Win32_file_exists(library_path())) {
			ABORT("Can't find %s", this->library_name.c_str());
		}

		/* Copy library */
		Win32_copy_file(library_path(), library_copy_path());

		/* Load copy */
		return load_game_library(this->library_copy_name);
	}

	void update(Game* game_state, GameLibrary* game_library) {
		switch (game_state->debug.reload_state) {
			case HotReloadState::Idle:
			case HotReloadState::Failed: {
				/* Trigger rebuild */
				if (Raylib_IsKeyPressed(KEY_F5)) {
					LOG_INFO("Rebuilding game library");
					game_state->debug.reload_state = HotReloadState::Rebuilding;
					Win32_run_command("cmake --build build --target Library", on_build_command_done);
				}

			} break;

			case HotReloadState::Rebuilding:
				// wait for `on_build_command_done` to be called
				break;

			case HotReloadState::ReadyToReload: {
				game_state->debug.reload_state = HotReloadState::Idle;

				/* Unload library */
				FreeLibrary(game_library->handle);

				/* Copy rebuilt library */
				Win32_copy_file(library_path(), library_copy_path());

				/* Reload copied library */
				*game_library = load_game_library(this->library_copy_name);
				LOG_INFO("Game library reloaded");
				PROFILING_LOG("Game library reloaded");
			} break;
		}
	}
};

int main(int argc, char** argv) {
	Win32_enable_crash_handler();
	Win32_set_process_dpi_aware();
	initialize_logging();

	/* Initialize Tracy */
	PROFILING_STARTUP_PROFILER();

	HotReloading hot_reloading = {
		.executable_directory = Win32_get_executable_directory(),
		.library_name = "GameLib.dll",
		.library_copy_name = "GameLib-hot-reload.dll",
	};

	/* Load copy */
	GameLibrary game_library = hot_reloading.load_library();

	/* State */
	g_game_state = game_library.initialize(argc, argv);

	/* Run program */
	while (!g_game_state->should_quit) {
		game_library.update(g_game_state);
		game_library.render(*g_game_state);
		hot_reloading.update(g_game_state, &game_library);
	}

	/* Shutdown */
	game_library.shutdown(g_game_state);
	FreeLibrary(game_library.handle);
	PROFILING_SHUTDOWN_PROFILER();
	return 0;
}
