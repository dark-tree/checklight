#pragma once

#include "external.hpp"

template <typename T, typename C = std::less<T>>
class WeighedSet {

	private:

		struct Node {
			int order;
			T value;
		};

		struct Order {
			bool operator() (const Node& lhs, const Node& rhs) const {
				return C{}(lhs.order, rhs.order);
			}
		};

		using Set = std::set<Node, Order>;
		Set set;

	public:

		/// Insert new element into set
		void insert(int weight, const T& value) {
			set.emplace(weight, value);
		}

		/// Remove element by value
		void remove(const T& value) {
			std::erase_if(set, [value] (const Node& node) {
				return node.value == value;
			});
		}

		/// Get the number of elements in this set
		size_t size() const {
			return set.size();
		}

		/// Check if the set is empty
		bool empty() const {
			return set.empty();
		}

		class Iterator {

			private:

				Set::iterator iterator;

			public:

				using value_type = T;
				using pointer = T*;
				using reference = T&;

				constexpr Iterator(const Set::iterator& iterator)
				: iterator(iterator) {}

				constexpr const T& operator*() const {
					return iterator->value;
				}

				constexpr Iterator& operator++() {
					iterator ++;
					return *this;
				}

				constexpr Iterator operator++(int) {
					Iterator tmp = *this;
					++(*this);
					return tmp;
				}

				constexpr bool operator==(const Iterator& other) const {
					return iterator == other.iterator;
				}

				constexpr bool operator!=(const Iterator& other) const {
					return iterator != other.iterator;
				}
		};

		/// Standard begin iterator
		constexpr Iterator begin() const {
			return {set.begin()};
		}

		/// Standard end iterator
		constexpr Iterator end() const {
			return {set.end()};
		}

};
