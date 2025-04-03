#pragma once
#include "external.hpp"


/// Function to finding a shared pointer object (spt) in vector (vec)
	/// @param vec Vector where we want to find
	/// @param spt Shared pointer object to finded
template <typename T>
auto findInVector(std::vector< std::weak_ptr<T>>& vec, std::shared_ptr<T> spt) {
	return std::find_if(vec.begin(), vec.end(),
		[spt](const std::weak_ptr<T>& wp) {
			return !wp.expired() && wp.lock() == spt;
		});
}


/// Function to finding a weak pointer object (spt) in vector (vec)
/// @param vec Vector where we want to find
/// @param wptr Weak pointer object to finded
template <typename T>
auto findInVector(std::vector< std::weak_ptr<T>>& vec, std::weak_ptr<T> wptr) {
	return std::find_if(vec.begin(), vec.end(),
		[wptr](const std::weak_ptr<T>& wp) {
			return !wp.expired() && wp.lock() == wptr.lock();
		});
}


/// Remove every expired weak pointers in vector
/// @param vec Vector where we want to remove every expired pointers
template <typename T>
void removeExpired(std::vector<std::weak_ptr<T>>& vec) {
	vec.erase(std::remove_if(vec.begin(), vec.end(),
		[](const std::weak_ptr<T>& wp) {
			return wp.expired();
		}), vec.end());
}