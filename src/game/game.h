#pragma once

#include "platform/window.h"

#include <variant>

struct Game;

class MainMenuScene {
public:
	void update(Game* game);
	void render(const Game& game) const;
};

class GameplayScene {
public:
	void update(Game* game);
	void render(const Game& game) const;
};

enum class SceneID {
	MainMenu,
	Gameplay,
};

using SceneState = std::variant<MainMenuScene, GameplayScene>;

struct Scene {
	Scene(SceneID id);
	void update(Game* game);
	void render(const Game& game) const;

	// private:
	SceneID id;
	SceneState state;
};

struct Game {
	bool should_quit = false;
	Scene scene;
	Window window;
};

extern "C" __declspec(dllexport) Game* Game_initialize(int argc, char** argv);
extern "C" __declspec(dllexport) void Game_update(Game* game);
extern "C" __declspec(dllexport) void Game_render(const Game& game);
extern "C" __declspec(dllexport) void Game_shutdown(Game* game);
