#pragma once

#include "external.hpp"
#include "listener.hpp"

class InputDispatcher : public InputListener {

	private:

		std::list<std::shared_ptr<InputListener>> listeners;

	public:

		/**
		 * Passes the event from the first to the last registered listener,
		 * as long as the event is not blocked, or consumed.
		 *
		 * @param event The event to process
		 */
		InputResult onEvent(const InputEvent &event) override;

	public:

		/**
		 * Adds a new listener to the dispatcher, listeners added earlier will only get
		 * triggered if all listeners added later PASS the event
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
