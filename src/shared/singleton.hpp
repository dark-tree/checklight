#pragma once
#include <memory>

template <typename T>
class Singleton;

template <typename T>
class SingletonGuard {

	private:

		Singleton<T>* singleton;

	public:

		SingletonGuard(Singleton<T>* singleton) {
			this->singleton = singleton;
		}

		~SingletonGuard() {
			this->singleton->reset();
		}

};

template <typename T>
class Singleton {

	private:

		std::unique_ptr<T> object {nullptr};

	public:

		void reset() {
			object.reset(nullptr);
		}

		template <typename... Args>
		SingletonGuard<T> create(Args... args) {
			object.reset(new T {args...});
			return {this};
		}

		T& operator* () {
			return *object;
		}

		T* operator-> () {
			return object.get();
		}

		bool operator ()() {
			return static_cast<bool>(object);
		}

		bool operator !() {
			return !object.get();
		}

};

