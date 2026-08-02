#pragma once

#include <memory>
#include <variant>
#include <vector>

struct Game;
class Scene;
enum class SceneID;

class SceneManager {
public:
	SceneManager();
	~SceneManager();

	void queue_push_scene(SceneID scene_id);
	void queue_pop_scene();
	void run_queued_actions(Game* game);

	void update_current_scene(Game* game);
	void render_current_scene(const Game& game) const;

private:
	struct PushScene {
		SceneID scene_id;
	};
	struct PopScene {};
	using SceneAction = std::variant<PushScene, PopScene>;

	// heap allocate scenes so they remain stable on vector re-allocation
	std::vector<std::unique_ptr<Scene>> m_scenes;
	std::vector<SceneAction> m_actions;

	void _push_scene(Game* game, SceneID scene_id);
	void _pop_scene(Game* game);
};
