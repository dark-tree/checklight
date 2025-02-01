#pragma once

#include "external.hpp"

template<typename T>
class Pyramid {

	private:

		std::vector<std::vector<T>> pyramid;

	public:

		class View {

			private:

				const Pyramid& pyramid;
				int index;

				friend class Pyramid;

				View(const Pyramid& pyramid)
				: pyramid(pyramid), index(0) {}

			public:

				/**
				 * Move one layer up
				 */
				void up() {
					index ++;
				}

				/**
				 * Returns a set of all the elements in the current layer
				 * and in all layers above it
				 */
				std::set<T> collect() const {
					std::set<T> values;

					for (int i = index; i < pyramid.size(); i ++) {
						for (const T& value : pyramid.get(i)) values.insert(value);
					}

					return values;
				}

		};

		/**
		 * Move one layer up
		 */
		void push() {
			pyramid.emplace_back();
		}

		/**
		 * Check if the pyramid has no layers (is empty)
		 */
		bool empty() const {
			return pyramid.empty();
		}

		/**
		 * Get the number of layers in the pyramid
		 */
		int size() const {
			return pyramid.size();
		}

		/**
		 * Get a vector of elements at the specified layer
		 */
		const std::vector<T>& get(int index) const {
			return pyramid.at(index);
		}

		/**
		 * Get the top layer (the layer currently being appended to)
		 */
		const std::vector<T>& top() const {
			return pyramid.back();
		}

		/**
		 * Add a element to the top layer
		 */
		void append(const T& value) {
			pyramid.back().push_back(value);
		}

		/**
		 * Get a read-only iterator-like view of the pyramid
		 */
		View view() const {
			return {*this};
		}

};
