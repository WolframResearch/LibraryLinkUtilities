#include "WolframLibrary.h"
#include "LLU/MArgumentManager.h"
#include "LLU/LibraryLinkFunctionMacro.h"

using namespace LibraryLinkUtils;

LIBRARY_LINK_FUNCTION(EchoTensor) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	mngr.operateOnTensor(0, [&mngr](auto&& t) {
		mngr.setTensor(t);
	});
	return LIBRARY_NO_ERROR;
}
