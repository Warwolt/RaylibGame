#pragma once

#include "game/scene/main_menu.h"

#include <variant>

struct Game;

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
