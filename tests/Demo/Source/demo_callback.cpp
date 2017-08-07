/* Include required header */
#include "WolframLibrary.h"
#include "stdio.h"
#include "stdlib.h"
#include "math.h"

#include "MArgumentManager.h"
#include "Tensor.h"

using namespace LibraryLinkUtils;

/* Return the version of Library Link */
EXTERN_C DLLEXPORT mint WolframLibrary_getVersion() {
	return WolframLibraryVersion;
}

static mint call_id;
static mint call_nargs;
static MArgument *cbArgs;
static mreal **tdata;

EXTERN_C DLLEXPORT mbool manage_callback(WolframLibraryData libData, mint id, MTensor argtypes) {
	if (call_id) {
		(*libData->releaseLibraryCallbackFunction)(call_id);
		call_id = 0;
		free(cbArgs);
		free(tdata);
	}
	call_id = id;
	const mint* dims = (*libData->MTensor_getDimensions)(argtypes);
	call_nargs = dims[0] - 1;
	if (call_nargs == 0) {
		call_id = 0;
		return False;
	}
	mint* typerank = (*libData->MTensor_getIntegerData)(argtypes);
	/* Check that the arguments and result (thus i <= call_nargs loop control) are scalar mreal */
	for (mint i = 0; i <= call_nargs; i++) {
		/* Each row is {type, rank} */
		if ((typerank[0] != MType_Real) || (typerank[1] != 0)) {
			call_id = 0;
			call_nargs = 0;
			return False;
		}
		typerank += 2;
	}
	cbArgs = (MArgument *) malloc((call_nargs + 1) * sizeof(MArgument));
	tdata = (mreal **) malloc(call_nargs * sizeof(mreal *));
	return True;
}

/* Initialize Library */
EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	call_id = 0;
	return (*libData->registerLibraryCallbackManager)("demo_callback_manager", manage_callback);
}

/* Uninitialize Library */
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {
	(*libData->unregisterLibraryCallbackManager)("demo_callback_manager");
}

EXTERN_C DLLEXPORT int apply_callback(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		if (call_id <= 0)
			return LIBRARY_FUNCTION_ERROR;
		if (Argc != call_nargs)
			return LIBRARY_FUNCTION_ERROR;

		MArgumentManager mngr(Argc, Args, Res);

		auto T = mngr.getTensor<double>(0);
		auto dims = T.dimensions();
		auto n = T.size();
		tdata[0] = T.data();

		for (auto j = 1; j < call_nargs; j++) {
			T = mngr.getTensor<double>(j);
			if (T.size() != n)
				throw LibraryLinkError<LLErrorCode>(LLErrorCode::DimensionsError);
			tdata[j] = T.data();
		}

		Tensor<double> Tres(0., dims);
		auto* r = Tres.data();

		for (mint i = 0; i < n; i++) {
			for (mint j = 0; j < call_nargs; j++)
				MArgument_getRealAddress(cbArgs[j]) = tdata[j] + i;
			MArgument_getRealAddress(cbArgs[call_nargs]) = r + i;
			if ((*libData->callLibraryCallbackFunction)(call_id, call_nargs, cbArgs, cbArgs[1])) {
				throw LibraryLinkError<LLErrorCode>(LLErrorCode::FunctionError);
			}
		}
		mngr.setTensor(Tres);
	}
	catch (LibraryLinkError<LLErrorCode>& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return static_cast<int>(err);
}

EXTERN_C DLLEXPORT int apply_sin(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
int err = 0;
int type;
mint i, n, rank;
mint const *dims;
mreal *t, *r;
MTensor T, Tres = 0;

if (Argc != 1)
	return LIBRARY_FUNCTION_ERROR;

T = MArgument_getMTensor(Args[0]);
type = (*libData->MTensor_getType)(T);
rank = (*libData->MTensor_getRank)(T);
dims = (*libData->MTensor_getDimensions)(T);
n = (*libData->MTensor_getFlattenedLength)(T);
t = (*libData->MTensor_getRealData)(T);

err = (*libData->MTensor_new)(type, rank, dims, &Tres);
if (err)
	return err;
r = (*libData->MTensor_getRealData)(Tres);

for (i = 0; i < n; i++) {
	r[i] = sin(t[i]);
}

MArgument_setMTensor(Res, Tres);
return 0;
}
