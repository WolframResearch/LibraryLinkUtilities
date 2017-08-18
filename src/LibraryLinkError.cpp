/** 
 * @file	LibraryLinkError.cpp
 * @brief	<brief description>
 * @details	<more detailed description>
 */

#include "mathlink.h"
#include "LibraryLinkError.h"

namespace LibraryLinkUtils {

	ErrorManager::ErrorMap ErrorManager::errors = {
		// Original LibraryLink error codes:
		{ "VersionError", LibraryLinkError { LLErrorCode::VersionError, "VersionError", "same as LIBRARY_VERSION_ERROR" } },
		{ "FunctionError", LibraryLinkError { LLErrorCode::FunctionError, "FunctionError", "same as LIBRARY_FUNCTION_ERROR" } },
		{ "MemoryError", LibraryLinkError { LLErrorCode::MemoryError, "MemoryError", "same as LIBRARY_MEMORY_ERROR" } },
		{ "NumericalError", LibraryLinkError { LLErrorCode::NumericalError, "NumericalError", "same as LIBRARY_NUMERICAL_ERROR" } },
		{ "DimensionsError", LibraryLinkError { LLErrorCode::DimensionsError, "DimensionsError", "same as LIBRARY_DIMENSIONS_ERROR" } },
		{ "RankError", LibraryLinkError { LLErrorCode::RankError, "RankError", "same as LIBRARY_RANK_ERROR" } },
		{ "TypeError", LibraryLinkError { LLErrorCode::TypeError, "TypeError", "same as LIBRARY_TYPE_ERROR" } },
		{ "NoError", LibraryLinkError { LLErrorCode::NoError, "NoError", "same as LIBRARY_NO_ERROR" } },

		// Reserved for use in paclets:
		// -1
		// ...
		// -100

		// MArgument errors: [-101 : -150]
		{ "MArgumentInitError", LibraryLinkError { LLErrorCode::MArgumentInitError, "MArgumentInitError", "MArgumentManager construction failed" } },
		{ "MArgumentIndexError", LibraryLinkError { LLErrorCode::MArgumentIndexError, "MArgumentIndexError", "wrong argument index" } },
		{ "MArgumentRawArrayError", LibraryLinkError { LLErrorCode::MArgumentRawArrayError, "MArgumentRawArrayError", "error involving RawArray argument" } },
		{ "MArgumentTensorError", LibraryLinkError { LLErrorCode::MArgumentTensorError, "MArgumentTensorError", "error involving Tensor argument" } },
		{ "MArgumentImageError", LibraryLinkError { LLErrorCode::MArgumentImageError, "MArgumentImageError", "error involving Image argument" } },

		// ErrorManager errors: [-151 : -200]
		{ "ErrorManagerThrowIdError", LibraryLinkError { LLErrorCode::ErrorManagerThrowIdError, "ErrorManagerThrowIdError", "trying to throw exception with non-existent id" } },
		{ "ErrorManagerThrowNameError", LibraryLinkError { LLErrorCode::ErrorManagerThrowNameError, "ErrorManagerThrowNameError", "trying to throw exception with non-existent name" } },
		{ "ErrorManagerCreateNameError", LibraryLinkError { LLErrorCode::ErrorManagerCreateNameError, "ErrorManagerCreateNameError", "trying to register exception with already existing name" } },

		// RawArray errors: [-201 : -300]
		{ "RawArrayInitError", LibraryLinkError { LLErrorCode::RawArrayInitError, "RawArrayInitError", "construction of RawArray failed" } },
		{ "RawArrayNewError", LibraryLinkError { LLErrorCode::RawArrayNewError, "RawArrayNewError", "creating new MRawArray failed" } },
		{ "RawArrayCloneError", LibraryLinkError { LLErrorCode::RawArrayCloneError, "RawArrayCloneError", "MRawArray cloning failed" } },
		{ "RawArrayTypeError", LibraryLinkError { LLErrorCode::RawArrayTypeError, "RawArrayTypeError", "MRawArray type mismatch" } },
		{ "RawArraySizeError", LibraryLinkError { LLErrorCode::RawArraySizeError, "RawArraySizeError", "wrong assumption about RawArray size" } },
		{ "RawArrayIndexError", LibraryLinkError { LLErrorCode::RawArrayIndexError, "RawArrayIndexError", "trying to access non-existing element" } },

		// MTensor errors: [-301 : -400]
		{ "TensorInitError", LibraryLinkError { LLErrorCode::TensorInitError, "TensorInitError", "Tensor construction failed" } },
		{ "TensorNewError", LibraryLinkError { LLErrorCode::TensorNewError, "TensorNewError", "creating new MTensor failed" } },
		{ "TensorCloneError", LibraryLinkError { LLErrorCode::TensorCloneError, "TensorCloneError", "MTensor cloning failed" } },
		{ "TensorTypeError", LibraryLinkError { LLErrorCode::TensorTypeError, "TensorTypeError", "Tensor type mismatch" } },
		{ "TensorSizeError", LibraryLinkError { LLErrorCode::TensorSizeError, "TensorSizeError", "wrong assumption about Tensor size" } },
		{ "TensorIndexError", LibraryLinkError { LLErrorCode::TensorIndexError, "TensorIndexError", "trying to access non-existing element" } },

		// MImage errors: [-401 : -500]
		{ "ImageInitError", LibraryLinkError { LLErrorCode::ImageInitError, "ImageInitError", "Image construction failed" } },
		{ "ImageNewError", LibraryLinkError { LLErrorCode::ImageNewError, "ImageNewError", "creating new MImage failed" } },
		{ "ImageCloneError", LibraryLinkError { LLErrorCode::ImageCloneError, "ImageCloneError", "MImage cloning failed" } },
		{ "ImageTypeError", LibraryLinkError { LLErrorCode::ImageTypeError, "ImageTypeError", "Image type mismatch" } },
		{ "ImageSizeError", LibraryLinkError { LLErrorCode::ImageSizeError, "ImageSizeError", "wrong assumption about Image size" } },
		{ "ImageIndexError", LibraryLinkError { LLErrorCode::ImageIndexError, "ImageIndexError", "trying to access non-existing element" } },

	};

	const ErrorManager::ErrorMap::const_iterator ErrorManager::insertionHint = errors.find("MArgumentInitError");
	int ErrorManager::nextErrorId = LLErrorCode::NoError - 1;


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

