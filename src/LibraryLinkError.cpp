/** 
 * @file	LibraryLinkError.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	20/04/2017
 *
 * @brief	Contains definitions of ErrorManager members and implementation of interface function sendRegisteredErrors used by PacletFailure framework in LLU
 *
 */

#include "mathlink.h"

#include "LibraryLinkError.h"

#include "Utilities.hpp"
#include "ML/MathLinkStream.h"
#include "ML/Utilities.h"

namespace LibraryLinkUtils {

	ErrorManager::ErrorMap& ErrorManager::errors = initErrorMap({
		// Original LibraryLink error codes:
		{ LLErrorCode::VersionError, 	"VersionError", 	"An error was caused by an incompatible function call. The library was compiled with a previous WolframLibrary version." },
		{ LLErrorCode::FunctionError, 	"FunctionError", 	"An error occurred in the library function." },
		{ LLErrorCode::MemoryError, 	"MemoryError", 		"An error was caused by failed memory allocation or insufficient memory." },
		{ LLErrorCode::NumericalError, 	"NumericalError", 	"A numerical error was encountered." },
		{ LLErrorCode::DimensionsError, "DimensionsError", 	"An error caused by inconsistent dimensions or by exceeding array bounds." },
		{ LLErrorCode::RankError, 		"RankError", 		"An error was caused by a tensor with an inconsistent rank." },
		{ LLErrorCode::TypeError, 		"TypeError", 		"An error caused by inconsistent types was encountered." },
		{ LLErrorCode::NoError, 		"NoError", 			"No errors occurred." },

		// Reserved for use in paclets:
		// -1
		// ...
		// -100

		// MArgument errors: [-101 : -150]
		{ LLErrorCode::MArgumentInitError, 		"MArgumentInitError", 		"MArgumentManager construction failed." },
		{ LLErrorCode::MArgumentIndexError, 	"MArgumentIndexError", 		"An error was caused by an incorrect argument index." },
		{ LLErrorCode::MArgumentRawArrayError, 	"MArgumentRawArrayError", 	"An error was caused by a RawArray argument." },
		{ LLErrorCode::MArgumentTensorError, 	"MArgumentTensorError", 	"An error was caused by a Tensor argument." },
		{ LLErrorCode::MArgumentImageError, 	"MArgumentImageError", 		"An error was caused by an Image argument." },

		// ErrorManager errors: [-151 : -200]
		{ LLErrorCode::ErrorManagerThrowIdError, 	"ErrorManagerThrowIdError", 	"An exception was thrown with a non-existent id." },
		{ LLErrorCode::ErrorManagerThrowNameError, 	"ErrorManagerThrowNameError", 	"An exception was thrown with a non-existent name." },
		{ LLErrorCode::ErrorManagerCreateNameError, "ErrorManagerCreateNameError", 	"An exception was registered with a name that already exists." },

		// RawArray errors: [-201 : -300]
		{ LLErrorCode::RawArrayInitError, 	"RawArrayInitError", 	"Failed to construct RawArray." },
		{ LLErrorCode::RawArrayNewError, 	"RawArrayNewError", 	"Failed to create a new MRawArray." },
		{ LLErrorCode::RawArrayCloneError, 	"RawArrayCloneError", 	"Failed to clone MRawArray." },
		{ LLErrorCode::RawArrayTypeError, 	"RawArrayTypeError", 	"An error was caused by an MRawArray type mismatch." },
		{ LLErrorCode::RawArraySizeError, 	"RawArraySizeError", 	"An error was caused by an incorrect RawArray size." },
		{ LLErrorCode::RawArrayIndexError,	"RawArrayIndexError", 	"An error was caused by attempting to access a nonexistent RawArray element." },

		// MTensor errors: [-301 : -400]
		{ LLErrorCode::TensorInitError, 	"TensorInitError", 	"Failed to construct Tensor." },
		{ LLErrorCode::TensorNewError, 		"TensorNewError", 	"Failed to create a new MTensor." },
		{ LLErrorCode::TensorCloneError, 	"TensorCloneError", "Failed to clone MTensor." },
		{ LLErrorCode::TensorTypeError, 	"TensorTypeError", 	"An error was caused by an MTensor type mismatch." },
		{ LLErrorCode::TensorSizeError, 	"TensorSizeError", 	"An error was caused by an incorrect Tensor size." },
		{ LLErrorCode::TensorIndexError, 	"TensorIndexError", "An error was caused by attempting to access a nonexistent Tensor element." },

		// MImage errors: [-401 : -500]
		{ LLErrorCode::ImageInitError, 	"ImageInitError", 	"Failed to construct Image." },
		{ LLErrorCode::ImageNewError, 	"ImageNewError", 	"Failed to create a new MImage." },
		{ LLErrorCode::ImageCloneError, "ImageCloneError", 	"Failed to clone MImage." },
		{ LLErrorCode::ImageTypeError, 	"ImageTypeError", 	"An error was caused by an MImage type mismatch." },
		{ LLErrorCode::ImageSizeError, 	"ImageSizeError", 	"An error was caused by an incorrect Image size." },
		{ LLErrorCode::ImageIndexError, "ImageIndexError", 	"An error was caused by attempting to access a nonexistent Image element." },

		// MathLink errors: [-501 : -600]
		{ LLErrorCode::MLTestHeadError, 		"MLTestHeadError", 		"MLTestHead failed (wrong head or number of arguments)." },
		{ LLErrorCode::MLPutSymbolError, 		"MLPutSymbolError", 	"MLPutSymbol failed." },
		{ LLErrorCode::MLPutFunctionError, 		"MLPutFunctionError", 	"MLPutFunction failed." },
		{ LLErrorCode::MLTestSymbolError, 		"MLTestSymbolError", 	"MLTestSymbol failed (different symbol on the link than expected)." },
		{ LLErrorCode::MLWrongSymbolForBool, 	"MLWrongSymbolForBool", "Tried to read something else than \"True\" or \"False\" as boolean." },
		{ LLErrorCode::MLGetListError, 			"MLGetListError", 		"Could not get list from MathLink." },
		{ LLErrorCode::MLGetScalarError, 		"MLGetScalarError", 	"Could not get scalar from MathLink." },
		{ LLErrorCode::MLGetStringError, 		"MLGetStringError", 	"Could not get string from MathLink." },
		{ LLErrorCode::MLPutListError, 			"MLPutListError", 		"Could not send list via MathLink." },
		{ LLErrorCode::MLPutScalarError, 		"MLPutScalarError", 	"Could not send scalar via MathLink." },
		{ LLErrorCode::MLPutStringError, 		"MLPutStringError", 	"Could not send string via MathLink." }
	});

	const ErrorManager::ErrorMap::const_iterator ErrorManager::insertionHint = errors.find("MArgumentInitError");
	int ErrorManager::nextErrorId = LLErrorCode::NoError - 1;

	auto ErrorManager::initErrorMap(std::initializer_list<LibraryLinkError> initList) -> ErrorMap& {
		static ErrorMap errMap;
		for (auto&& err : initList) {
			errMap.insert({ err.name(), std::move(err)});
		}
		return errMap;
	}

	void ErrorManager::registerPacletErrors(const std::vector<std::pair<std::string, std::string>>& errors) {
		for (auto&& err : errors) {
			set(err.first, err.second);
		}
	}

	void ErrorManager::registerPacletErrors(std::vector<std::pair<std::string, std::string>>&& errors) {
		for (auto&& err : errors) {
			set(std::move(err.first), std::move(err.second));
		}
	}

	void ErrorManager::set(std::string errorName, std::string errorData) {
		auto elem = errors.emplace_hint(insertionHint, std::make_pair(errorName, LibraryLinkError { nextErrorId, errorName, errorData }));
		if (elem->second.id() != nextErrorId) {
			throw errors.find("ErrorManagerCreateNameError")->second;
		}
		nextErrorId--;
	}

	void ErrorManager::throwException(int errorId) {
		throw findError(errorId);
	}

	void ErrorManager::throwException(int errorId, const std::string& debugInfo) {
		auto exception = findError(errorId);
		exception.setDebugInfo(debugInfo);
		throw exception;
	}

	void ErrorManager::throwException(const std::string& errorName) {
		throw findError(errorName);
	}

	void ErrorManager::throwException(const std::string& errorName, const std::string& debugInfo) {
		auto exception = findError(errorName);
		exception.setDebugInfo(debugInfo);
		throw exception;
	}

	const LibraryLinkError& ErrorManager::findError(int errorId) {
		for (auto&& err : errors) {
			if (err.second.id() == errorId) {
				return err.second;
			}
		}
		throw errors.find("ErrorManagerThrowIdError")->second;
	}

	const LibraryLinkError& ErrorManager::findError(const std::string& errorName) {
		const auto& exception = errors.find(errorName);
		if (exception == errors.end()) {
			throw errors.find("ErrorManagerThrowNameError")->second;
		}
		return exception->second;
	}

	// Initial implementation, no error checking. Will be nicer when I wrap MLINK into stream class.
	void ErrorManager::sendRegisteredErrorsViaMathlink(MLINK mlp) {
		MathLinkStream ms(mlp, "List", 0);

		ms << ML::NewPacket << ML::Association(errors.size());

		for (const auto& err : errors) {
			ms << ML::Rule << err.first << ML::List(2) << err.second.id() << err.second.message();
		}

		ms << ML::EndPacket << ML::Flush;
	}

	EXTERN_C DLLEXPORT int sendRegisteredErrors(WolframLibraryData libData, MLINK mlp) {
		Unused(libData);
		auto err = LLErrorCode::NoError;
		try {
			ErrorManager::sendRegisteredErrorsViaMathlink(mlp);
		}
		catch (LibraryLinkError& e) {
			err = e.which();
		}
		catch (std::exception& e) {
			err = LLErrorCode::FunctionError;
		}
		return err;
	}
} /* namespace LibraryLinkUtils */

