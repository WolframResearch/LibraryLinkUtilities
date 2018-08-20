#include "WolframLibrary.h"
#include "LLU/MArgumentManager.h"

EXTERN_C DLLEXPORT int EchoString(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	LibraryLinkUtils::MArgumentManager mngr(libData, Argc, Args, Res);

	auto& in1 = mngr.getString(0);
	mngr.setString(std::move(in1));

	return LIBRARY_NO_ERROR;
}
