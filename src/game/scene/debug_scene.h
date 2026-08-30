struct Game;

#include <raylib.h>

#include <vector>

struct LineSegment {
	Vector2 start;
	Vector2 end;
};

class DebugScene {
public:
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;

private:
	LineSegment m_line_segment;
	bool m_start_hovered = false;
	bool m_end_hovered = false;
};
