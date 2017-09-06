/* Include required header */
#include "WolframLibrary.h"
#include "WolframSparseLibrary.h"

#include "MArgumentManager.h"
#include "LibraryLinkError.h"

using namespace LibraryLinkUtils;

/* Return the version of Library Link */
EXTERN_C DLLEXPORT mint WolframLibrary_getVersion() {
	return WolframLibraryVersion;
}

/* Initialize Library */
EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	return LIBRARY_NO_ERROR;
}

/* Uninitialize Library */
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {
	return;
}

/* Access to sparse properties and CSR data */
EXTERN_C DLLEXPORT int sparse_properties(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto error = LLErrorCode::NoError;
	try {
		WolframSparseLibrary_Functions sparseFuns = libData->sparseLibraryFunctions;
		int err = 0;
		MArgumentManager mngr(Argc, Args, Res);
		const auto& what = mngr.getString(0);
		auto S = MArgument_getMSparseArray(Args[1]);
		MTensor* T = 0;
		MTensor Tres = 0;
		if (what == "ImplicitValue") {
			T = sparseFuns->MSparseArray_getImplicitValue(S);
		}
		else if (what == "ExplicitValues") {
			T = sparseFuns->MSparseArray_getExplicitValues(S);
			if (*T == NULL) {
				T = (*(sparseFuns->MSparseArray_getImplicitValue))(S);
				mint type = libData->MTensor_getType(*T);
				mint len = 0;
				err = libData->MTensor_new(type, 1, &len, &Tres);
			}
		}
		else if (what == "RowPointers") {
			T = sparseFuns->MSparseArray_getRowPointers(S);
		}
		else if (what == "ColumnIndices") {
			T = sparseFuns->MSparseArray_getColumnIndices(S);
		}
		else if (what == "ExplicitPositions") {
			err = sparseFuns->MSparseArray_getExplicitPositions(S, &Tres);
		}
		else if (what == "Normal") {
			err = sparseFuns->MSparseArray_toMTensor(S, &Tres);
		}
		else {
			err = LIBRARY_FUNCTION_ERROR;
		}
		if (err)
			ErrorManager::throwException("FunctionError");
		if (!Tres)
			libData->MTensor_clone(*T, &Tres);

		MArgument_setMTensor(Res, Tres);
	}
	catch (LibraryLinkError& e) {
		error = e.which();
	}
	catch (std::exception& e) {
		error = LLErrorCode::FunctionError;
	}
	return static_cast<int>(error);
}

EXTERN_C DLLEXPORT int sparse_modify_values(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	WolframSparseLibrary_Functions sparseFuns = libData->sparseLibraryFunctions;
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);

		MSparseArray S = MArgument_getMSparseArray(Args[0]);
		Tensor<double> Vp(*(sparseFuns->MSparseArray_getExplicitValues(S)));

		auto T = mngr.getTensor<double>(1);
		if (T.size() != Vp.size())
			return LIBRARY_DIMENSION_ERROR;

		std::copy(T.begin(), T.end(), Vp.begin());

		/* Recompute explicit positions */
		MSparseArray Sout;
		auto err = sparseFuns->MSparseArray_resetImplicitValue(S, nullptr, &Sout);
		sparseFuns->MSparseArray_disown(S);
		if (!err)
			MArgument_setMSparseArray(Res, Sout);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

