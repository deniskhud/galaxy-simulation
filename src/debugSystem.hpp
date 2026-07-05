#ifndef GALACTIC_DEBUGSYSTEM_HPP
#define GALACTIC_DEBUGSYSTEM_HPP
#include <format>
#include <iostream>
#include <stdexcept>
#include <string>

#ifdef NDEBUG
constexpr bool enableValidationLayers = false;
#else
constexpr bool enableValidationLayers = true;
#endif

/**
 * @def LOG_INFO
 * @brief Logs an informational message
 *
 * Active only when @c enableValidationLayers is true. Writes to @c std::cout
 * @param tag Source/module name.
 * @param ... Format arguments.
 *
 * @code
 * LOG_INFO("Renderer", "Loaded {} chunks", chunkCount);
 * @endcode
 *
 * @warning The format string must be a compile-time string literal
 *          (required by @c std::format's consteval validation).
 *          Runtime strings (e.g. function return values) cannot be used
 *          as the format string itself - pass them as arguments instead.
 */
#define LOG_INFO(tag, ...) \
do { if (enableValidationLayers) debugSystem::log(LogLevel::INFO, tag, std::format(__VA_ARGS__)); } while(0)


/**
 * @def LOG_WARNING
 * @brief Logs a warning message
 *
 * Active only when @c enableValidationLayers is true. Writes to @c std::cerr
 *
 *
 * @param tag Source/module name.
 * @param ... Format arguments.
 *
 * @code
 * LOG_WARNING("Window", "Frame buffer resized to {}x{}", width, height);
 * @endcode
 *
 * @warning The format string must be a compile-time string literal
 *          (required by @c std::format's consteval validation).
 */
#define LOG_WARNING(tag, ...) \
do { if (enableValidationLayers) debugSystem::log(LogLevel::WARNING, tag, std::format(__VA_ARGS__)); } while(0)


/**
 * @def LOG_ERROR
 * @brief Logs an error message and throws
 *
 * Always active, in both debug and release builds - critical errors
 * must never be silently suppressed. Writes to @c std::cerr
 *
 * @throw std::runtime_error Always thrown after logging.
 *
 * @warning The format string must be a compile-time string literal
 *          (required by @c std::format's consteval validation).
 *
 * @param tag     Source/module name (string literal).
 * @param ...     std::format-style format string followed by its arguments.
 *
 * @code
 * LOG_ERROR("Window", "Failed to create window surface: {}", std::string(SDL_GetError()));
 * @endcode
 */
	#define LOG_ERROR(tag, ...) \
	do { debugSystem::log(LogLevel::ERROR, tag, std::format(__VA_ARGS__)); } while(0)

enum class LogLevel {
	INFO,
	WARNING,
	ERROR,
};

class debugSystem final {
public:
	static void log(LogLevel logLevel, const std::string& tag, const std::string& message) {
		switch (logLevel) {
			case LogLevel::WARNING:
				std::cerr << format(logLevel, tag, message);
				break;
			case LogLevel::ERROR: {
				std::string msg = format(logLevel, tag, message);
				std::cerr << msg;
				throw std::runtime_error(msg);
			}
			default:
				std::cout << format(logLevel, tag, message);
				break;
		}
	}
private:
	static std::string format(LogLevel logLevel, const std::string& tag, const std::string& message) {
		return logLevelToString(logLevel) + "[" + tag + "]: " + message + "\n";
	}
	static std::string logLevelToString(LogLevel logLevel) {
		switch (logLevel) {
			case LogLevel::WARNING: return "[WARNING] ";
			case LogLevel::ERROR:   return "[ERROR] ";
			default:                return "[INFO] ";
		}
	}
};
#endif // GALACTIC_DEBUGSYSTEM_HPP