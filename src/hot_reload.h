#pragma once

#include "library.h"

#include <expected>
#include <string>

enum class HotReloadState {
	Idle,
	Rebuilding,
	ReadyToReload,
	Failed,
};

struct HotReloading {
	std::string executable_directory;
	std::string library_name;
	std::string library_copy_name;

	std::string library_path() const;
	std::string library_copy_path() const;
	std::expected<GameLibrary, std::string> load_library() const;
	void update(Game* game_state, GameLibrary* game_library);
};
