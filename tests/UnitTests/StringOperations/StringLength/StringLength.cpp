#include "WolframLibrary.h"
#include "MArgumentManager.h"

EXTERN_C DLLEXPORT int StringLength(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	auto& name = mngr.getString(0);
	mngr.setInteger(name.length());
	return LIBRARY_NO_ERROR;
}
