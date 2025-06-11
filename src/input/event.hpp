#pragma once

#include "external.hpp"

class InputEvent;

template <class T>
concept InputEventDerived = std::derived_from<T, InputEvent>;

/**
 * Base class of all input events
 */
class InputEvent {

	public:

		/**
		 * For use in if-with-resources statements, safely and easily assets the event
		 * type. Returns a pointer of the requested type or nullptr (A falsely value).
		 */
		template <InputEventDerived T>
		const T* as() const {
			return dynamic_cast<const T*>(this);
		}

	public:

		InputEvent() = default;
		virtual ~InputEvent() {};

};
