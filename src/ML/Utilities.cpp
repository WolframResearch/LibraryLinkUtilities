/** 
 * @file	Utilities.h
 * @date	Nov 26, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Implementation file with miscellaneous definitions used throughout the MathLink-related part of LibraryLinkUtilities
 */

#include "Utilities.h"

#include <string>

#include "mathlink.h"
#include "MathLinkStream.h"
#include "../LibraryLinkError.h"

namespace LibraryLinkUtils {

	namespace ML {

		const std::string& Symbol::getHead() const {
			return head;
		}

		void Symbol::setHead(std::string h) {
			head = std::move(h);
		}

		int Function::getArgc() const {
			return argc;
		}

		void Function::setArgc(int newArgc) {
			argc = newArgc;
		}

		MathLinkStream& NewPacket(MathLinkStream& ms) {
			checkError(
				ms.get(),
				MLNewPacket(ms.get()),
				LLErrorCode::MLPacketHandleError,
				"Error in MLNewPacket"
			);
			return ms;
		}

		MathLinkStream& EndPacket(MathLinkStream& ms) {
			checkError(
				ms.get(),
				MLEndPacket(ms.get()),
				LLErrorCode::MLPacketHandleError,
				"Error in MLEndPacket"
			);
			return ms;
		}

		MathLinkStream& Flush(MathLinkStream& ms) {
			checkError(
				ms.get(),
				MLFlush(ms.get()),
				LLErrorCode::MLFlowControlError,
				"Error in MLFlush"
			);
			return ms;
		}

		MathLinkStream& Rule(MathLinkStream& ms, Direction dir) {
			if (dir == Direction::Put)
				return ms << Function("Rule", 2);
			else
				return ms >> Function("Rule", 2);
		}

		MathLinkStream& Null(MathLinkStream& ms, Direction dir) {
			if (dir == Direction::Put)
				return ms << Symbol("Null");
			else
				return ms >> Symbol("Null");
		}

		std::string getMLErrorText(MLINK mlp) {
			std::string err = "Error code reported by MathLink: " + std::to_string(MLError(mlp)) + "\n";
			auto mlErrorMsg = MLErrorMessage(mlp);
			err += "\"" + std::string(mlErrorMsg) + "\"\nDebug info: ";
			MLReleaseErrorMessage(mlp, mlErrorMsg);
			MLClearError(mlp);
			return err;
		}


		void checkError(MLINK m, int statusOk, int errorCode, const std::string& debugInfo) {
			if (!statusOk) {
				ErrorManager::throwException(errorCode, getMLErrorText(m) + debugInfo);
			}
		}

		void checkError(MLINK m, int statusOk, const std::string& errorName, const std::string& debugInfo) {
			if (!statusOk) {
				ErrorManager::throwException(errorName, getMLErrorText(m) + debugInfo);
			}
		}

	}
}


