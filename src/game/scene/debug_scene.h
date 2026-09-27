struct Game;

#include "game/physics.h"

#include <raylib.h>

#include <optional>
#include <vector>

class DebugScene {
public:
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;

private:
	std::vector<Rectangle> m_rectangles;
};
