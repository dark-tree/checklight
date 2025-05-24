
#include "file.hpp"

void file::createPathDirectories(const std::string& file_path) {
	std::filesystem::path path(file_path);
	std::filesystem::create_directories(path.parent_path());
}