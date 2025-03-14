#pragma once

/// Enum used to control the way arcs are drown
enum struct ArcMode {
	OPEN_PIE,
	OPEN_CHORD
};

/// Enum used to control the quality of arcs and Bezier curves
enum struct ArcQuality {
	VERY_LOW   = 100,
	LOW        =  75,
	MEDIUM     =  45,
	HIGH       =  33,
	VERY_HIGH  =  25,
};