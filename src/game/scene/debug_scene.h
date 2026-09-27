struct Game;

#include "game/physics.h"

#include <raylib.h>

#include <optional>
#include <vector>

struct InteractionState {
	bool is_hovered;
	bool is_active;
};

class DebugScene {
public:
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;

private:
	std::vector<Rectangle> m_rectangles;
	std::vector<InteractionState> m_rectangle_states;
	std::optional<Vector2> m_delta_start;
	std::optional<size_t> m_selected_rectangle_index;
};
