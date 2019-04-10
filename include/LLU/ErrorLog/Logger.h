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

#include "LLU/Containers/LibDataHolder.h"
#include "LLU/ML/MLStream.hpp"


#ifdef LLU_LOG_DEBUG
#define LLU_LOG_LEVEL_DEBUG
#define LLU_LOG_WARNING
#endif

#ifdef LLU_LOG_WARNING
#define LLU_LOG_LEVEL_WARNING
#define LLU_LOG_ERROR
#endif

#ifdef LLU_LOG_ERROR
#define LLU_LOG_LEVEL_ERROR
#endif

#ifdef LLU_LOG_LEVEL_DEBUG
#define LLU_DEBUG(...) LibraryLinkUtils::Logger::log<LibraryLinkUtils::Logger::Level::Debug>(__LINE__, __FILE__, __func__, __VA_ARGS__)
#else
#define LLU_DEBUG(...) ((void)0)
#endif

#ifdef LLU_LOG_LEVEL_WARNING
#define LLU_WARNING(...) LibraryLinkUtils::Logger::log<LibraryLinkUtils::Logger::Level::Warning>(__LINE__, __FILE__, __func__, __VA_ARGS__)
#else
#define LLU_WARNING(...) ((void)0)
#endif

#ifdef LLU_LOG_LEVEL_ERROR
#define LLU_ERROR(...) LibraryLinkUtils::Logger::log<LibraryLinkUtils::Logger::Level::Error>(__LINE__, __FILE__, __func__, __VA_ARGS__)
#else
#define LLU_ERROR(...) ((void)0)
#endif

namespace LibraryLinkUtils {

	namespace Logger {

		constexpr const char* TopLevelLogCallback = "LLU`Logger`Log";

		enum class Level {
			Debug,
			Warning,
			Error
		};

		/**
		 * Converts Logger::Level value to std::string
		 * @param l - log level
		 * @return a string representation of the input log level
		 */
		std::string to_string(Level l);

		/**
		 * Sends a Logger::Level value via MLStream
		 * @tparam 	EIn - MLStream input encoding
		 * @tparam 	EOut - MLStream output encoding
		 * @param 	ms - reference to the MLStream object
		 * @param 	l - log level
		 * @return	reference to the input stream
		 */
		template<ML::Encoding EIn, ML::Encoding EOut>
		MLStream<EIn, EOut>& operator<<(MLStream<EIn, EOut>& ms, Level l) {
			return ms << to_string(l);
		}


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
		template<Logger::Level L, typename... T>
		void log(WolframLibraryData libData, int line, const std::string& fileName, const std::string& function, T&&... args) {
			static std::mutex mlinkGuard;
			if (!libData) {
				return;
			}
			std::lock_guard<std::mutex> lock(mlinkGuard);

			MLStream<ML::Encoding::UTF8> mls { libData->getWSLINK(libData) };
			mls << ML::Function("EvaluatePacket", 1);
			mls << ML::Function(TopLevelLogCallback, 4 + sizeof...(T));
			mls << L << line << fileName << function;
			static_cast<void>(std::initializer_list<int> { (mls << args, 0)... });
			libData->processWSLINK(mls.get());
			auto pkt = MLNextPacket(mls.get());
			if ( pkt == RETURNPKT) {
				mls << ML::NewPacket;
			}
		}

		/**
		 *
		 * @tparam L
		 * @tparam T
		 * @param line
		 * @param fileName
		 * @param function
		 * @param args
		 */
		template<Logger::Level L, typename... T>
		void log(int line, const std::string& fileName, const std::string& function, T&&... args) {
			log<L>(LibDataHolder::getLibraryData(), line, fileName, function, std::forward<T>(args)...);
		}
	} // namespace Logger

}
#endif //LLUTILS_LOGGER_H
