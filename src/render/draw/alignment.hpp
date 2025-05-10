#pragma once

/// Enum used to control the up-down alignment of elements
enum struct VerticalAlignment {
	TOP    = 0,
	CENTER = 1,
	BOTTOM = 2
};

/// Enum used to control the left-right alignment of elements
enum struct HorizontalAlignment {
	LEFT   = 0,
	CENTER = 1,
	RIGHT  = 2
};

template <typename T>
concept Alignment = std::is_same_v<VerticalAlignment, T> || std::is_same_v<HorizontalAlignment, T>;

/// Convert alignment enums into a float factor
template <Alignment T>
constexpr float toAlignmentFactor(T alignment) {
	return static_cast<int>(alignment) * 0.5f;
}