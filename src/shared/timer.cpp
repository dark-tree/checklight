
#include "timer.hpp"

Timer::Timer()
: start(Clock::now()) {}

double Timer::milliseconds() const {
	return elapsed<std::milli>().count();
}

double Timer::nanoseconds() const {
	return elapsed<std::nano>().count();
}

Timer::operator bool() const {
	return true;
}

Timer Timer::of(const std::function<void()>& thing) {
	Timer timer;
	thing();
	return timer;
}