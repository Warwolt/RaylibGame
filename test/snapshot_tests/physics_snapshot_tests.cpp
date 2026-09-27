#include "game/physics.h"

#include "test/snapshot_tests/snapshots.h"

#include <gtest/gtest.h>
#include <raylib.h>

constexpr int SCREEN_WIDTH = 250;
constexpr int SCREEN_HEIGHT = 250;
constexpr Vector2 SCREEN_SIZE = { SCREEN_WIDTH, SCREEN_HEIGHT };

class PhysicsSnapshotTests : public ::testing::Test {
public:
	static void SetUpTestSuite() {
		Raylib_SetTraceLogLevel(LOG_WARNING);
		Raylib_SetConfigFlags(FLAG_WINDOW_HIDDEN);
		Raylib_InitWindow(1, 1, "Snapshot Test");
	}

	static void TearDownTestSuite() {
		Raylib_CloseWindow();
	}
};

Image static render_segment_intersections(std::vector<LineSegment> segments, std::vector<Vector2> intersections) {
	return snapshots::render_image(SCREEN_SIZE, [&]() {
		// Draw segments
		for (LineSegment segment : segments) {
			Raylib_DrawLineV(segment.start, segment.end, GREEN);
		}

		// Draw intersections
		for (Vector2 intersection : intersections) {
			Raylib_DrawCircleV(intersection, 3.0f, RED);
		}
	});
}

template <typename T>
static std::vector<T> to_vector(const std::optional<T>& value) {
	return value.has_value() ? std::vector<T> { *value } : std::vector<T> {};
}

TEST_F(PhysicsSnapshotTests, Segment_Segment_Intersection_TwoDiagonalLines) {
	const LineSegment segment_a = { .start = { 0, 0 }, .end = { 100, 100 } };
	const LineSegment segment_b = { .start = { 100, 0 }, .end = { 0, 100 } };

	const std::optional<Vector2> intersection = segment_segment_intersection(segment_a, segment_b);
	const Image image = render_segment_intersections({ segment_a, segment_b }, to_vector(intersection));

	ASSERT_TRUE(intersection.has_value());
	EXPECT_EQ(intersection->x, 50);
	EXPECT_EQ(intersection->y, 50);
	EXPECT_SNAPSHOT_EQ(image);
}
