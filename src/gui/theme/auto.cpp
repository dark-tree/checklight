#include "auto.hpp"

#include "dark.hpp"
#include "light.hpp"

#if defined(WIN32) || defined(_WIN32)
#	include <windows.h>
#endif

bool AutoTheme::isDarkTheme() const {

#if defined(WIN32) || defined(_WIN32)

	/*
	 * Windows
	 */

	// https://stackoverflow.com/a/70753913
	auto buffer = std::vector<char>(4);
	auto cbData = static_cast<DWORD>(buffer.size() * sizeof(char));
	auto res = RegGetValueW(
		HKEY_CURRENT_USER,
		L"Software\\Microsoft\\Windows\\CurrentVersion\\Themes\\Personalize",
		L"AppsUseLightTheme",
		RRF_RT_REG_DWORD, // expected value type
		nullptr,
		buffer.data(),
		&cbData);

	if (res != ERROR_SUCCESS) {
		out::error("Failed to determine system appearance!");
	}

	// convert bytes written to our buffer to an int, assuming little-endian
	auto i = int(buffer[3] << 24 |
		buffer[2] << 16 |
		buffer[1] << 8 |
		buffer[0]);

	// 1 - light
	return i != 1;

#elif __linux__

	/*
	 * Linux
	 */

	// https://stackoverflow.com/a/478960
	auto exec = [] (const char* cmd) -> std::string {
		char buffer[128];
		std::string result = "";
		FILE* pipe = popen(cmd, "r");
		if (!pipe) throw std::runtime_error("popen() failed!");
		try {
			while (fgets(buffer, sizeof buffer, pipe) != NULL) {
				result += buffer;
			}
		} catch (...) {
			pclose(pipe);
			throw;
		}
		pclose(pipe);
		return result;
	};

	// https://unix.stackexchange.com/a/723275
	std::string result = exec("gdbus call --session --timeout=1000"
           " --dest=org.freedesktop.portal.Desktop"
           " --object-path /org/freedesktop/portal/desktop"
           " --method org.freedesktop.portal.Settings.Read org.freedesktop.appearance color-scheme");

	// shouldn't happen
	if (result.empty()) {
		return false;
	}

	const std::regex expr {R"(\(<<\w+ (\d+)>>,\)\n?)"};
	std::smatch match;

	if (std::regex_match(result, match, expr)) {

		// 0 - no preference
		// 1 - dark
		// 2 - light
		return match.size() == 2 && match[1] == "1";
	}

	out::error("Failed to determine system appearance!");

#endif

	return false;
}

AutoTheme::AutoTheme() {
	setTheme(isDarkTheme()
		? std::static_pointer_cast<Theme>(std::make_shared<DarkTheme>())
		: std::static_pointer_cast<Theme>(std::make_shared<LightTheme>()));
}
