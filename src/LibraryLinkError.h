/**
 * @file	LibraryLinkError.h
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

/**
 * @namespace LibraryLinkUtils
 * @brief Main namespace of LibraryLinkUtilities
 */
namespace LibraryLinkUtils {


	/**
	 * @brief Error codes predefined in LibraryLinkUtilities
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
	 * @brief	Class representing an exception in paclet code
	 *
	 * All exceptions that are thrown from paclet code should be of this class. To prevent users from overriding predefined LLU exceptions the constructor
	 * of LibraryLinkError class is private. Developers should use ErrorManager::throwException method to throw exceptions.
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
		 *   @param[in]     which - error code
		 *   @param[in]		t - error type/name
		 *   @param[in]		msg - error description
		 *   @warning		This is constructor is not supposed to be used directly by paclet developers. All errors should be thrown by ErrorManager.
		 **/
		LibraryLinkError(IdType which, std::string t, std::string msg) :
				std::runtime_error(t), errorId(which), type(std::move(t)), messageTemplate(std::move(msg)) {

		}

		const IdType errorId;
		const std::string type;
		const std::string messageTemplate;
		std::string debugInfo;
	};


	/**
	 * @class	ErrorManager
	 * @brief	"Static" class responsible for error registration and throwing
	 *
	 * ErrorManager holds a map with all errors that may be thrown from paclet code. These are: LLU errors (with codes between 0 and 7 or smaller than -100)
	 * plus paclet-specific errors which should be registered (for example in WolframLibrary_initialize) using registerPacletErrors function.
	 * Developers must never throw LibraryLinkErrors directly, instead they should use one of ErrorManager::throwException overloads.
	 **/
	class ErrorManager {
	public:

		/**
		 * @brief Default constructor is deleted since ErrorManager is supposed to be completely static
		 */
		ErrorManager() = delete;

		/**
		 * @brief 	Function used to register paclet-specific errors.
		 * @param 	errors - a list of pairs: {"ErrorName", "Short string with error description"}
		 */
		static void registerPacletErrors(const std::vector<std::pair<std::string, std::string>>& errors);
		static void registerPacletErrors(std::vector<std::pair<std::string, std::string>>&& errors);

		/**
		 * @brief 	Use this function to add new entry to the map of registered errors.
		 * @param 	errorName - string with error name
		 * @param 	errorData - string with error description
		 */
		static void set(std::string errorName, std::string errorData);

		/**
		 * @brief 	Throw exception with given id.
		 * @param 	errorId - id of error to be thrown
		 * @note	Developers are not allowed to make any assumptions about error ids of paclet-specific errors other than that they are in range -1 : -100.
		 * So this overload of throwException should be only used to throw LLU-specific exceptions whose ids are known at compile time and documented.
		 */
		static void throwException(int errorId);

		/**
		 * @brief 	Throw exception with given id and some additional debug information.
		 * @param 	errorId - id of error to be thrown
		 * @param 	debugInfo - additional message with debug info, this message will not be passed to top-level Failure object
		 */
		static void throwException(int errorId, const std::string& debugInfo);

		/**
		 * @brief 	Throw exception with given name.
		 * @param 	errorName - name of the error that will be thrown
		 */
		static void throwException(const std::string& errorName);

		/**
		 * @brief 	Throw exception with given name and some additional debug information.
		 * @param 	errorName - name of the error that will be thrown
		 * @param	debugInfo - additional message with debug info, this message will not be passed to top-level Failure object
		 */
		static void throwException(const std::string& errorName, const std::string& debugInfo);

		/**
		 * @brief Function used to send all registered errors to top-level Mathematica code.
		 *
		 * Sending registered errors allows for nice and meaningful Failure objects to be generated when paclet function fails in top level,
		 * instead of usual LibraryFunctionError expressions.
		 * @param mlp - active MathLink connection
		 */
		static void sendRegisteredErrorsViaMathlink(MLINK mlp);

	private:
		/**
		 * @brief Find error by id.
		 * @param errorId - error id
		 * @return const& to the desired error
		 */
		static const LibraryLinkError& findError(int errorId);

		/**
		 * @brief Find error by name.
		 * @param errorName - error name
		 * @return const& to the desired error
		 */
		static const LibraryLinkError& findError(const std::string& errorName);

		/// Errors are stored in a map with elements of the form { "ErrorName", immutable LibraryLinkError object }
		using ErrorMap = std::unordered_map<std::string, const LibraryLinkError>;

		/// Static map of registered errors
		static ErrorMap& errors;

		/***
		 * @brief Initialization of static error map
		 * @param initList - list of errors used internally by LLU
		 * @return reference to static error map
		 */
		static ErrorMap& initErrorMap(std::initializer_list<LibraryLinkError> initList);

		/// Helper data to speed up error registering
		static const ErrorMap::const_iterator insertionHint;

		/// Id that will be assigned to the next registered errors. Developers should not make any assumptions about this value
		static int nextErrorId;
	};


} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_LIBRARYLINKERROR_H_ */
