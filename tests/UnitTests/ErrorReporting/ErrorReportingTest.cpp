/**
 * @file	ErrorReportingTest.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	March 17, 2019
 * @brief	Tests of error reporting functionality.
 */
#include <fstream>
#include <string>
#include <vector>

#include "mathlink.h"

#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	ErrorManager::registerPacletErrors({
		{"DataFileError", "Data in file `fname` in line `lineNumber` is invalid because `reason`."},
		{"RepeatedTemplateError", "Cannot accept `x` nor `y` because `x` is unacceptable. So are `y` and `z`."},
		{"NumberedSlotsError", "First slot is `1` and second is `2`."},
		{"RepeatedNumberTemplateError", "Cannot accept `` nor `` because `1` is unacceptable. So are `2` and ``."},
		{"MixedSlotsError", "This message `` mixes `2` different `kinds` of `` slots."}
	});
	return 0;
}

EXTERN_C DLLEXPORT int ReadData(WolframLibraryData, mint Argc, MArgument *Args, MArgument Res) {
		auto err = LLErrorCode::NoError;
		try {
			MArgumentManager mngr(Argc, Args, Res);
			auto fileName = mngr.getString(0);
			auto fileNameLen = static_cast<mlint64>(fileName.length());
			ErrorManager::throwException("DataFileError", fileName, fileNameLen, "data type is not supported");
		}
		catch (const LibraryLinkError& e) {
			err = e.which();
		}
		catch (...) {
			err = LLErrorCode::FunctionError;
		}
		return err;
}

LIBRARY_LINK_FUNCTION(ReadDataLocalWLD) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto fileName = mngr.getString(0);
		auto fileNameLen = static_cast<mlint64>(fileName.length());
		ErrorManager::throwException(libData, "DataFileError", fileName, fileNameLen, "data type is not supported");
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int RepeatedTemplate(WolframLibraryData, mint, MArgument*, MArgument) {
	auto err = LLErrorCode::NoError;
	try {
		ErrorManager::throwException("RepeatedTemplateError", "x", "y", "z");
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int NumberedSlots(WolframLibraryData, mint, MArgument*, MArgument) {
	auto err = LLErrorCode::NoError;
	try {
		ErrorManager::throwException("NumberedSlotsError", 1, std::vector<std::string> {"2", "3", "4"});
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int RepeatedNumberTemplate(WolframLibraryData, mint, MArgument*, MArgument) {
	auto err = LLErrorCode::NoError;
	try {
		ErrorManager::throwException("RepeatedNumberTemplateError", "x", "y", "z");
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int TooManyValues(WolframLibraryData, mint, MArgument*, MArgument) {
	auto err = LLErrorCode::NoError;
	try {
		ErrorManager::throwException("NumberedSlotsError", 1, 2, 3, 4, 5);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int TooFewValues(WolframLibraryData, mint, MArgument*, MArgument) {
	auto err = LLErrorCode::NoError;
	try {
		ErrorManager::throwException("NumberedSlotsError");
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int MixedSlots(WolframLibraryData, mint, MArgument*, MArgument) {
	auto err = LLErrorCode::NoError;
	try {
		ErrorManager::throwException("MixedSlotsError", 1, 2, 3, 4);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Tests and demonstration of ErrorManager::throwCustomError
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Define custom error class. LoggingError inherits from LibraryLinkError but it additionally logs some exception details to a file.
struct LoggingError : LibraryLinkError {
	template<typename... T>
	LoggingError(const LibraryLinkError& e, WolframLibraryData ld, int line, const std::string& file, const std::string& func, T&&... params) :
			LibraryLinkError(e) {
		// Pass LibraryLinkError's parameters to top-level
		setMessageParameters(ld, std::forward<T>(params)...);
		sendParameters(ld);
		// Extend debug info and log to a file
		setDebugInfo("Exception " + name() + " in " + file + ":" + std::to_string(line) + " in " + func + ": " + debug());
		std::ofstream log {logFileName, std::ios_base::app};
		log << debug() << "\n";
	}

	static constexpr const char* logFileName = "LLUErrorLog.txt";
};

// Use this macro to conveniently throw LoggingErrors with current line, filename and function name
#define THROW_LOGGING_ERROR(name, ...) ErrorManager::throwCustomException<LoggingError>(name, libData,  __LINE__, __FILE__, __func__, __VA_ARGS__)

LIBRARY_LINK_FUNCTION(ReadDataWithLoggingError) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto fileName = mngr.getString(0);
		if (fileName.find(':') != std::string::npos) {
			THROW_LOGGING_ERROR("DataFileError", fileName, 0, R"(file name contains a possibly problematic character ":")");
		}
		auto fileNameLen = static_cast<mlint64>(fileName.length());
		if (fileNameLen > 16) {
			THROW_LOGGING_ERROR("DataFileError", fileName, fileNameLen, "file name is too long");
		}
		THROW_LOGGING_ERROR("DataFileError", fileName, fileNameLen, "data type is not supported");
	}
	// Notice that even though we use custom error class that does some extra work, the error handling code stays the same
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Tests and demonstration of ErrorManager::sendParametersImmediately
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

LIBRARY_LINK_FUNCTION(GetSendParametersImmediately) {
	LibraryLinkUtils::Unused(libData);
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		mngr.setBoolean(ErrorManager::getSendParametersImmediately());
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(SetSendParametersImmediately) {
	LibraryLinkUtils::Unused(libData);
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		ErrorManager::setSendParametersImmediately(mngr.getBoolean(0));
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(ReadDataDelayedParametersTransfer) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto fileName = mngr.getString(0);
		auto fileNameLen = static_cast<mlint64>(fileName.length());
		ErrorManager::throwException("DataFileError", fileName, fileNameLen, "data type is not supported");
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
		e.sendParameters(libData);
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}