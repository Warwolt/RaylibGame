#pragma once

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
