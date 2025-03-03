#pragma once

#include "external.hpp"
#include "listener.hpp"
#include "shared/weighed.hpp"

class InputDispatcher : public InputListener {

	private:

		WeighedSet<std::shared_ptr<InputListener>, std::greater<>> listeners;

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
		 * Adds a new listener to the dispatcher, listeners with lower priority will only get
		 * triggered if all listeners with higher priority PASS or YIELD the event
		 *
		 * @param listener The input listener to register
		 * @param priority The priority of the new listener
		 */
		void registerListener(const std::shared_ptr<InputListener>& listener, int priority = 0);

		/**
		 * Adds a new listener to the dispatcher with priority lower than the priority of
		 * all currently added listeners (or 0 if there are no listeners)
		 *
		 * @param listener The input listener to register
		 */
		void registerLowerListener(const std::shared_ptr<InputListener>& listener);

		/**
		 * Adds a new listener to the dispatcher with priority hgher than the priority of
		 * all currently added listeners (or 0 if there are no listeners)
		 *
		 * @param listener The input listener to register
		 */
		void registerHigherListener(const std::shared_ptr<InputListener>& listener);

		/**
		 * Removes the specified listener, after calling this method
		 * not further events will be dispatched to the listener and reference will be freed
		 *
		 * @param listener The input listener to remove
		 */
		void removeListener(const std::shared_ptr<InputListener>& listener);

};
