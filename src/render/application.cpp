
#include "application.hpp"

/*
 * ApplicationParameters
 */

std::string ApplicationParameters::getName() const {
	return name.empty() ? "Untitled" : name;
}

std::string ApplicationParameters::getTitle() const {
	return title.empty() ? getName() : title;
}

ApplicationParameters::ApplicationParameters() {
	this->major = 1;
	this->minor = 0;
	this->patch = 0;
	this->width = 720;
	this->height = 300;
}

ApplicationParameters ApplicationParameters::begin() {
	return {};
}

ApplicationParameters& ApplicationParameters::setVersion(int major, int minor, int patch) {
	this->major = major;
	this->minor = minor;
	this->patch = patch;
	return *this;
}

ApplicationParameters& ApplicationParameters::setName(const std::string& name) {
	this->name = name;
	return *this;
}

ApplicationParameters& ApplicationParameters::setDimensions(int width, int height) {
	this->width = width;
	this->height = height;
	return *this;
}

ApplicationParameters& ApplicationParameters::setTitle(const std::string& title) {
	this->title = title;
	return *this;
}