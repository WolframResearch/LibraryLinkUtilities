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

#include "LLU/LibraryLinkError.h"
#include "EncodingTraits.hpp"

namespace LibraryLinkUtils {

	template<ML::Encoding, ML::Encoding>
	class MLStream;

	/**
	 * @brief Contains definitions related to MathLink functionality in LLU
	 */
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
		 * @brief	Simple wrapper over ErrorManager::throwException used to break dependency cycle between MLStream and ErrorManager.
		 * @param 	errorName - what error name to put in the exception
		 * @param 	debugInfo - additional info to be attached to the exception
		 */
		[[noreturn]] void throwLLUException(const std::string& errorName, const std::string& debugInfo = "");

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

		template<ML::Encoding EIn, ML::Encoding EOut>
		MLStream<EIn, EOut>& NewPacket(MLStream<EIn, EOut>& ms) {
			checkError(
				ms.get(),
				MLNewPacket(ms.get()),
				LLErrorName::MLPacketHandleError,
				"Error in MLNewPacket"
			);
			return ms;
		}

		template<ML::Encoding EIn, ML::Encoding EOut>
		MLStream<EIn, EOut>& EndPacket(MLStream<EIn, EOut>& ms) {
			checkError(
				ms.get(),
				MLEndPacket(ms.get()),
				LLErrorName::MLPacketHandleError,
				"Error in MLEndPacket"
			);
			return ms;
		}

		template<ML::Encoding EIn, ML::Encoding EOut>
		MLStream<EIn, EOut>& Flush(MLStream<EIn, EOut>& ms) {
			checkError(
				ms.get(),
				MLFlush(ms.get()),
				LLErrorName::MLFlowControlError,
				"Error in MLFlush"
			);
			return ms;
		}

		template<ML::Encoding EIn, ML::Encoding EOut>
		MLStream<EIn, EOut>& Rule(MLStream<EIn, EOut>& ms, Direction dir) {
			if (dir == Direction::Put)
				return ms << Function("Rule", 2);
			else
				return ms >> Function("Rule", 2);
		}

		template<ML::Encoding EIn, ML::Encoding EOut>
		MLStream<EIn, EOut>& Null(MLStream<EIn, EOut>& ms, Direction dir) {
			if (dir == Direction::Put)
				return ms << Symbol("Null");
			else
				return ms >> Symbol("Null");
		}


		/**
		 * @struct BeginExpr
		 *
		 * Simple structure to indicate that we will be sending an expression which length is not known beforehand
		 */
		struct BeginExpr : Symbol {
			explicit BeginExpr(const std::string& head) : Symbol(head) {}
		};

		/**
		 * @struct DropExpr
		 *
		 * An empty structure to indicate that current expression started with BeginExpr should be immediately discarded
		 */
		struct DropExpr {};

		/**
		 * @struct EndExpr
		 *
		 * An empty structure to indicate that current expression started with BeginExpr has ended and we can forward it to the "parent" link
		 */
		struct EndExpr {};


		/**
		 * @brief	Returns a new loopback link using MLLinkEnvironment(m) as MENV
		 * @param 	m - valid MLINK
		 * @return 	a brand new Loopback Link
		 */
		MLINK getNewLoopback(MLINK m);

		/**
		 * @brief	Get the number of expressions stored in the loopback link
		 * @param	lpbckLink - a reference to the loopback link, after expressions are counted this argument will be assigned a different MLINK
		 * @return	a number of expression stored in the loopback link
		 */
		int countExpressionsInLoopbackLink(MLINK& lpbckLink);
	}
}

#endif /* LLUTILS_ML_UTILITIES_H_ */
