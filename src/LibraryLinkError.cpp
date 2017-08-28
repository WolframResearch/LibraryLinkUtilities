/** 
 * @file	LibraryLinkError.cpp
 * @brief	<brief description>
 * @details	<more detailed description>
 */

#include "mathlink.h"
#include "LibraryLinkError.h"

namespace LibraryLinkUtils {

	ErrorManager::ErrorMap ErrorManager::errors = initErrorMap({
		// Original LibraryLink error codes:
		{ LLErrorCode::VersionError, "VersionError", "same as LIBRARY_VERSION_ERROR" },
		{ LLErrorCode::FunctionError, "FunctionError", "same as LIBRARY_FUNCTION_ERROR" },
		{ LLErrorCode::MemoryError, "MemoryError", "same as LIBRARY_MEMORY_ERROR" },
		{ LLErrorCode::NumericalError, "NumericalError", "same as LIBRARY_NUMERICAL_ERROR" },
		{ LLErrorCode::DimensionsError, "DimensionsError", "same as LIBRARY_DIMENSIONS_ERROR" },
		{ LLErrorCode::RankError, "RankError", "same as LIBRARY_RANK_ERROR" },
		{ LLErrorCode::TypeError, "TypeError", "same as LIBRARY_TYPE_ERROR" },
		{ LLErrorCode::NoError, "NoError", "same as LIBRARY_NO_ERROR" },

		// Reserved for use in paclets:
		// -1
		// ...
		// -100

		// MArgument errors: [-101 : -150]
		{ LLErrorCode::MArgumentInitError, "MArgumentInitError", "MArgumentManager construction failed" },
		{ LLErrorCode::MArgumentIndexError, "MArgumentIndexError", "wrong argument index" },
		{ LLErrorCode::MArgumentRawArrayError, "MArgumentRawArrayError", "error involving RawArray argument" },
		{ LLErrorCode::MArgumentTensorError, "MArgumentTensorError", "error involving Tensor argument" },
		{ LLErrorCode::MArgumentImageError, "MArgumentImageError", "error involving Image argument" },

		// ErrorManager errors: [-151 : -200]
		{ LLErrorCode::ErrorManagerThrowIdError, "ErrorManagerThrowIdError", "trying to throw exception with non-existent id" },
		{ LLErrorCode::ErrorManagerThrowNameError, "ErrorManagerThrowNameError", "trying to throw exception with non-existent name" },
		{ LLErrorCode::ErrorManagerCreateNameError, "ErrorManagerCreateNameError", "trying to register exception with already existing name" },

		// RawArray errors: [-201 : -300]
		{ LLErrorCode::RawArrayInitError, "RawArrayInitError", "construction of RawArray failed" },
		{ LLErrorCode::RawArrayNewError, "RawArrayNewError", "creating new MRawArray failed" },
		{ LLErrorCode::RawArrayCloneError, "RawArrayCloneError", "MRawArray cloning failed" },
		{ LLErrorCode::RawArrayTypeError, "RawArrayTypeError", "MRawArray type mismatch" },
		{ LLErrorCode::RawArraySizeError, "RawArraySizeError", "wrong assumption about RawArray size" },
		{ LLErrorCode::RawArrayIndexError, "RawArrayIndexError", "trying to access non-existing element" },

		// MTensor errors: [-301 : -400]
		{ LLErrorCode::TensorInitError, "TensorInitError", "Tensor construction failed" },
		{ LLErrorCode::TensorNewError, "TensorNewError", "creating new MTensor failed" },
		{ LLErrorCode::TensorCloneError, "TensorCloneError", "MTensor cloning failed" },
		{ LLErrorCode::TensorTypeError, "TensorTypeError", "Tensor type mismatch" },
		{ LLErrorCode::TensorSizeError, "TensorSizeError", "wrong assumption about Tensor size" },
		{ LLErrorCode::TensorIndexError, "TensorIndexError", "trying to access non-existing element" },

		// MImage errors: [-401 : -500]
		{ LLErrorCode::ImageInitError, "ImageInitError", "Image construction failed" },
		{ LLErrorCode::ImageNewError, "ImageNewError", "creating new MImage failed" },
		{ LLErrorCode::ImageCloneError, "ImageCloneError", "MImage cloning failed" },
		{ LLErrorCode::ImageTypeError, "ImageTypeError", "Image type mismatch" },
		{ LLErrorCode::ImageSizeError, "ImageSizeError", "wrong assumption about Image size" },
		{ LLErrorCode::ImageIndexError, "ImageIndexError", "trying to access non-existing element" }
	});

	const ErrorManager::ErrorMap::const_iterator ErrorManager::insertionHint = errors.find("MArgumentInitError");
	int ErrorManager::nextErrorId = LLErrorCode::NoError - 1;

	auto ErrorManager::initErrorMap(std::initializer_list<LibraryLinkError> initList) -> ErrorMap {
		ErrorMap errMap;
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
		long len;
		MLCheckFunction(mlp, "List", &len);
		MLNewPacket(mlp);
		MLPutFunction(mlp, "Association", errors.size());
		for (const auto& err : errors) {
			MLPutFunction(mlp, "Rule", 2);
			MLPutUTF8String(mlp, reinterpret_cast<const unsigned char*>(err.first.c_str()), static_cast<int>(err.first.size()));
			MLPutFunction(mlp, "List", 2);
			MLPutInteger32(mlp, err.second.id());
			MLPutUTF8String(mlp, reinterpret_cast<const unsigned char*>(err.second.message().c_str()), static_cast<int>(err.second.message().size()));
		}
		MLEndPacket(mlp);
		MLFlush(mlp);
	}

	EXTERN_C DLLEXPORT int sendRegisteredErrors(WolframLibraryData libData, MLINK mlp) {
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

