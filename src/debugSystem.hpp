#ifndef GALACTIC_DEBUGSYSTEM_HPP
#define GALACTIC_DEBUGSYSTEM_HPP
#include "include/inc.hpp"

#include <iostream>
#include <ostream>
#include <string>

enum class LogLevel {
	INFO,
	WARNING,
	ERROR,
};

namespace debugSystem {
static void log(LogLevel logLevel, const std::string& tag, const std::string& logMessage) {
	if (!enableValidationLayers)
		return;
	std::string msg = "";
	switch (logLevel) {
	case LogLevel::WARNING: {
		msg += "[WARNING] ";
		break;
	}
	case LogLevel::ERROR: {
		msg += "[ERROR] ";
		break;
	}
	default: {
		msg += "[INFO] ";
		break;
	}
	}
	msg += std::string("[" + tag + "]: ") + logMessage + "\n";
	std::cout << msg;
}

} // namespace debugSystem
#endif // GALACTIC_DEBUGSYSTEM_HPP