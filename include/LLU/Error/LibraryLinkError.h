/**
 * @file	LibraryLinkError.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 *
 * @brief	Error class and error codes used by LibraryLink Utilities classes
 *
 */
#ifndef LLUTILS_LIBRARYLINKERROR_H_
#define LLUTILS_LIBRARYLINKERROR_H_

#include <stdexcept>
#include <string>

#include "LLU/ML/MLStream.hpp"

/**
 * @namespace LibraryLinkUtils
 * @brief Main namespace of LibraryLinkUtilities
 */
namespace LibraryLinkUtils {

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

		LibraryLinkError(const LibraryLinkError& e);

		LibraryLinkError(LibraryLinkError&& e) noexcept : std::runtime_error(e), errorId(e.errorId), type(e.type), messageTemplate(e.messageTemplate),
														  debugInfo(e.debugInfo), messageParams(e.messageParams) {
			e.messageParams = nullptr;
		}

		~LibraryLinkError();

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

		/**
		 * @brief	Store arbitrary number of message parameters in a List expression on a loopback link.
		 * 			They will travel with the exception until \c sendParamaters is called on the exception.
		 * @tparam 	T - any type(s) that MLStream supports
		 * @param 	libData - WolframLibraryData, if nullptr, the parameters will not be send
		 * @param 	params - any number of message parameters
		 */
		template<typename... T>
		void setMessageParameters(WolframLibraryData libData, T&&... params);

		/**
		 * @brief	Send parameters stored in the loopback link to top-level.
		 * 			They will be assigned as a List to symbol passed in \p WLSymbol parameter.
		 * @param 	libData - WolframLibraryData, if nullptr, the parameters will not be send
		 * @param	WLSymbol - symbol to assign parameters to in top-level
		 */
		int sendParameters(WolframLibraryData libData, const std::string& WLSymbol = exceptionDetailsSymbol) const noexcept;

		/**
		 * @brief	Set custom Wolfram Language symbol that will hold the details of last thrown exception.
		 * @param 	newSymbol - a Wolfram Language symbol name
		 * @note 	This function does not check if \p newSymbol is actually a valid Wolfram Language symbol which is not Protected.
		 */
		static void setExceptionDetailsSymbol(std::string newSymbol);

		/**
		 * @brief	Get current symbol that will hold the details of last thrown exception.
		 * @return	a WL symbol that will hold the details of last thrown exception
		 */
		static const std::string& getExceptionDetailsSymbol();

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

		/**
		 * @brief	Helper functions that opens a loopback link given a MathLink environment
		 * @param 	env - MathLink environment
		 * @return 	a loopback link (may be nullptr if function failed to create the link)
		 */
		static MLINK openLoopback(MLENV env);

		/// A WL symbol that will hold the details of last thrown exception.
		static std::string exceptionDetailsSymbol;

		const IdType errorId;
		const std::string type;
		const std::string messageTemplate;
		std::string debugInfo;
		MLINK messageParams = nullptr;
	};


	template<typename... T>
	void LibraryLinkError::setMessageParameters(WolframLibraryData libData, T&&... params) {
		messageParams = openLoopback(libData->getWSLINKEnvironment(libData));
		if (!messageParams) {
			return;
		}
		MLStream<ML::Encoding::UTF8> loopback { messageParams };
		auto messageParamsCount = sizeof...(T);
		loopback << ML::List(static_cast<int>(messageParamsCount));
		static_cast<void>(std::initializer_list<int> { (loopback << params, 0)... });
	}
} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_LIBRARYLINKERROR_H_ */
