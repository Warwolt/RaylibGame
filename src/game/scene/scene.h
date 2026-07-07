#pragma once

#include <memory>
#include <vector>

struct Game;
class Scene;
enum class SceneID;

class SceneManager {
public:
	SceneManager();
	~SceneManager();

	void push_scene(Game* game, SceneID scene_id);
	void pop_scene(Game* game);

	void update_current_scene(Game* game);
	void render_current_scene(const Game& game) const;

private:
	// heap allocate scenes so they remain stable on vector re-allocation
	std::vector<std::unique_ptr<Scene>> m_scenes;
};
