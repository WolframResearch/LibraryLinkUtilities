#include "WolframLibrary.h"
#include "LLU/MArgumentManager.h"

EXTERN_C DLLEXPORT int HelloWorld(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	mngr.setString("Hello World");
	return LIBRARY_NO_ERROR;
}
