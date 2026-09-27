#include "game/physics.h"

#include <raymath.h>

static Vector2 Vector2Orthogonal(Vector2 v) {
	return Vector2 { -v.y, v.x };
}

std::optional<Vector2> segment_segment_intersection(LineSegment a, LineSegment b) {
	const Vector2 delta_a = a.end - a.start;
	const Vector2 delta_b = b.end - b.start;
	const Vector2 delta_a_orthogonal = Vector2Orthogonal(delta_a);
	const Vector2 delta_b_orthogonal = Vector2Orthogonal(delta_b);
	const Vector2 delta_ab = b.start - a.start;
	const Vector2 delta_ba = a.start - b.start;

	// Check if segments are parallel
	if (Vector2DotProduct(delta_a, delta_b_orthogonal) == 0) {
		return {};
	}

	// Compute segment parameter values t and u
	const float t = Vector2DotProduct(delta_ab, delta_b_orthogonal) / Vector2DotProduct(delta_a, delta_b_orthogonal);
	const float u = Vector2DotProduct(delta_ba, delta_a_orthogonal) / Vector2DotProduct(delta_b, delta_a_orthogonal);

	// Segments intersect if parameters t and u are both in range [0, 1]
	const bool t_in_range = 0.0f <= t && t <= 1.0f;
	const bool u_in_range = 0.0f <= u && u <= 1.0f;
	if (!t_in_range || !u_in_range) {
		return {};
	}

	// Return point of intersection
	return a.start + t * delta_a;
}
