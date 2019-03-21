/**
 * @file	ErrorManager.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	March 21, 2019
 * @brief	Definition of the ErrorManager class responsible for error registration and throwing.
 */
#ifndef LLUTILS_ERRORMANAGER_H
#define LLUTILS_ERRORMANAGER_H

#include <initializer_list>
#include <string>
#include <vector>
#include <algorithm>
#include <unordered_map>
#include <utility>

#include "mathlink.h"
#include "WolframLibrary.h"

#include <LLU/Containers/LibDataHolder.h>
#include <LLU/LibraryLinkError.h>
#include <LLU/ML/MLStream.hpp>

namespace LibraryLinkUtils {

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
		 * @brief	Throw exception with given name.
		 * 			Optionally, send arbitrary details of the exception occurrence to top-level. This will send any number of arguments via MathLink
		 * 			as a List and assign this List to the symbol specified in ErrorManager::exceptionDetailsSymbol.
		 * @tparam 	T - type template parameter pack
		 * @param 	errorName - name of error to be thrown, must be registered beforehand
		 * @param 	args - any number of arguments that will replace TemplateSlots (``, `1`, `xx`, etd) in the message text in top-level
		 * @note	This function requires a copy of WolframLibraryData to be saved in WolframLibrary_initialize via LibDataHolder::setLibraryData
		 * 			or MArgumentManager::setLibraryData.
		 */
		template<typename... T>
		[[noreturn]] static void throwException(const std::string& errorName, T&&... args);

		/**
		 * @brief	Throw exception with given name.
		 * 			Optionally, send arbitrary details of the exception occurrence to top-level. This will send any number of arguments via MathLink
		 * 			as a List and assign this List to the symbol specified in ErrorManager::exceptionDetailsSymbol.
		 * @tparam 	T - type template parameter pack
		 * @param	libData - a copy of WolframLibraryData which should be used to extract the MLink for MathLink connection
		 * @param 	errorName - name of error to be thrown, must be registered beforehand
		 * @param 	args - any number of arguments that will replace TemplateSlots (``, `1`, `xx`, etd) in the message text in top-level
		 */
		template<typename... T>
		[[noreturn]] static void throwException(WolframLibraryData libData, const std::string& errorName, T&&... args);

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
		 * @brief	Throw exception with given name.
		 * 			Optionally, send arbitrary details of the exception occurrence to top-level. This will send any number of arguments via MathLink
		 * 			as a List and assign this List to the symbol specified in ErrorManager::exceptionDetailsSymbol.
		 * @tparam 	T - type template parameter pack
		 * @param 	errorName - name of error to be thrown, must be registered beforehand
		 * @param	debugInfo - additional message with debug info, this message will not be passed to top-level Failure object
		 * @param 	args - any number of arguments that will replace TemplateSlots (``, `1`, `xx`, etd) in the message text in top-level
		 * @note	This function requires a copy of WolframLibraryData to be saved in WolframLibrary_initialize via LibDataHolder::setLibraryData
		 * 			or MArgumentManager::setLibraryData.
		 */
		template<typename... T>
		[[noreturn]] static void throwExceptionWithDebugInfo(const std::string& errorName, const std::string& debugInfo, T&&... args);

		/**
		 * @brief	Throw exception with given name.
		 * 			Optionally, send arbitrary details of the exception occurrence to top-level. This will send any number of arguments via MathLink
		 * 			as a List and assign this List to the symbol specified in ErrorManager::exceptionDetailsSymbol.
		 * @tparam 	T - type template parameter pack
		 * @param	libData - a copy of WolframLibraryData which should be used to extract the MLink for MathLink connection
		 * @param 	errorName - name of error to be thrown, must be registered beforehand
		 * @param	debugInfo - additional message with debug info, this message will not be passed to top-level Failure object
		 * @param 	args - any number of arguments that will replace TemplateSlots (``, `1`, `xx`, etd) in the message text in top-level
		 */
		template<typename... T>
		[[noreturn]] static void throwExceptionWithDebugInfo(WolframLibraryData libData, const std::string& errorName, const std::string& debugInfo, T&&... args);

		/**
		 * @brief Function used to send all registered errors to top-level Mathematica code.
		 *
		 * Sending registered errors allows for nice and meaningful Failure objects to be generated when paclet function fails in top level,
		 * instead of usual LibraryFunctionError expressions.
		 * @param mlp - active MathLink connection
		 */
		static void sendRegisteredErrorsViaMathlink(MLINK mlp);

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

		/**
		 * @brief	Send a number of failure parameters to top-level.
		 * 			They will be assigned in a List to symbol whose name is stored in ErrorManager::exceptionDetailsSymbol
		 * @tparam 	T - any type that MLStream supports
		 * @param 	libData - WolframLibraryData, if nullptr, the parameters will not be send
		 * @param 	args - parameters of an exception currently being thrown
		 */
		template<typename... T>
		static void sendExceptionParameters(WolframLibraryData libData, T&&... args);

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

		/// A WL symbol that will hold the details of last thrown exception.
		static std::string exceptionDetailsSymbol;
	};

	template<typename... T>
	void ErrorManager::sendExceptionParameters(WolframLibraryData libData, T&&... args) {
		constexpr auto argCount = sizeof...(T);
		if (libData) {
			MLStream<ML::Encoding::UTF8> mls { libData->getWSLINK(libData) };
			mls << ML::Function("EvaluatePacket", 1);
			mls << ML::Function("Set", 2);
			mls << ML::Symbol(exceptionDetailsSymbol);
			mls << ML::List(argCount);
			static_cast<void>(std::initializer_list<int> { (mls << args, 0)... });
			libData->processWSLINK(mls.get());
			auto pkt = MLNextPacket(mls.get());
			if ( pkt == RETURNPKT) {
				mls << ML::NewPacket;
			}
		}
	}

	template<typename... T>
	void ErrorManager::throwException(const std::string& errorName, T&&... args) {
		throwException(LibDataHolder::getLibraryData(), errorName, std::forward<T>(args)...);
	}

	template<typename... T>
	void ErrorManager::throwException(WolframLibraryData libData, const std::string& errorName, T&&... args) {
		sendExceptionParameters(libData, std::forward<T>(args)...);
		throw findError(errorName);
	}

	template<class Error, typename... Args>
	void ErrorManager::throwCustomException(const std::string& errorName, Args&&... args) {
		throw Error(findError(errorName), std::forward<Args>(args)...);
	}

	template<typename... T>
	void ErrorManager::throwExceptionWithDebugInfo(const std::string& errorName, const std::string& debugInfo, T&&... args) {
		throwExceptionWithDebugInfo(LibDataHolder::getLibraryData(), errorName, debugInfo, std::forward<T>(args)...);
	}

	template<typename... T>
	void ErrorManager::throwExceptionWithDebugInfo(WolframLibraryData libData, const std::string& errorName, const std::string& debugInfo, T&&... args) {
		auto exception = findError(errorName);
		exception.setDebugInfo(debugInfo);
		sendExceptionParameters(libData, std::forward<T>(args)...);
		throw exception;
	}

}  /* namespace LibraryLinkUtils */

#endif //LLUTILS_ERRORMANAGER_H
