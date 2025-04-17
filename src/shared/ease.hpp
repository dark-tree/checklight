#pragma once

#include "external.hpp"

namespace ease {

	/**
	 * An ease-in-out function, expects time to be in range [0, 1]
	 *
	 * @param time Linear time value in range [0, 1]
	 * @return Eased value in range [0, 1]
	 */
	float ofInOut(float time);

	/**
	 * An linear function, expects time to be in range [0, 1]
	 *
	 * @param time Linear time value in range [0, 1]
	 * @return Eased value in range [0, 1]
	 */
	float ofLinear(float time);

};
