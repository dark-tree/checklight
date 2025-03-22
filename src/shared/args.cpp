
#include "args.hpp"
#include "logger.hpp"

/*
 * Args
 */

Args::Args(int argc, const char* argv[]) {

	if (argc == 0) {
		out::warn("Zero arguments provided on startup, weird...");
		program = "checklight";
		return;
	}

	program = argv[0];
	std::string key = "";

	for (int i = 1; i < argc; i++) {
		std::string arg = argv[i];

		if (arg.starts_with("--") || arg.starts_with("-")) {
			args[arg] = "";
			key = arg;
		} else {
			if (!key.empty()) {
				args[key] = arg;
				key = "";
				continue;
			}

			out::warn("Unexpected value '%s', did you mean '--%s'?", arg.c_str(), arg.c_str());
		}
	}

}

bool Args::has(const char* flag) const {
	return args.find(flag) != args.end();
}

const std::string& Args::get(const char* flag) const {
	return args.at(flag);
}

const std::string& Args::name() const {
	return program;
}


