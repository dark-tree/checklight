#pragma once
#include "external.hpp"

/// Function to finding a shared pointer object (spt) in vector (vec)
/// 
/// @param vec Vector where we want to find
/// @param spt Shared pointer object to finded
template <typename T>
auto findInVector(std::vector< std::weak_ptr<T>>& vec,const std::shared_ptr<T>& spt) {
	return std::find_if(vec.begin(), vec.end(),
		[&spt](const std::weak_ptr<T>& wp) {
			return wp.lock() == spt;
		});
}

/// Function to finding a weak pointer object (spt) in vector (vec)
/// 
/// @param vec Vector where we want to find
/// @param wptr Weak pointer object to finded
template <typename T>
auto findInVector(std::vector< std::weak_ptr<T>>& vec,const std::weak_ptr<T>& wptr) {
	return std::find_if(vec.begin(), vec.end(),
		[sptr = wptr.lock()](const std::weak_ptr<T>& wp) {
			return wp.lock() == sptr;
		});
}

/// Function to finding a shared pointer object (spt) in shared ptr vector (vec)
/// 
/// @param vec Vector where we want to find
/// @param spt Shared pointer object to finded
template <typename T>
auto findInVector(std::vector< std::shared_ptr<T>>& vec,const std::shared_ptr<T>& spt) {
	return std::find_if(vec.begin(), vec.end(),
		[&spt](const std::shared_ptr<T>& sp) {
			return sp == spt;
		});
}

/// Remove every expired weak pointers in vector
/// 
/// @param vec Vector where we want to remove every expired pointers
template <typename T>
void removeExpired(std::vector<std::weak_ptr<T>>& vec) {
	vec.erase(std::remove_if(vec.begin(), vec.end(),
		[](const std::weak_ptr<T>& wp) {
			return wp.expired();
		}), vec.end());
}

/// Remove every expired shared pointers in vector
/// 
/// @param vec Vector where we want to remove every expired pointers
template <typename T>
void removeExpired(std::vector<std::shared_ptr<T>>& vec) {
	vec.erase(std::remove_if(vec.begin(), vec.end(),
		[](const std::shared_ptr<T>& wp) {
			return wp==nullptr;
		}), vec.end());
}

template <typename T>
void removeFromVector(std::vector<std::shared_ptr<T>>& vec,const std::shared_ptr<T>& spt) {
	vec.erase(std::remove_if(vec.begin(), vec.end(),
		[&spt](const std::shared_ptr<T>& sp) {
			return sp == spt;
		}), vec.end());
}