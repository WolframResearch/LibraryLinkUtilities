/**
 * @file	Logger.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Implementation of functions defined in Logger.h.
 */
#include "LLU/ErrorLog/Logger.h"

namespace LibraryLinkUtils {

	namespace Logger {
		std::string to_string(Level l) {
			switch(l) {
				case Level::Debug:
					return "Debug";
				case Level::Warning:
					return "Warning";
				case Level::Error:
					return "Error";
				default:
					return "Unknown ";
			}
		}
	}
}