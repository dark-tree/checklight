#pragma once

/**
 * InputResult informs the InputDispatcher
 * on the state of the InputEvent handling
 */
enum struct InputResult {

	/**
	 * Continue dispatching the events to handlers of lower priority,
	 * both in this and parent dispatchers.
	 *
	 * @note
	 * Should be used when the listener ignores the event.
	 */
	PASS,

	/**
	 * Don't invoke event handlers
	 * in the calling dispatcher's siblings, but continue in the calling dispatcher.
	 *
	 * @note
	 * Should be used when there is no interest in handling the event in the local context.
	 */
	YIELD,

	/**
	 * Continue dispatching the events to handlers of lower priority,
	 * in calling dispatcher, but stop further processing in siblings of the calling dispatcher.
	 *
	 * @note
	 * Should be used if the event was acted upon but may still need processing in the context of the calling dispatcher.
	 */
	ACCEPT,

	/**
	 * Don't invoke event handlers
	 * both in the calling dispatcher and its siblings.
	 *
	 * @note
	 * Should be used if the event was acted upon.
	 */
	BLOCK,

};