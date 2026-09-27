#pragma once

#include <raylib.h>

#include <optional>

struct LineSegment {
	Vector2 start;
	Vector2 end;
};

std::optional<Vector2> segment_segment_intersection(LineSegment a, LineSegment b);
