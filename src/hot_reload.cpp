#include "hot_reload.h"

#include "game/game.h"
#include "core/debug/assert.h"
#include "core/debug/logging.h"
#include "library.h"
#include "platform/win32.h"

static std::optional<HotReloadState> g_rebuild_result;

static std::expected<GameLibrary, std::string> load_game_library(const std::string& library_path) {
	HMODULE handle = LoadLibraryA(library_path.c_str());
	if (!handle) {
		DWORD error_code = GetLastError();
		char error_detail[256] = { 0 };
		FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, error_code, 0, error_detail, sizeof(error_detail), nullptr);
		char message[512] = { 0 };
		snprintf(message, sizeof(message), "Couldn't load %s\nError code: %lu\nReason: %s", library_path.c_str(), error_code, error_detail);
		return std::unexpected(message);
	}

	return GameLibrary {
		.handle = handle,
		.initialize = (decltype(&Game_initialize))GetProcAddress(handle, "Game_initialize"),
		.shutdown = (decltype(&Game_shutdown))GetProcAddress(handle, "Game_shutdown"),
		.update = (decltype(&Game_update))GetProcAddress(handle, "Game_update"),
		.render = (decltype(&Game_render))GetProcAddress(handle, "Game_render"),
	};
}

std::string HotReloading::library_path() const {
	return this->executable_directory + "\\" + this->library_name;
}

std::string HotReloading::library_copy_path() const {
	return this->executable_directory + "\\" + this->library_copy_name;
}

std::expected<GameLibrary, std::string> HotReloading::load_library() const {
	/* Check library exists */
	if (!Win32_file_exists(library_path())) {
		ABORT("Can't find %s", this->library_name.c_str());
	}

	/* Copy library */
	Win32_copy_file(library_path(), library_copy_path());

	/* Load copy */
	return load_game_library(this->library_copy_name);
}

void HotReloading::update(Game* game_state, GameLibrary* game_library) {
	switch (game_state->hot_reload_state.value()) {
		case HotReloadState::Idle:
		case HotReloadState::Failed: {
			/* Trigger rebuild */
			if (Raylib_IsKeyPressed(KEY_F5)) {
				LOG_INFO("Rebuilding game library");
				game_state->hot_reload_state = HotReloadState::Rebuilding;
				auto on_build_command_done = [](int exit_code) {
					if (exit_code == 0) {
						g_rebuild_result = HotReloadState::ReadyToReload;
					} else {
						g_rebuild_result = HotReloadState::Failed;
						LOG_ERROR("Build finished with errors!");
					}
				};
				Win32_run_command("cmake --build build --target Library", on_build_command_done);
			}

		} break;

		case HotReloadState::Rebuilding:
			/* Wait for rebuild to finish */
			if (g_rebuild_result) {
				game_state->hot_reload_state = g_rebuild_result.value();
				g_rebuild_result = {};
			}
			break;

		case HotReloadState::ReadyToReload: {
			game_state->hot_reload_state = HotReloadState::Idle;

			/* Reload copied library */
			FreeLibrary(game_library->handle);
			*game_library = this->load_library().value();
			LOG_INFO("Game library reloaded");
		} break;
	}
}
