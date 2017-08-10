#include "WolframLibrary.h"
#include "MArgumentManager.h"
#include "LibraryLinkFunctionMacro.h"

using namespace LibraryLinkUtils;

LIBRARY_LINK_FUNCTION(EchoTensor) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	mngr.operateOnTensor(0, [&mngr](auto&& t) {
		mngr.setTensor(t);
	});
	return LIBRARY_NO_ERROR;
}
