struct Game;

#include <raylib.h>

#include <vector>

struct LineSegment {
	Vector2 start;
	Vector2 end;
};

struct VertexState {
	bool is_hovered = false;
	bool is_grabbed = false;
};

class DebugScene {
public:
	void initialize(Game* game);
	void deinitialize(Game* game);

	void update(Game* game);
	void render(const Game& game) const;

private:
	Vector2& _line_segment_vertex(int index);
	const Vector2& _line_segment_vertex(int index) const;

	std::vector<LineSegment> m_line_segments;
	std::vector<VertexState> m_vertex_states;
};
