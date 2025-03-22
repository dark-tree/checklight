#pragma once

#include "external.hpp"
#include "file.hpp"

/**
 * Trigger a fatal engine error, application will terminate
 * The error message is composed of concatinated arguments, follow normal logger guildlines
 */
template <typename... Args>
[[noreturn]] void FAULT(Args... args) noexcept;

/**
 * Thread safe file/output logger
 * in most cases it should not be used directly
 */
class Logger {

	public:

		enum Level : uint32_t {
			DEBUG   = 0b00001,
			INFO    = 0b00010,
			WARNING = 0b00100,
			ERROR   = 0b01000,
		};

		static constexpr uint32_t LEVEL_NORMAL = INFO | WARNING | ERROR;
		static constexpr uint32_t LEVEL_VERBOSE = DEBUG | LEVEL_NORMAL;

	private:

		uint32_t mask = LEVEL_NORMAL;
		std::mutex watchtower;
		std::FILE* output = nullptr;

		const char* getLevelName(Level level);

		template <typename... Args>
		void unwrap(Level level, const char* format, Args... args) {

			// discard messaged based on log level
			if (!(level & mask)) {
				return;
			}

			// in debug mode we try to validate logging itself a bit
			#if ENGINE_DEBUG
				std::string raw = format;

				if (raw.empty()) {
					warn("Invalid logger usage, tried to print empty message, check your code!");
					return;
				}

				if (raw.back() == '\n') {
					error("Invalid logger usage, last character in print format was '\\n', check your code!");
					return;
				}
			#endif

			const auto time = std::time(nullptr);

			std::ostringstream s;
			s << std::put_time(std::localtime(&time), "[%H:%M:%S]");
			s << " ";
			s << getLevelName(level);
			s << ": ";
			s << format;
			s << "\n";

			const std::string fmt = s.str();
			print(fmt.c_str(), args...);
		}

	public:

		Logger(const char* path);
		~Logger();

		/**
		 * Set the mask used to determine if the message should be printed or ignored
		 */
		void setLogLevelMask(uint32_t level);

		/**
		 * Print to the logger, behaves like the standard printf() function
		 * Use only when ABSOLUTELY necessary
		 */
		template <typename... Args>
		void print(const char* format, Args... args) {

			// in debug mode we try to validate logging itself a bit
			#if ENGINE_DEBUG
				int argc = 0;

				for (int i = 0; format[i] != '\0'; i++) {
					if (format[i] == '%' && format[i + 1] != '%') {
						argc ++;
					}
				}

				if (sizeof...(args) != argc) {
					FAULT("Invalid logger usage, format and args do not match, check your code!");
				}
			#endif


			std::unique_lock lock(watchtower);
			printf(format, args...);

			if (output) {
				fprintf(output, format, args...);

				// inefficient but needed so nothing ever breaks
				// no matter what happens with the application
				fflush(output);
			}
		}

		template <typename... Args>
		void debug(Args... args) {
			unwrap(DEBUG, args...);
		}

		template <typename... Args>
		void info(Args... args) {
			unwrap(INFO, args...);
		}

		template <typename... Args>
		void warn(Args... args) {
			unwrap(WARNING, args...);
		}

		template <typename... Args>
		void error(Args... args) {
			unwrap(ERROR, args...);
		}

};

/**
 * Static wrapper around the thread-safe file/output
 * Logger class for general purpose use in the engine
 */
namespace out {

	extern Logger logger;

	/**
	 * Print a debug message, those will appear only if the program is started with
	 * a '--verbose' flag and should be used for overly verbose information about the engine inner workings
	 *
	 * Examples
	 * - General debugging prints
	 * - Special non-abnormal cases in algorithms
	 *
	 * Style guide
	 * - Begin with a capital letter
	 * - Put no new line at the end
	 * - Print only single lines
	 * - Place no dot at the end, but "!", "?", "..." are allowed
	 *
	 * @note Even with debug messages try to avoid printing inside the
	 *       frame/update loop to make the log readable and grow to large sizes
	 */
	template <typename... Args>
	void debug(Args... args) {
		logger.debug(args...);
	}

	/**
	 * Print a info message, those are meant to notify the user that some normal operation
	 * or stage was completed without issues, and are meant to be used sparingly, for more spammy messages see
	 * out::debug()
	 *
	 * Examples
	 * - Competition of expensive and/or rare operations
	 * - Resource management information
	 *
	 * Style guide
	 * - Begin with a capital letter
	 * - Put no new line at the end
	 * - Print only single lines
	 * - Place no dot at the end, but "!", "?", "..." are allowed
	 */
	template <typename... Args>
	void info(Args... args) {
		logger.info(args...);
	}

	/**
	 * Print a warning message, those are meant to notify the user that some abnormal of non-ideal condition has occurred
	 * but that (at least at the current time) that condition has not further bearing on the engine's continued operation
	 *
	 * Examples
	 * - Missing optional Vulkan extension required for an optimized code path
	 * - Unexpected conditions that don't necessarily mean anything is wrong but are unexpected nonetheless
	 *
	 * Style guide
	 * - Begin with a capital letter
	 * - Put no new line at the end
	 * - Print only single lines
	 * - Place no dot at the end, but "!", "?", "..." are allowed
	 */
	template <typename... Args>
	void warn(Args... args) {
		logger.warn(args...);
	}

	/**
	 * Print a error message, those are meant to notify the user that some abnormal of non-ideal condition has occurred
	 * that will cause the engine to (now or later) cease operation or required complex special handling on the engine's part to avoid
	 * in the future
	 *
	 * Examples
	 * - Invalid Vulkan state, failed assertions
	 * - Errors reported by the operating system or external libraries
	 *
	 * Style guide
	 * - Begin with a capital letter
	 * - Put no new line at the end
	 * - Print only single lines
	 * - Place no dot at the end, but "!", "?", "..." are allowed
	 */
	template <typename... Args>
	void error(Args... args) {
		logger.error(args...);
	}

}

template <typename... Args>
[[noreturn]] void FAULT(Args... args) noexcept {

	std::ostringstream s;
	((s << args), ...);
	auto message = s.str();

	out::error("%s", message.c_str());
	out::error("Fatal error occurred, application will be terminated!");

	auto nuke = [&] {
		throw std::runtime_error {message};
	};

	nuke();
	UNREACHABLE;
}
