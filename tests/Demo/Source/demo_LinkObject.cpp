/*
 An example that demonstrates using a LinkObject to communicate
 between Mathematica and a Wolfram Library.
 */

#include <stdlib.h>
#include <string.h>

#include "mathlink.h"

#include "WolframLibrary.h"

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion() {
	return WolframLibraryVersion;
}

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	return 0;
}

static int addtwoImpl(int i, int j) {
	return i + j;
}

EXTERN_C DLLEXPORT int addtwo(WolframLibraryData libData, MLINK mlp) {
	int i1, i2, sum;
	int len;

	if (!MLTestHead(mlp, "List", &len))
		goto retPt;
	if (len != 2)
		goto retPt;

	if (!MLGetInteger(mlp, &i1))
		goto retPt;
	if (!MLGetInteger(mlp, &i2))
		goto retPt;
	if (!MLNewPacket(mlp))
		goto retPt;

	sum = addtwoImpl(i1, i2);

	if (!MLPutInteger(mlp, sum))
		goto retPt;

	return LIBRARY_NO_ERROR;
retPt:
	return LIBRARY_FUNCTION_ERROR;
}

static char* reverseStringImpl(const char* inStr) {
	int i, len;
	char* outStr;

	len = strlen(inStr);
	outStr = (char*)malloc(len + 1);

	outStr[len] = '\0';
	for (i = 0; i < len; i++) {
		outStr[i] = inStr[len - i - 1];
	}
	return outStr;
}

EXTERN_C DLLEXPORT int reverseString(WolframLibraryData libData, MLINK mlp) {
	int res = LIBRARY_FUNCTION_ERROR;
	int len;
	const char* inStr = NULL;
	char* outStr = NULL;

	if (!MLTestHead(mlp, "List", &len))
		goto retPt;
	if (len != 1)
		goto retPt;

	if (!MLGetString(mlp, &inStr))
		goto retPt;

	if (!MLNewPacket(mlp))
		goto retPt;

	outStr = reverseStringImpl(inStr);

	if (!MLPutString(mlp, outStr))
		goto retPt;
	res = LIBRARY_NO_ERROR;
retPt:
	if (inStr != NULL)
		MLReleaseString(mlp, inStr);
	if (outStr != NULL)
		free(outStr);
	return res;
}
