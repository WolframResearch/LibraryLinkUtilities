/** 
 * @file	Utilities.h
 * @date	Nov 26, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Header file with miscellaneous utilities used throughout the MathLink-related part of LibraryLinkUtilities
 */
#ifndef LLUTILS_ML_UTILITIES_H_
#define LLUTILS_ML_UTILITIES_H_

#include <string>
#include <type_traits>

#include "mathlink.h"
#include "../Utilities.hpp"

namespace LibraryLinkUtils {

	class MathLinkStream;

	namespace ML {

		/**
		 * @brief 		Checks if MathLink operation was successful and throws appropriate exception otherwise
		 * @param[in] 	m - low-level object of type MLINK received from LibraryLink
		 * @param[in] 	statusOk - status code return from a MathLink function
		 * @param[in] 	errorCode - which error code to put in the exception if MathLink function failed
		 * @param[in] 	debugInfo - additional info to be attached to the exception
		 */
		void checkError(MLINK m, int statusOk, int errorCode, const std::string& debugInfo = "");

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


		enum class Direction : bool {
			Get,
			Put
		};


		template<typename T>
		struct IsSupportedInMLArithmetic : std::false_type {};

		template<> struct IsSupportedInMLArithmetic<unsigned char> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<short> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<int> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<mlint64> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<float> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<double> : std::true_type {};


		template<typename T, typename U = MathLinkStream&>
		using ScalarSupportedTypeQ = std::enable_if_t<IsSupportedInMLArithmetic<remove_cv_ref<T>>::value, U>;

		template<typename T, typename U = MathLinkStream&>
		using NotScalarSupportedTypeQ = std::enable_if_t<!IsSupportedInMLArithmetic<remove_cv_ref<T>>::value, U>;

		template<typename T, typename U = MathLinkStream&>
		using ScalarNotSupportedTypeQ = std::enable_if_t<std::is_arithmetic<T>::value && !IsSupportedInMLArithmetic<remove_cv_ref<T>>::value, U>;

		template<typename T>
		struct IsSupportedInMLString : std::false_type {};

		template<> struct IsSupportedInMLString<char> : std::true_type {};
		template<> struct IsSupportedInMLString<unsigned char> : std::true_type {};
		template<> struct IsSupportedInMLString<unsigned short> : std::true_type {};
		template<> struct IsSupportedInMLString<unsigned int> : std::true_type {};


		template<typename T, typename U = MathLinkStream&>
		using StringTypeQ = std::enable_if_t<IsSupportedInMLString<remove_cv_ref<T>>::value, U>;


		MathLinkStream& NewPacket(MathLinkStream& ms);

		MathLinkStream& EndPacket(MathLinkStream& ms);

		MathLinkStream& Flush(MathLinkStream& ms);

		MathLinkStream& Rule(MathLinkStream& ms, Direction dir);
	}
}

#endif /* LLUTILS_ML_UTILITIES_H_ */
