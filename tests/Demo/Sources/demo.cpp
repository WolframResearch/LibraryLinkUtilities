/**
 * @file	demo.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief
 */

#include <LLU/LLU.h>

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	try {
		LLU::LibraryData::setLibraryData(libData);
//		LLU::ErrorManager::registerPacletErrors({
//			{"MatioIncorrectLibraryVersion", "matio header version is `hv`, but paclet linked to version `pv`."},
//			{"UnexpectedMArgumentType", "Unexpected MArgument type: `type`."},
//		});

	} catch (const LLU::LibraryLinkError& e) {
		return e.which();
	}
	return LLU::ErrorCode::NoError;
}