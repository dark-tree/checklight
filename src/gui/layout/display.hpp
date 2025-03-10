#pragma once

/**
 * Controls how the component takes free space
 * in simple panel layout.
 */
enum struct Display {
	AUTO,       ///< Select space allocation based on vertical and horizontal alignment
	HORIZONTAL, ///< Occupy a horizontal block (strip)
	VERTICAL,   ///< Occupy a vertical block (strip)
	GREEDY,     ///< Occupy given bounds
};