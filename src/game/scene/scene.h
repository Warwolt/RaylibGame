#pragma once

#include <vector>

struct Game;

enum class SceneID {
	MainMenu,
	Gameplay,
};

class Scene;

class SceneManager {
public:
	SceneManager(SceneID start_scene_id);
	~SceneManager();

	void push_scene(Game* game, SceneID scene_id);
	void pop_scene(Game* game);

	void update_current_scene(Game* game);
	void render_current_scene(const Game& game) const;

private:
	std::vector<Scene> m_scenes;
};
