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
	 * @brief Error codes predefined in Library Link
	 */
	namespace LLErrorCode {

		// Original LibraryLink error codes:
		constexpr int VersionError = 7;		///< same as LIBRARY_VERSION_ERROR
		constexpr int FunctionError = 6; 	///< same as LIBRARY_FUNCTION_ERROR
		constexpr int MemoryError = 5; 		///< same as LIBRARY_MEMORY_ERROR
		constexpr int NumericalError = 4; 	///< same as LIBRARY_NUMERICAL_ERROR
		constexpr int DimensionsError = 3; 	///< same as LIBRARY_DIMENSIONS_ERROR
		constexpr int RankError = 2; 		///< same as LIBRARY_RANK_ERROR
		constexpr int TypeError = 1; 		///< same as LIBRARY_TYPE_ERROR
		constexpr int NoError = 0; 			///< same as LIBRARY_NO_ERROR
	}

	/**
	 * @brief Names of all errors used across LLU
	 */
	namespace LLErrorName {

		// Original LibraryLink error codes:
		extern const std::string VersionError;		///< same as LIBRARY_VERSION_ERROR
		extern const std::string FunctionError; 	///< same as LIBRARY_FUNCTION_ERROR
		extern const std::string MemoryError; 		///< same as LIBRARY_MEMORY_ERROR
		extern const std::string NumericalError; 	///< same as LIBRARY_NUMERICAL_ERROR
		extern const std::string DimensionsError;	///< same as LIBRARY_DIMENSIONS_ERROR
		extern const std::string RankError; 		///< same as LIBRARY_RANK_ERROR
		extern const std::string TypeError; 		///< same as LIBRARY_TYPE_ERROR
		extern const std::string NoError; 			///< same as LIBRARY_NO_ERROR

		// MArgument errors:
		extern const std::string MArgumentInitError; 		///< MArgumentManager construction failed
		extern const std::string MArgumentIndexError; 		///< wrong argument index
		extern const std::string MArgumentNumericArrayError; 	///< error involving NumericArray argument
		extern const std::string MArgumentTensorError; 		///< error involving Tensor argument
		extern const std::string MArgumentImageError; 		///< error involving Image argument

		// ErrorManager errors:
		extern const std::string ErrorManagerThrowIdError; 		///< trying to throw exception with non-existent id
		extern const std::string ErrorManagerThrowNameError; 	///< trying to throw exception with non-existent name
		extern const std::string ErrorManagerCreateNameError; 	///< trying to register exception with already existing name

		// NumericArray errors:
		extern const std::string NumericArrayInitError; 		///< construction of NumericArray failed
		extern const std::string NumericArrayNewError; 			///< creating new MNumericArray failed
		extern const std::string NumericArrayCloneError; 		///< MNumericArray cloning failed
		extern const std::string NumericArrayTypeError; 		///< MNumericArray type mismatch
		extern const std::string NumericArraySizeError; 		///< wrong assumption about NumericArray size
		extern const std::string NumericArrayIndexError; 		///< trying to access non-existing element
		extern const std::string NumericArrayConversionError;   ///< conversion from NumericArray of different type failed

		// MTensor errors:
		extern const std::string TensorInitError; 		///< Tensor construction failed
		extern const std::string TensorNewError; 		///< creating new MTensor failed
		extern const std::string TensorCloneError; 		///< MTensor cloning failed
		extern const std::string TensorTypeError; 		///< Tensor type mismatch
		extern const std::string TensorSizeError; 		///< wrong assumption about Tensor size
		extern const std::string TensorIndexError; 		///< trying to access non-existing element

		// MImage errors:
		extern const std::string ImageInitError; 		///< Image construction failed
		extern const std::string ImageNewError; 		///< creating new MImage failed
		extern const std::string ImageCloneError; 		///< MImage cloning failed
		extern const std::string ImageTypeError; 		///< Image type mismatch
		extern const std::string ImageSizeError; 		///< wrong assumption about Image size
		extern const std::string ImageIndexError; 		///< trying to access non-existing element

		// MathLink errors:
		extern const std::string MLTestHeadError; 		///< MLTestHead failed (wrong head or number of arguments)
		extern const std::string MLPutSymbolError; 		///< MLPutSymbol failed
		extern const std::string MLPutFunctionError; 	///< MLPutFunction failed
		extern const std::string MLTestSymbolError;		///< MLTestSymbol failed (different symbol on the link than expected)
		extern const std::string MLWrongSymbolForBool;	///< Tried to read something else than "True" or "False" as boolean
		extern const std::string MLGetListError;		///< Could not get list from MathLink
		extern const std::string MLGetScalarError;		///< Could not get scalar from MathLink
		extern const std::string MLGetStringError;		///< Could not get string from MathLink
		extern const std::string MLGetArrayError;		///< Could not get array from MathLink
		extern const std::string MLPutListError;		///< Could not send list via MathLink
		extern const std::string MLPutScalarError;		///< Could not send scalar via MathLink
		extern const std::string MLPutStringError;		///< Could not send string via MathLink
		extern const std::string MLPutArrayError;		///< Could not send array via MathLink
		extern const std::string MLGetSymbolError; 		///< MLGetSymbol failed
		extern const std::string MLGetFunctionError; 	///< MLGetFunction failed
		extern const std::string MLPacketHandleError;	///< One of the packet handling functions failed
		extern const std::string MLFlowControlError;		///< One of the flow control functions failed
		extern const std::string MLTransferToLoopbackError;	///< Something went wrong when transferring expressions from loopback link
		extern const std::string MLCreateLoopbackError;		///< Could not create a new loopback link
		extern const std::string MLLoopbackStackSizeError;	///< Loopback stack size too small to perform desired action

		// DataList errors:
		extern const std::string DLNullRawNode;			///< DataStoreNode passed to Node wrapper was null
		extern const std::string DLInvalidNodeType;		///< DataStoreNode passed to Node wrapper carries data of invalid type
		extern const std::string DLGetNodeDataError;	///< DataStoreNode_getData failed
		extern const std::string DLNullRawDataStore;    ///< DataStore passed to DataList was null
		extern const std::string DLPushBackTypeError;   ///< Element to be added to the DataList has incorrect type

		// MArgument errors:
		extern const std::string ArgumentCreateNull;    	///< Trying to create Argument object from nullptr
		extern const std::string ArgumentAddNodeMArgument;  ///< Trying to add DataStore Node of type MArgument (aka MType_Undef)
	}

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

		/**
		 * Set debug info
		 * @param dbg - additional information helpful in debugging
		 */
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

		/**
		 *   @brief Get debug info
		 **/
		const std::string& debug() const noexcept {
			return debugInfo;
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
	 * ErrorManager holds a map with all errors that may be thrown from paclet code. These are: LLU errors, framework errors (e.g. MDevices)
	 * and paclet-specific errors which should be registered (for example in WolframLibrary_initialize) using registerPacletErrors function.
	 * Developers must never throw LibraryLinkErrors directly, instead they should use one of ErrorManager::throwException overloads.
	 **/
	class ErrorManager {
	public:
		using ErrorStringData = std::pair<std::string, std::string>;

	public:

		/**
		 * @brief Default constructor is deleted since ErrorManager is supposed to be completely static
		 */
		ErrorManager() = delete;

		/**
		 * @brief 	Function used to register paclet-specific errors.
		 * @param 	errors - a list of pairs: {"ErrorName", "Short string with error description"}
		 */
		static void registerPacletErrors(const std::vector<ErrorStringData>& errors);

		/**
		 * @brief 	Throw exception with given name.
		 * @param 	errorName - name of the error that will be thrown
		 */
		[[noreturn]] static void throwException(const std::string& errorName);

		/**
		 * @brief 	Throw exception with given name and some additional debug information.
		 * @param 	errorName - name of the error that will be thrown
		 * @param	debugInfo - additional message with debug info, this message will not be passed to top-level Failure object
		 */
		[[noreturn]] static void throwException(const std::string& errorName, const std::string& debugInfo);

		/**
		 * @brief 	Throw exception of given class that carries the error with given name.
		 *
		 * This is useful if you want to throw custom exception classes from your paclet and still see the nice Failure objects in top-level.
		 *
		 * @tparam	Error - custom exception class it must define a constructor that takes a LibraryLinkError as first parameter
		 * but it doesn't have to derive from LibraryLinkError
		 * @param 	errorName - name of error to be thrown
		 * @param 	args - additional arguments that will be perfectly forwarded to the constructor of Error class
		 */
		template<class Error, typename... Args>
		[[noreturn]] static void throwCustomException(const std::string& errorName, Args&&... args);

		/**
		 * @brief Function used to send all registered errors to top-level Mathematica code.
		 *
		 * Sending registered errors allows for nice and meaningful Failure objects to be generated when paclet function fails in top level,
		 * instead of usual LibraryFunctionError expressions.
		 * @param mlp - active MathLink connection
		 */
		static void sendRegisteredErrorsViaMathlink(MLINK mlp);

	private:

		/// Errors are stored in a map with elements of the form { "ErrorName", immutable LibraryLinkError object }
		using ErrorMap = std::unordered_map<std::string, const LibraryLinkError>;

	private:
		/**
		 * @brief 	Use this function to add new entry to the map of registered errors.
		 * @param 	errorData - a pair of strings: error name + error description
		 */
		static void set(const ErrorStringData& errorData);

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

		/***
		 * @brief Initialization of static error map
		 * @param initList - list of errors used internally by LLU
		 * @return reference to static error map
		 */
		static ErrorMap registerLLUErrors(std::initializer_list<ErrorStringData> initList);

		/// Static map of registered errors
		static ErrorMap& errors();

		/// Id that will be assigned to the next registered error.
		static int& nextErrorId();
	};

	template<class Error, typename... Args>
	void ErrorManager::throwCustomException(const std::string& errorName, Args&&... args) {
		throw Error(findError(errorName), std::forward<Args>(args)...);
	}

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_LIBRARYLINKERROR_H_ */
