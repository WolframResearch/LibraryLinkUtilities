/**
 * @file	Logger.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Define Logger namespace containing logging related declarations and convenience macros.
 */
#ifndef LLUTILS_LOGGER_H
#define LLUTILS_LOGGER_H

#include <initializer_list>
#include <mutex>
#include <string>
#include <utility>

#include "LLU/LibraryData.h"
#include "LLU/ML/MLStream.hpp"

// "Public" macros:

/// Define LLU_LOG_DEBUG to enable all log levels
#ifdef LLU_LOG_DEBUG
#define LLU_LOG_LEVEL_DEBUG
#define LLU_LOG_WARNING
#endif

/// Define LLU_LOG_WARNING to enable warning and error logs. Debug logs will be ignored.
#ifdef LLU_LOG_WARNING
#define LLU_LOG_LEVEL_WARNING
#define LLU_LOG_ERROR
#endif

/// Define LLU_LOG_ERROR to enable only error logs. Debug and warning logs will be ignored.
#ifdef LLU_LOG_ERROR
#define LLU_LOG_LEVEL_ERROR
#endif

// "Private" macros:

#ifdef LLU_LOG_LEVEL_DEBUG
#define LLU_DEBUG(...) LLU::Logger::log<LLU::Logger::Level::Debug>(__LINE__, __FILE__, __func__, __VA_ARGS__)
#else
#define LLU_DEBUG(...) ((void)0)
#endif

#ifdef LLU_LOG_LEVEL_WARNING
#define LLU_WARNING(...) LLU::Logger::log<LLU::Logger::Level::Warning>(__LINE__, __FILE__, __func__, __VA_ARGS__)
#else
#define LLU_WARNING(...) ((void)0)
#endif

#ifdef LLU_LOG_LEVEL_ERROR
#define LLU_ERROR(...) LLU::Logger::log<LLU::Logger::Level::Error>(__LINE__, __FILE__, __func__, __VA_ARGS__)
#else
#define LLU_ERROR(...) ((void)0)
#endif

namespace LLU {

	/**
	 * Logger class is responsible for sending log messages via MathLink to Mathematica.
	 * It may be more convenient to use one of the LLU_DEBUG/WARNING/ERROR macros instead of calling Logger methods directly.
	 */
	class Logger {
	public:
		/// Possible log severity levels
		enum class Level { Debug, Warning, Error };

		/**
		 * @brief	Send a log message of given severity.
		 * @tparam 	L - log level, severity of the log
		 * @tparam 	T - any number of mathlink-supported types
		 * @param 	libData - WolframLibraryData, if nullptr - no logging happens
		 * @param 	line - line number where the log was called
		 * @param 	fileName - name of the file in which the log was called
		 * @param 	function - function in which the log was called
		 * @param 	args - additional parameters carrying the actual log message contents
		 * @warning This function communicates with MathLink and if this communication goes wrong, MLStream may throw
		 * 			so be careful when logging in destructors.
		 */
		template<Level L, typename... T>
		static void log(WolframLibraryData libData, int line, const std::string& fileName, const std::string& function, T&&... args);

		/**
		 * @brief	Send a log message of given severity.
		 * @tparam 	L - log level, severity of the log
		 * @tparam 	T - any number of mathlink-supported types
		 * @param 	line - line number where the log was called
		 * @param 	fileName - name of the file in which the log was called
		 * @param 	function - function in which the log was called
		 * @param 	args - additional parameters carrying the actual log message contents
		 * @warning This function communicates with MathLink and if this communication goes wrong, MLStream may throw
		 * 			so be careful when logging in destructors.
		 */
		template<Level L, typename... T>
		static void log(int line, const std::string& fileName, const std::string& function, T&&... args);

		/**
		 * Converts Logger::Level value to std::string
		 * @param l - log level
		 * @return a string representation of the input log level
		 */
		static std::string to_string(Level l);

		/**
		 * Set new context for the top-level symbol that will handle logging.
		 * @param context - new context, must end with "`"
		 */
		static void setContext(std::string context) {
			logSymbolContext = std::move(context);
		}

		/**
		 * Get the top-level symbol with full context, to which all logs are sent
		 * @return top-level symbol to which logs are sent
		 */
		static std::string getSymbol() {
			return logSymbolContext + topLevelLogCallback;
		}

	private:
		/// Name of the WL function, to which log elements will be sent as arguments via MathLink.
		static constexpr const char* topLevelLogCallback = "Logger`LogHandler";
		static std::mutex mlinkGuard;
		static std::string logSymbolContext;
	};

	/**
	 * Sends a Logger::Level value via MLStream
	 * @tparam 	EIn - MLStream input encoding
	 * @tparam 	EOut - MLStream output encoding
	 * @param 	ms - reference to the MLStream object
	 * @param 	l - log level
	 * @return	reference to the input stream
	 */
	template<ML::Encoding EIn, ML::Encoding EOut>
	static MLStream<EIn, EOut>& operator<<(MLStream<EIn, EOut>& ms, Logger::Level l) {
		return ms << Logger::to_string(l);
	}

	template<Logger::Level L, typename... T>
	void Logger::log(WolframLibraryData libData, int line, const std::string& fileName, const std::string& function, T&&... args) {
		if (!libData) {
			return;
		}
		std::lock_guard<std::mutex> lock(mlinkGuard);

		MLStream<ML::Encoding::UTF8> mls {libData->getWSLINK(libData)};
		mls << ML::Function("EvaluatePacket", 1);
		mls << ML::Function(getSymbol(), 4 + sizeof...(T));
		mls << L << line << fileName << function;
		static_cast<void>(std::initializer_list<int> {(mls << args, 0)...});
		libData->processWSLINK(mls.get());
		auto pkt = MLNextPacket(mls.get());
		if (pkt == RETURNPKT) {
			mls << ML::NewPacket;
		}
	}

	template<Logger::Level L, typename... T>
	void Logger::log(int line, const std::string& fileName, const std::string& function, T&&... args) {
		log<L>(LibraryData::API(), line, fileName, function, std::forward<T>(args)...);
	}

}
#endif	  // LLUTILS_LOGGER_H
