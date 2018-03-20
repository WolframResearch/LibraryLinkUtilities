/** 
 * @file	Utilities.h
 * @date	Nov 26, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Header file with miscellaneous utilities used throughout the MathLink-related part of LibraryLinkUtilities
 */
#ifndef LLUTILS_ML_UTILITIES_H_
#define LLUTILS_ML_UTILITIES_H_

#include <cstring>
#include <string>

#include "mathlink.h"

namespace LibraryLinkUtils {

	class MathLinkStream;

	namespace ML {

		/**
		 * @brief 		Checks if MathLink operation was successful and throws appropriate exception otherwise
		 * @param[in] 	m - low-level object of type MLINK received from LibraryLink
		 * @param[in] 	statusOk - status code return from a MathLink function
		 * @param[in] 	errorName - what error name to put in the exception if MathLink function failed
		 * @param[in] 	debugInfo - additional info to be attached to the exception
		 */
		void checkError(MLINK m, int statusOk, const std::string& errorName, const std::string& debugInfo = "");

		/**
		 * @struct 	Symbol
		 * @brief	Structure representing any symbol in Wolfram Language
		 */
		struct Symbol {
			/**
			 * @brief Default constructor.
			 * The head is empty and such Symbol can be used as argument of MathLinkStream::operator>>, so to read a Symbol from top-level.
			 */
			Symbol() = default;

			/**
			 * @brief	Constructs a Symbol given its head.
			 * @param 	h - head (or Symbol's name)
			 */
			explicit Symbol(std::string h) : head(std::move(h)) {};

			/**
			 * Get Symbol name/head.
			 * @return head
			 */
			const std::string& getHead() const;

			/**
			 * Set Symbol name/head.
			 * @param h - new head for the Symbol
			 */
			void setHead(std::string h);

		private:
			std::string head;
		};

		/**
		 * @struct 	Function
		 * @brief	Structure representing any function in Wolfram Language, i.e. a head plus number of arguments.
		 */
		struct Function : Symbol {

			/**
			 * @brief Default constructor.
			 * The head is empty and number of arguments is set to a dummy value.
			 * This constructor should only be used to create a Function right before calling MathLinkStream::operator>> on it, which will read a function from top-level.
			 */
			Function() : Function("", -1) {};

			/**
			 * @brief Create a Function with given head but unknown number of arguments.
			 * You can later call MathLinkStream::operator>> on such constructed Function to populate argument count with a value read from top-level.
			 * @param h - function head
			 */
			explicit Function(const std::string& h) : Function(h, -1) {}

			/**
			 * @brief	Construct a Function with given head and number of arguments
			 * @param 	h - function head
			 * @param 	argCount - number of arguments this function takes
			 */
			Function(const std::string& h, int argCount) : Symbol(h), argc(argCount) {}

			/**
			 * @brief	Get argument count.
			 * @return 	number of arguments this function takes
			 */
			int getArgc() const;

			/**
			 * @brief Set argument count.
			 * @param newArgc - new value for argument count
			 */
			void setArgc(int newArgc);

		private:
			int argc;
		};

		/**
		 * @struct	Association
		 * Special case of a Function.
		 */
		struct Association : Function {
			Association() : Function("Association") {}
			Association(int argCount) : Function("Association", argCount) {}
		};

		/**
		 * @struct	List
		 * Special case of a Function.
		 */
		struct List : Function {
			List() : Function("List") {}
			List(int argCount) : Function("List", argCount) {}
		};

		/**
		 * @struct	Missing
		 * Represents WL expression Missing["reason"].
		 */
		struct Missing : Function {
			Missing() : Function("Missing") {}
			Missing(std::string r) : Function("Missing", 1), reason(std::move(r)) {}

			const std::string& why() const { return reason; }
		private:
			std::string reason;
		};


		enum class Direction : bool {
			Get,
			Put
		};


		MathLinkStream& NewPacket(MathLinkStream& ms);

		MathLinkStream& EndPacket(MathLinkStream& ms);

		MathLinkStream& Flush(MathLinkStream& ms);

		MathLinkStream& Rule(MathLinkStream& ms, Direction dir);

		MathLinkStream& Null(MathLinkStream& ms, Direction dir);

		/**
		 *	@struct PutAsUTF8
		 *	@brief	Utility structure used to enforce sending given string as UTF8 string via MathLink
		 */
		struct PutAsUTF8 {

			/**
			 * @brief	Take a _const char*_ to be sent with MLPutUTF8String.
			 * @param 	s - C-style string encoded in UTF8
			 *
			 * @warning	You are entirely responsible for making sure that the string is actually UTF8 encoded!
			 */
			explicit PutAsUTF8(const char* s) : str(reinterpret_cast<const unsigned char*>(s)), len(std::strlen(s)) {};

			/**
			 * @brief	Take a _const unsigned char*_ to be sent with MLPutUTF8String.
			 * @param 	s - C-style string encoded in UTF8
			 *
			 * @warning	You are entirely responsible for making sure that the string is actually UTF8 encoded!
			 */
			explicit PutAsUTF8(const unsigned char* s) : str(s), len(std::strlen(reinterpret_cast<const char*>(s))) {};

			/**
			 * @brief	Take a _std::string_ to be sent with MLPutUTF8String.
			 * @param 	s - const reference to a string encoded in UTF8
			 *
			 * @warning	You are entirely responsible for making sure that the string is actually UTF8 encoded!
			 */
			explicit PutAsUTF8(const std::string& s) : str(reinterpret_cast<const unsigned char*>(s.c_str())), len(s.length()) {};

			/**
			 * @brief	Take a _std::basic_string<unsigned char>_ to be sent with MLPutUTF8String.
			 * @param 	s - const reference to a string encoded in UTF8
			 *
			 * @warning	You are entirely responsible for making sure that the string is actually UTF8 encoded!
			 */
			explicit PutAsUTF8(const std::basic_string<unsigned char>& s) : str(s.c_str()), len(s.length()) {};

			/// String data after casting to a type that MLPutUTF8String accepts
			const unsigned char* str;

			/// Length of the input string
			std::size_t len;
		};


		/**
		 *	@struct GetAsUTF8
		 *	@brief	Utility structure to facilitate reading UTF8 strings into std::string
		 */
		struct GetAsUTF8 {

			/**
			 * @brief	Read a string from MathLink with MLGetUTF8String and assign to std::string
			 * @param 	s - reference to a string that will be read from MathLink
			 */
			explicit GetAsUTF8(std::string& s) : str(s) {};


			/// Store a reference to which later a UTF8 string from MathLink will be assigned
			std::string& str;
		};
	}
}

#endif /* LLUTILS_ML_UTILITIES_H_ */
