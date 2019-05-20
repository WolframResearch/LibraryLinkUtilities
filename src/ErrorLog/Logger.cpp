/**
 * @file	Logger.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Implementation of functions defined in Logger.h.
 */
#include "LLU/ErrorLog/Logger.h"

#include "LLU/LLU.h"
#include "LLU/LibraryLinkFunctionMacro.h"

namespace LibraryLinkUtils {

	std::string Logger::logSymbolContext;

	std::string Logger::to_string(Level l) {
		switch (l) {
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


	LIBRARY_LINK_FUNCTION(setLoggerContext) {
		auto err = LLErrorCode::NoError;
		try {
			MArgumentManager mngr {libData, Argc, Args, Res};
			auto newContext = mngr.getString(0);
			Logger::setContext(newContext);
			mngr.setString(Logger::getSymbol());
		}
		catch (LibraryLinkError& e) {
			err = e.which();
		}
		catch (...) {
			err = LLErrorCode::FunctionError;
		}
		return err;
	}
}