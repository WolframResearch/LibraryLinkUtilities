/*
 An example that demonstrates calling back to Mathematica inside a Wolfram Library function.
*/
#ifdef _WIN32
#define NOMINMAX
#endif /* _WIN32 */

#include "mathlink.h"
#include "WolframLibrary.h"
#include "LLU/MArgumentManager.h"

using namespace LibraryLinkUtils;

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion( ) {
	return WolframLibraryVersion;
}


EXTERN_C DLLEXPORT int WolframLibrary_initialize( WolframLibraryData libData) {
	return 0;
}

EXTERN_C DLLEXPORT void WolframLibrary_uninitialize( WolframLibraryData libData) {
	return;
}


EXTERN_C DLLEXPORT int function1(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	MArgumentManager mngr(libData, Argc, Args, Res);

	auto I0 = mngr.getInteger<mint>(0);
	auto I1 = mngr.getInteger<mint>(1);
	
	auto link = libData->getWSLINK(libData);
	MLPutFunction( link, "EvaluatePacket", 1);
	MLPutFunction( link, "Message", 2);
	MLPutFunction( link, "MessageName", 2);
	MLPutSymbol( link, "MyFunction");
	MLPutString( link, "info");
	MLPutString( link, "Message called from within Library function.");
	libData->processWSLINK( link);
	auto pkt = MLNextPacket( link);
	if ( pkt == RETURNPKT) {
		MLNewPacket(link);
	}
	
	mngr.setInteger(I0 + I1);
	return 0;
}

