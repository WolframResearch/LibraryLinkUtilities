/**
 * @file	Utilities.h
 * @date	Nov 26, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Header file with miscellaneous utilities used throughout the WSTP-related part of LibraryLinkUtilities
 */
#ifndef LLU_WSTP_UTILITIES_H_
#define LLU_WSTP_UTILITIES_H_

#include <cstring>
#include <string>

#include "wstp.h"

#include "LLU/ErrorLog/Errors.h"

#include "EncodingTraits.hpp"

namespace LLU {

	template<WS::Encoding, WS::Encoding>
	class WSStream;

	/**
	 * @brief Contains definitions related to WSTP functionality in LLU
	 */
	namespace WS {

		/**
		 * @brief 		Checks if WSTP operation was successful and throws appropriate exception otherwise
		 * @param[in] 	m - low-level object of type WSLINK received from LibraryLink
		 * @param[in] 	statusOk - status code return from a WSTP function
		 * @param[in] 	errorName - what error name to put in the exception if WSTP function failed
		 * @param[in] 	debugInfo - additional info to be attached to the exception
		 */
		void checkError(WSLINK m, int statusOk, const std::string& errorName, const std::string& debugInfo = "");

		/**
		 * @brief	Simple wrapper over ErrorManager::throwException used to break dependency cycle between WSStream and ErrorManager.
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
			 * The head is empty and such Symbol can be used as argument of WSStream::operator>>, so to read a Symbol from top-level.
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
			 * This constructor should only be used to create a Function right before calling WSStream::operator>> on it, which will read a function from
			 * top-level.
			 */
			Function() : Function("", -1) {};

			/**
			 * @brief Create a Function with given head but unknown number of arguments.
			 * You can later call WSStream::operator>> on such constructed Function to populate argument count with a value read from top-level.
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
            explicit Association(int argCount) : Function("Association", argCount) {}
		};

		/**
		 * @struct	List
		 * Special case of a Function.
		 */
		struct List : Function {
			List() : Function("List") {}
            explicit List(int argCount) : Function("List", argCount) {}
		};

		/**
		 * @struct	Missing
		 * Represents WL expression Missing["reason"].
		 */
		struct Missing : Function {
			Missing() : Function("Missing") {}
			explicit Missing(std::string r) : Function("Missing", 1), reason(std::move(r)) {}

			const std::string& why() const {
				return reason;
			}

		private:
			std::string reason;
		};

		enum class Direction : bool { Get, Put };

		template<WS::Encoding EIn, WS::Encoding EOut>
		WSStream<EIn, EOut>& NewPacket(WSStream<EIn, EOut>& ms) {
			checkError(ms.get(), WSNewPacket(ms.get()), ErrorName::WSPacketHandleError, "Error in WSNewPacket");
			return ms;
		}

		template<WS::Encoding EIn, WS::Encoding EOut>
		WSStream<EIn, EOut>& EndPacket(WSStream<EIn, EOut>& ms) {
			checkError(ms.get(), WSEndPacket(ms.get()), ErrorName::WSPacketHandleError, "Error in WSEndPacket");
			return ms;
		}

		template<WS::Encoding EIn, WS::Encoding EOut>
		WSStream<EIn, EOut>& Flush(WSStream<EIn, EOut>& ms) {
			checkError(ms.get(), WSFlush(ms.get()), ErrorName::WSFlowControlError, "Error in WSFlush");
			return ms;
		}

		template<WS::Encoding EIn, WS::Encoding EOut>
		WSStream<EIn, EOut>& Rule(WSStream<EIn, EOut>& ms, Direction dir) {
			if (dir == Direction::Put)
				return ms << Function("Rule", 2);
			else
				return ms >> Function("Rule", 2);
		}

		template<WS::Encoding EIn, WS::Encoding EOut>
		WSStream<EIn, EOut>& Null(WSStream<EIn, EOut>& ms, Direction dir) {
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
		 * @brief	Returns a new loopback link using WSLinkEnvironment(m) as MENV
		 * @param 	m - valid WSLINK
		 * @return 	a brand new Loopback Link
		 */
		WSLINK getNewLoopback(WSLINK m);

		/**
		 * @brief	Get the number of expressions stored in the loopback link
		 * @param	lpbckLink - a reference to the loopback link, after expressions are counted this argument will be assigned a different WSLINK
		 * @return	a number of expression stored in the loopback link
		 */
		int countExpressionsInLoopbackLink(WSLINK& lpbckLink);
	}
}

#endif /* LLU_WSTP_UTILITIES_H_ */
