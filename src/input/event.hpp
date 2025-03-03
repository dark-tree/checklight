#pragma once

#include "external.hpp"

class InputEvent;

template <class T>
concept InputEventDerived = std::derived_from<T, InputEvent>;

class InputEvent {

	public:

		enum Type {
			SCROLL,   ///< ScrollEvent
			MOUSE,    ///< MouseEvent
			KEYBOARD, ///< KeyboardEvent
			BUTTON,   ///< ButtonEvent
			UNICODE,  ///< UnicodeEvent
			CLOSE,    ///< CloseEvent
			RESIZE,   ///< ResizeEvent
			FRAME,    ///< FrameEvent
		};

		/**
		 * Get the specific event type, this matches one of the
		 * input event super classes.
		 */
		virtual Type getType() const = 0;

		/**
		 * For use in if-with-resources statements, safely and easily assets the event
		 * type. Returns a pointer of the requested type or nullptr (A falsely value).
		 */
		template <InputEventDerived T>
		const T* as() const {
			if (getType() == T::type) {
				return static_cast<const T*>(this);
			}

			return nullptr;
		}

	public:

		InputEvent() = default;

};
