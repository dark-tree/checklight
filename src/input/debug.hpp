#pragma once

#include "listener.hpp"

class DebugInputListener : public InputListener {

	private:

		std::string modifiersToString(int modifiers) const;
		std::string actionToString(int action) const;

	public:

		InputResult onEvent(const InputEvent &event) override;

};
