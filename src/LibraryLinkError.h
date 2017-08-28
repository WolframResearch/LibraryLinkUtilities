/**
 * @file	LibraryLinkError.hpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	20/04/2017
 *
 * @brief	Error class and error codes used by LibraryLink Utilities classes
 *
 */
#ifndef LLUTILS_LIBRARYLINKERROR_H_
#define LLUTILS_LIBRARYLINKERROR_H_

#include <stdexcept>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <utility>

#include "WolframLibrary.h"

namespace LibraryLinkUtils {


	/**
	 * @enum 	LLErrorCode
	 * @brief 	Error codes used in LibraryLink Utilities
	 */
	namespace LLErrorCode {

		// Original LibraryLink error codes:
		constexpr int VersionError = 7;	///< same as LIBRARY_VERSION_ERROR
		constexpr int FunctionError = 6; 	///< same as LIBRARY_FUNCTION_ERROR
		constexpr int MemoryError = 5; 	///< same as LIBRARY_MEMORY_ERROR
		constexpr int NumericalError = 4; 	///< same as LIBRARY_NUMERICAL_ERROR
		constexpr int DimensionsError = 3; ///< same as LIBRARY_DIMENSIONS_ERROR
		constexpr int RankError = 2; 		///< same as LIBRARY_RANK_ERROR
		constexpr int TypeError = 1; 		///< same as LIBRARY_TYPE_ERROR
		constexpr int NoError = 0; 		///< same as LIBRARY_NO_ERROR

		// Reserved for use in paclets:
		// -1
		// ...
		// -100

		// MArgument errors: [-101 : -200]
		constexpr int MArgumentInitError = -101; 		///< MArgumentManager construction failed
		constexpr int MArgumentIndexError = -102; 		///< wrong argument index
		constexpr int MArgumentRawArrayError = -103; 	///< error involving RawArray argument
		constexpr int MArgumentTensorError = -104; 		///< error involving Tensor argument
		constexpr int MArgumentImageError = -105; 		///< error involving Image argument

		// ErrorManager errors: [-151 : -200]
		constexpr int ErrorManagerThrowIdError = -151; 		///< trying to throw exception with non-existent id
		constexpr int ErrorManagerThrowNameError = -152; 	///< trying to throw exception with non-existent name
		constexpr int ErrorManagerCreateNameError = -153; 	///< trying to register exception with already existing name

		// RawArray errors: [-201 : -300]
		constexpr int RawArrayInitError = -201; 	///< construction of RawArray failed
		constexpr int RawArrayNewError = -202; 		///< creating new MRawArray failed
		constexpr int RawArrayCloneError = -203; 	///< MRawArray cloning failed
		constexpr int RawArrayTypeError = -204; 	///< MRawArray type mismatch
		constexpr int RawArraySizeError = -205; 	///< wrong assumption about RawArray size
		constexpr int RawArrayIndexError = -206; 	///< trying to access non-existing element

		// MTensor errors: [-301 : -400]
		constexpr int TensorInitError = -301; 		///< Tensor construction failed
		constexpr int TensorNewError = -302; 		///< creating new MTensor failed
		constexpr int TensorCloneError = -303; 		///< MTensor cloning failed
		constexpr int TensorTypeError = -304; 		///< Tensor type mismatch
		constexpr int TensorSizeError = -305; 		///< wrong assumption about Tensor size
		constexpr int TensorIndexError = -306; 		///< trying to access non-existing element

		// MImage errors: [-401 : -500]
		constexpr int ImageInitError = -401; 		///< Image construction failed
		constexpr int ImageNewError = -402; 		///< creating new MImage failed
		constexpr int ImageCloneError = -403; 		///< MImage cloning failed
		constexpr int ImageTypeError = -404; 		///< Image type mismatch
		constexpr int ImageSizeError = -405; 		///< wrong assumption about Image size
		constexpr int ImageIndexError = -406; 		///< trying to access non-existing element

	};

	/**
	 * @class	LibraryLinkError
	 * @brief	This is a class template, where template parameter is an enumerated type containing (possibly user-defined) error codes.
	 * @tparam	Enum - enumerated type with error codes
	 * @note	Within LibraryLink Utilities \b Enum class will always be LLErrorCode
	 **/
	class LibraryLinkError: public std::runtime_error {
		friend class ErrorManager;
	public:
		using IdType = int;

		void setDebugInfo(std::string dbg) {
			debugInfo = std::move(dbg);
		}

		/**
		 *   @brief Get the value of error code
		 **/
		IdType id() const noexcept {
			return errorId;
		}

		/**
		 *   @brief Alias for id() to preserve backwards compatibility
		 **/
		IdType which() const noexcept {
			return errorId;
		}

		/**
		 *   @brief Get the value of error code
		 **/
		const std::string& name() const noexcept {
			return type;
		}

		/**
		 *   @brief Get the value of error code
		 **/
		const std::string& message() const noexcept {
			return messageTemplate;
		}

	private:
		/**
		 *   @brief         Constructs an exception with given error code and predefined error message
		 *   @param[in]     which - error code of template type \b Enum
		 *   @warning		Default error message assumes that the value of error code will fit into \b int
		 **/
		LibraryLinkError(IdType which, std::string t, std::string msg) :
				std::runtime_error(t), errorId(which), type(std::move(t)), messageTemplate(std::move(msg)) {

		}

		const IdType errorId;
		const std::string type;
		const std::string messageTemplate;
		std::string debugInfo;
	};


	class ErrorManager {
	public:
		ErrorManager() = delete;

		static void registerPacletErrors(const std::vector<std::pair<std::string, std::string>>& errors);
		static void registerPacletErrors(std::vector<std::pair<std::string, std::string>>&& errors);

		static void set(std::string errorName, std::string errorData);

		static void throwException(int errorId);
		static void throwException(int errorId, const std::string& debugInfo);
		static void throwException(const std::string& errorName);
		static void throwException(const std::string& errorName, const std::string& debugInfo);

		static void sendRegisteredErrorsViaMathlink(MLINK mlp);
	private:
		static const LibraryLinkError& findError(int errorId);
		static const LibraryLinkError& findError(const std::string& errorName);

		using ErrorMap = std::unordered_map<std::string, const LibraryLinkError>;
		static ErrorMap errors;
		static ErrorMap initErrorMap(std::initializer_list<LibraryLinkError> initList);

		static const ErrorMap::const_iterator insertionHint;
		static int nextErrorId;
	};


} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_LIBRARYLINKERROR_H_ */
