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

		void checkError(MLINK m, int statusOk, int errorCode, const std::string& debugInfo = "");
		void checkError(MLINK m, int statusOk, const std::string& errorName, const std::string& debugInfo = "");

		struct Symbol {
			Symbol() = default;
			explicit Symbol(std::string h) : head(std::move(h)) {};

			const std::string& getHead() const;
			void setHead(std::string h);
		private:
			std::string head;
		};

		struct Function : Symbol {
			Function() : Function("", -1) {};
			explicit Function(const std::string& h) : Function(h, -1) {}
			Function(const std::string& h, int argCount) : Symbol(h), argc(argCount) {}

			int getArgc() const;
			void setArgc(int newArgc);
		private:
			int argc;
		};

		struct Association : Function {
			Association() : Function("Association") {}
			Association(int argCount) : Function("Association", argCount) {}
		};

		struct List : Function {
			List() : Function("List") {}
			List(int argCount) : Function("List", argCount) {}
		};


		enum class Direction : bool {
			Get,
			Put
		};


		template<typename T>
		struct IsSupportedInMLArithmetic;

		template<> struct IsSupportedInMLArithmetic<unsigned char> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<short> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<int> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<mlint64> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<float> : std::true_type {};
		template<> struct IsSupportedInMLArithmetic<double> : std::true_type {};


		template<typename T>
		using ArithmeticTypeQ = std::enable_if_t<IsSupportedInMLArithmetic<remove_cv_ref<T>>::value, int>;

		template<typename T>
		using NotArithmeticTypeQ = std::enable_if_t<!IsSupportedInMLArithmetic<remove_cv_ref<T>>::value, int>;

		template<typename T>
		struct IsSupportedInMLString;

		template<> struct IsSupportedInMLString<char> : std::true_type {};
		template<> struct IsSupportedInMLString<unsigned char> : std::true_type {};
		template<> struct IsSupportedInMLString<unsigned short> : std::true_type {};
		template<> struct IsSupportedInMLString<unsigned int> : std::true_type {};


		template<typename T>
		using StringTypeQ = std::enable_if_t<IsSupportedInMLString<remove_cv_ref<T>>::value, int>;


		MathLinkStream& NewPacket(MathLinkStream& ms);

		MathLinkStream& EndPacket(MathLinkStream& ms);

		MathLinkStream& Flush(MathLinkStream& ms);

		MathLinkStream& Rule(MathLinkStream& ms, Direction dir);
	}
}

#endif /* LLUTILS_ML_UTILITIES_H_ */
