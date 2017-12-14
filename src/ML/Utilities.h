/** 
 * @file	Utilities.h
 * @date	Nov 26, 2017
 * @author	rafalc
 * @brief	<brief description>
 */
#ifndef LLUTILS_ML_UTILITIES_H_
#define LLUTILS_ML_UTILITIES_H_

#include <string>

#include "mathlink.h"
#include "../LibraryLinkError.h"

namespace LibraryLinkUtils {

	namespace LLErrorCode {
		// MathLink errors: [-501 : -600]
		constexpr int MLTestHeadError = -501; 		///< MLTestHead failed (wrong head or number of arguments)
		constexpr int MLPutSymbolError = -502; 		///< MLPutSymbol failed
		constexpr int MLPutFunctionError = -503; 	///< MLPutFunction failed
		constexpr int MLTestSymbolError = -504;		///< MLTestSymbol failed (different symbol on the link than expected)
		constexpr int MLWrongSymbolForBool = -505;	///< Tried to read something else than "True" or "False" as boolean
		constexpr int MLGetListError = -506;		///< Could not get list from MathLink
		constexpr int MLGetScalarError = -507;		///< Could not get scalar from MathLink
		constexpr int MLGetStringError = -508;		///< Could not get string from MathLink
		constexpr int MLGetArrayError = -509;		///< Could not get array from MathLink
		constexpr int MLPutListError = -510;		///< Could not send list via MathLink
		constexpr int MLPutScalarError = -511;		///< Could not send scalar via MathLink
		constexpr int MLPutStringError = -512;		///< Could not send string via MathLink
		constexpr int MLPutArrayError = -513;		///< Could not send array via MathLink
		constexpr int MLGetSymbolError = -514; 		///< MLGetSymbol failed
		constexpr int MLGetFunctionError = -515; 	///< MLGetFunction failed
	}

	class MathLinkStream;

	namespace ML {

		void checkError(MLINK m, int statusOk, int errorCode, const std::string& debugInfo = "");
		void checkError(MLINK m, int statusOk, const std::string& errorName, const std::string& debugInfo = "");

		struct Symbol {
			Symbol() = default;
			Symbol(std::string h) : head(std::move(h)) {};

			const std::string& getHead() const;
			void setHead(std::string h);
		private:
			std::string head;
		};

		struct Function : Symbol {
			Function() : Function("", -1) {};
			Function(const std::string& h) : Function(h, -1) {}
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


		MathLinkStream& NewPacket(MathLinkStream& ms);

		MathLinkStream& EndPacket(MathLinkStream& ms);

		MathLinkStream& Flush(MathLinkStream& ms);

		MathLinkStream& Rule(MathLinkStream& ms, Direction dir);
	}
}

#endif /* LLUTILS_ML_UTILITIES_H_ */
