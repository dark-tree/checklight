#pragma once

#include "external.hpp"
#include "listener.hpp"

class InputDispatcher : public InputListener {

	private:

		std::list<std::shared_ptr<InputListener>> listeners;

	public:

		InputResult onEvent(const InputEvent &event) override;

	public:

		/**
		 * Adds a new listener to the dispatcher, listeners added later will only get
		 * triggered if all previously added listeners PASS the event
		 *
		 * @param listener The input listener to register
		 */
		void registerListener(const std::shared_ptr<InputListener>& listener);

		/**
		 * Removes the specified listener, after calling this method
		 * not further events will be dispatched to the listener and reference will be freed
		 *
		 * @param listener The input listener to remove
		 */
		void removeListener(const std::shared_ptr<InputListener>& listener);

};
