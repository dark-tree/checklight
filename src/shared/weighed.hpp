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

		using Set = std::multiset<Node, Order>;
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

		/// Get the lowest priority used in the set
		int lowest() const {
			if (set.empty()) {
				throw std::length_error {"There is no lowest element in empty set!"};
			}

			return std::min(set.begin()->order, set.rbegin()->order);
		}

		/// Get the highest priority used in the set
		int highest() const {
			if (set.empty()) {
				throw std::length_error {"There is no highest element in empty set!"};
			}

			return std::max(set.begin()->order, set.rbegin()->order);
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
