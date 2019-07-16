/**
 * @file	PoliciesTest.cpp
 * @brief	Unit tests for passing policies and related functionality
 */

#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>


EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	//LLU::ErrorManager::registerPacletErrors();
	return 0;
}

LIBRARY_LINK_FUNCTION(IsOwnerAutomatic) {
	LLU::MArgumentManager mngr {libData, Argc, Args, Res};
	auto img = mngr.getGenericImage(0);
	//mngr.set(img.);
}