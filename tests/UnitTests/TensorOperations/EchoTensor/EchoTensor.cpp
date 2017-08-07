#include "WolframLibrary.h"
#include "MArgumentManager.h"

using namespace LibraryLinkUtils;

EXTERN_C DLLEXPORT int EchoTensor(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	mngr.operateOnTensor(0, [&mngr](auto&& t) {
		mngr.setTensor(t);
	});
	return LIBRARY_NO_ERROR;
}
