#pragma once

#include "game/scene/gameplay.h"
#include "game/scene/main_menu.h"

#include <variant>
#include <vector>

#include <memory>

struct Game;

enum class SceneID {
	MainMenu,
	Gameplay,
};

using SceneState = std::variant<MainMenuScene, GameplayScene>;

class Scene {
public:
	Scene(SceneID id);
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;

private:
	SceneID m_id;
	SceneState m_state;
};

class SceneManager {
public:
	SceneManager(SceneID start_scene_id);
	~SceneManager();

	void push_scene(Game* game, SceneID scene_id);
	void pop_scene(Game* game);

	void update_current_scene(Game* game);
	void render_current_scene(const Game& game) const;

private:
	struct Impl;
	std::unique_ptr<Impl> m_impl;
};
