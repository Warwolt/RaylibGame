#include "game/physics.h"

#include "test/snapshot_tests/snapshots.h"

#include <gtest/gtest.h>
#include <raylib.h>
#include <raymath.h>

constexpr int SCREEN_WIDTH = 100;
constexpr int SCREEN_HEIGHT = 100;
constexpr Vector2 SCREEN_SIZE = { SCREEN_WIDTH, SCREEN_HEIGHT };

template <typename T>
static std::vector<T> to_vector(const std::optional<T>& value) {
	return value.has_value() ? std::vector<T> { *value } : std::vector<T> {};
}

template <typename T>
static std::vector<T> operator+(const std::vector<T>& lhs, const std::vector<T>& rhs) {
	std::vector<T> result;
	result.reserve(lhs.size() + rhs.size());
	result.insert(result.end(), lhs.begin(), lhs.end());
	result.insert(result.end(), rhs.begin(), rhs.end());
	return result;
}

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

TEST_F(PhysicsSnapshotTests, Segment_Segment_Intersection_CrossingLines) {
	const LineSegment segment_a = { .start = { 0, 0 }, .end = { 100, 100 } };
	const LineSegment segment_b = { .start = { 100, 0 }, .end = { 0, 100 } };

	const std::optional<Vector2> intersection = segment_segment_intersection(segment_a, segment_b);
	const Image image = render_segment_intersections({ segment_a, segment_b }, to_vector(intersection));

	EXPECT_EQ(intersection, Vector2(50, 50));
	EXPECT_SNAPSHOT_EQ(image);
}

TEST_F(PhysicsSnapshotTests, Segment_Segment_Intersection_Triangle) {
	const LineSegment segment_a = { .start = { 0, 0 }, .end = { 100, 100 } };
	const LineSegment segment_b = { .start = { 100, 0 }, .end = { 0, 100 } };
	const LineSegment segment_c = { .start = { 0, 25 }, .end = { 100, 25 } };

	const std::optional<Vector2> intersection1 = segment_segment_intersection(segment_a, segment_b);
	const std::optional<Vector2> intersection2 = segment_segment_intersection(segment_a, segment_c);
	const std::optional<Vector2> intersection3 = segment_segment_intersection(segment_b, segment_c);
	const std::vector<Vector2> intersections = to_vector(intersection1) + to_vector(intersection2) + to_vector(intersection3);
	const Image image = render_segment_intersections({ segment_a, segment_b, segment_c }, intersections);

	EXPECT_EQ(intersection1, Vector2(50, 50));
	EXPECT_EQ(intersection2, Vector2(25, 25));
	EXPECT_EQ(intersection3, Vector2(75, 25));
	EXPECT_SNAPSHOT_EQ(image);
}
