/**
 * @file	DataVectorTest.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	March 15, 2024
 * @brief	Source code for unit tests of GenericDataVector.
 */

#include <string.h>

#include <LLU/Containers/BitVector.h>
#include <LLU/LibraryLinkFunctionMacro.h>
#include <LLU/LLU.h>

/* Initialize Library */
EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	return LIBRARY_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DataVector in plain LibraryLink tests
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static errcode_t constructValidity(WolframDataVectorLibrary_Functions dvFuns, mint length, mint missing_position, bitvector_t* validity) {
	errcode_t err = dvFuns->BitVector_new(length, True, validity); /* True means that all elements are valid */
	if (err) {
		return err;
	}
	/* Mark element at position missing_position as invalid (missing) */
	dvFuns->BitVector_clear(*validity, missing_position);
	return 0;
}

static errcode_t generateNumericDataVector(WolframLibraryData libData, DataVector* res) {
	const mint length = 5;
	const mint missing_position = 3;
	MNumericArray na = NULL;
	int8_t* na_data = NULL;
	bitvector_t validity = NULL;
	errcode_t err = 0;
	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;
	err = naFuns->MNumericArray_new(MNumericArray_Type_Bit8, 1, &length, &na);
	if (err) {
		return err;
	}
	na_data = (int8_t*)(naFuns->MNumericArray_getData(na));
	for (mint i = 0; i < length; i++) {
		na_data[i] = i;
	}
	err = constructValidity(dvFuns, length, missing_position, &validity);
	if (err) {
		return err;
	}
	err = dvFuns->DataVector_newNumeric(&na, validity, res);
	dvFuns->BitVector_release(validity);
	if (err) {
		return err;
	}
	return 0;
}

static errcode_t generateStringDataVector(WolframLibraryData libData, DataVector* res) {
	const mint size = 5;
	const char* strings[size] =
	{"one","two","three",NULL,"five"};
		const mint length = 3 + 3 + 5 + 4;	  // missing_position = 3
	const mint missing_position = 3;
	bitvector_t validity = NULL;
	const char* data = NULL;
	char* buff = NULL;
	mint* offsets = NULL;
	errcode_t err = 0;
	mint i = 0;
	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	offsets = (mint*)libData->WL_malloc(sizeof(mint) * (size + 1));
	if (offsets == NULL) {
		err = -1;
		goto cleanup;
	}
	offsets[0] = 0;
	data = (const char*)libData->WL_malloc(sizeof(char) * length);
	if (data == NULL) {
		err = -1;
		goto cleanup;
	}
	buff = (char*)data;
	for (i = 0; i < size; i++) {
		if (i != missing_position) {
			offsets[i + 1] = offsets[i] + strlen(strings[i]);
			mint nbytes = offsets[i + 1] - offsets[i];
			memcpy(buff, strings[i], nbytes);
			buff += nbytes;
		} else {
			offsets[i + 1] = offsets[i];
		}
	}
	err = constructValidity(dvFuns, size, missing_position, &validity);
	if (err) {
		goto cleanup;
	}
	err = dvFuns->DataVector_newString(size, data, offsets, validity, res);
cleanup:
	dvFuns->BitVector_release(validity);
	if (err) {
		libData->WL_free((void*)offsets);
		libData->WL_free((void*)data);
		dvFuns->DataVector_release(*res);
	}
	return err;
}

EXTERN_C DLLEXPORT int newNumericDataVector(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv = NULL;
	errcode_t err = 0;
	err = generateNumericDataVector(libData, &dv);
	if (err) {
		return err;
	}
	MArgument_setDataVector(res, dv);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int newStringDataVector(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv = NULL;
	errcode_t err = 0;
	err = generateStringDataVector(libData, &dv);
	if (err) {
		return err;
	}
	MArgument_setDataVector(res, dv);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int newBooleanDataVector(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	const mint length = 5;
	bitvector_t bvec = NULL;
	DataVector dv = NULL;
	bitvector_t validity = NULL;
	errcode_t err = 0;
	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;
	err = dvFuns->BitVector_new(length, False, &bvec);
	if (err) {
		return err;
	}
	dvFuns->BitVector_set(bvec, 0);
	dvFuns->BitVector_set(bvec, length - 1);
	err = constructValidity(dvFuns, length, 3, &validity);
	if (err) {
		return err;
	}
	err = dvFuns->DataVector_newBoolean(bvec, validity, &dv);
	dvFuns->BitVector_release(validity);
	if (err) {
		return err;
	}
	MArgument_setDataVector(res, dv);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int newBinaryDataVector(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	mint i = 0;
	const mint size = 5;
	const mint length = 1 + 2 + 3 + 5;	  // missing_position = 3
	const mint missing_position = 3;
	MNumericArray array = NULL;
	uint8_t* array_data = NULL;
	mint* offsets = NULL;
	DataVector dv = NULL;
	bitvector_t validity = NULL;
	errcode_t err = 0;
	int errCode = LIBRARY_FUNCTION_ERROR;
	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	offsets = (mint*)libData->WL_malloc(sizeof(mint) * (size + 1));
	if (offsets == NULL) {
		err = -1;
		goto cleanup;
	}
	offsets[0] = 0;
	err = naFuns->MNumericArray_new(MNumericArray_Type_UBit8, 1, &length, &array);
	if (err) {
		goto cleanup;
	}
	array_data = (uint8_t*)(naFuns->MNumericArray_getData(array));

	for (i = 0; i < size; i++) {
		if (i != missing_position) {
			offsets[i + 1] = offsets[i] + 1 + i;
			for (mint j = offsets[i]; j < offsets[i + 1]; j++) {
				array_data[j] = j;
			}
		} else {
			offsets[i + 1] = offsets[i];
		}
	}
	err = constructValidity(dvFuns, size, missing_position, &validity);
	if (err) {
		goto cleanup;
	}
	err = dvFuns->DataVector_newBinary(size, &array, offsets, validity, &dv);
	if (err) {
		goto cleanup;
	}
	MArgument_setDataVector(res, dv);
	errCode = LIBRARY_NO_ERROR;
cleanup:
	dvFuns->BitVector_release(validity);
	if (errCode != LIBRARY_NO_ERROR) {
		dvFuns->DataVector_release(dv);
	}
	return errCode;
}

EXTERN_C DLLEXPORT int newFixedWidthBinaryDataVector(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	const mint length = 5;
	const mint missing_position = 3;
	mint dims[] = {length, 4};
	MNumericArray array = NULL;
	DataVector dv = NULL;
	bitvector_t validity = NULL;
	errcode_t err = 0;
	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;
	err = naFuns->MNumericArray_new(MNumericArray_Type_UBit8, 2, dims, &array);
	if (err) {
		return err;
	}
	uint8_t* na_data = (uint8_t*)(naFuns->MNumericArray_getData(array));
	for (mint i = 0; i < dims[0]; i++) {
		if (i != missing_position) {
			for (mint j = 0; j < dims[1]; j++) {
				na_data[i * dims[1] + j] = i * dims[1] + j;
			}
		}
	}
	err = constructValidity(dvFuns, length, missing_position, &validity);
	if (err) {
		return err;
	}
	err = dvFuns->DataVector_newFixedWidthBinary(&array, validity, &dv);
	dvFuns->BitVector_release(validity);
	if (err) {
		return err;
	}
	MArgument_setDataVector(res, dv);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int newDateDataVector(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv = NULL;
	errcode_t err = 0;
	const mint length = 5;
	const mint missing_position = 3;
	MNumericArray na = NULL;
	int32_t* na_data = NULL;
	bitvector_t validity = NULL;
	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;
	err = naFuns->MNumericArray_new(MNumericArray_Type_Bit32, 1, &length, &na);
	if (err) {
		return err;
	}
	na_data = (int32_t*)(naFuns->MNumericArray_getData(na));
	na_data[0] = 19740;
	na_data[1] = 19952;
	na_data[2] = 19824;
	na_data[3] = 0;
	na_data[4] = 19900;
	err = constructValidity(dvFuns, length, missing_position, &validity);
	if (err) {
		return err;
	}
	err = dvFuns->DataVector_newDate(&na, 40 /*DAY_GRANULARITY*/, -1 /*unused*/, NULL, validity, &dv);
	dvFuns->BitVector_release(validity);
	if (err) {
		return err;
	}
	MArgument_setDataVector(res, dv);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int newTimeDataVector(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	int errCode = LIBRARY_FUNCTION_ERROR;
	DataVector dv = NULL;
	errcode_t err = 0;
	const mint length = 5;
	const mint missing_position = 3;
	MNumericArray na = NULL;
	int32_t* na_data = NULL;
	bitvector_t validity = NULL;
	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;
	err = naFuns->MNumericArray_new(MNumericArray_Type_Bit32, 1, &length, &na);
	if (err) {
		goto cleanup;
	}
	na_data = (int32_t*)(naFuns->MNumericArray_getData(na));
	na_data[0] = 7795;
	na_data[1] = 75737;
	na_data[2] = 9427;
	na_data[3] = 0;
	na_data[4] = 22961;
	err = constructValidity(dvFuns, length, missing_position, &validity);
	if (err) {
		goto cleanup;
	}
	err = dvFuns->DataVector_newTime(&na, 0 /*SECOND_GRANULARITY*/, 0 /*SECOND_PREC*/, validity, &dv);
	dvFuns->BitVector_release(validity);
	if (err) {
		goto cleanup;
	}
	MArgument_setDataVector(res, dv);
	return LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int passDataVector(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv_in = NULL;
	DataVector dv_out = NULL;
	mbool returnCopyQ = False;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;

	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	if (Argc != 2) {
		goto cleanup;
	}
	dv_in = MArgument_getDataVector(Args[0]);
	if (dv_in == NULL) {
		goto cleanup;
	}
	returnCopyQ = MArgument_getBoolean(Args[1]);
	if (returnCopyQ) {
		err = dvFuns->DataVector_clone(dv_in, &dv_out);
		if (err) {
			goto cleanup;
		}
	} else {
		dv_out = dv_in;
	}
	MArgument_setDataVector(res, dv_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	if (errCode != LIBRARY_NO_ERROR) {
		if (dv_out && dv_out != dv_in) {
			dvFuns->DataVector_release(dv_out);
		}
	}
	return errCode;
}

EXTERN_C DLLEXPORT int getDataVectorLength(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv_in = NULL;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	mint length = 0;

	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getDataVector(Args[0]);
	if (dv_in == NULL) {
		goto cleanup;
	}
	err = dvFuns->DataVector_getLength(dv_in, &length);
	if (err) {
		goto cleanup;
	}
	MArgument_setInteger(res, length);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getDataVectorMissingCount(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv_in = NULL;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	mint missing_count = 0;

	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getDataVector(Args[0]);
	if (dv_in == NULL) {
		goto cleanup;
	}
	err = dvFuns->DataVector_getMissingCount(dv_in, &missing_count);
	if (err) {
		goto cleanup;
	}
	MArgument_setInteger(res, missing_count);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getDataVectorValidity(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv_in = NULL;
	DataVector dv_out = NULL;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	bitvector_t validity = NULL;
	bitvector_t validity_copy = NULL;

	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getDataVector(Args[0]);
	if (dv_in == NULL) {
		goto cleanup;
	}
	err = dvFuns->DataVector_getValidity(dv_in, &validity);
	if (err) {
		goto cleanup;
	}
	err = dvFuns->BitVector_clone(validity, &validity_copy);
	if (err) {
		goto cleanup;
	}
	err = dvFuns->DataVector_newBoolean(validity_copy, NULL, &dv_out);
	if (err) {
		goto cleanup;
	}
	MArgument_setDataVector(res, dv_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getDataNumeric(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv_in = NULL;
	MNumericArray na = NULL;
	MNumericArray na_copy = NULL;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;

	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getDataVector(Args[0]);
	if (dv_in == NULL) {
		goto cleanup;
	}

	err = dvFuns->DataVector_getDataNumeric(dv_in, &na);
	if (err) {
		goto cleanup;
	}
	err = naFuns->MNumericArray_clone(na, &na_copy);
	if (err) {
		goto cleanup;
	}
	MArgument_setMNumericArray(res, na_copy);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getDataString(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv_in = NULL;
	DataStore ds = NULL;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	char* data = NULL;
	mint* offsets = NULL;
	mint length = 0;

	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getDataVector(Args[0]);
	if (dv_in == NULL) {
		goto cleanup;
	}

	err = dvFuns->DataVector_getLength(dv_in, &length);
	if (err) {
		goto cleanup;
	}

	err = dvFuns->DataVector_getDataString(dv_in, &data, &offsets);
	if (err) {
		goto cleanup;
	}

	ds = libData->ioLibraryFunctions->createDataStore();
	for (mint i = 0; i < length; i++) {
		mint nbytes = offsets[i + 1] - offsets[i];
		char* str = (char*)libData->WL_malloc(sizeof(char) * (nbytes + 1));
		memcpy(str, data, nbytes);
		str[nbytes] = 0;
		libData->ioLibraryFunctions->DataStore_addString(ds, str);
		libData->WL_free(str);
		data += nbytes;
	}
	MArgument_setDataStore(res, ds);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	if (errCode != LIBRARY_NO_ERROR) {
		if (ds) {
			libData->ioLibraryFunctions->deleteDataStore(ds);
		}
	}
	return errCode;
}

EXTERN_C DLLEXPORT int getDataBoolean(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv_in = NULL;
	DataVector dv_out = NULL;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	bitvector_t data = NULL;
	bitvector_t data_copy = NULL;
	bitvector_t validity = NULL;

	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getDataVector(Args[0]);
	if (dv_in == NULL) {
		goto cleanup;
	}
	err = dvFuns->DataVector_getDataBoolean(dv_in, &data);
	if (err) {
		goto cleanup;
	}
	err = dvFuns->BitVector_clone(data, &data_copy);
	if (err) {
		goto cleanup;
	}
	err = dvFuns->DataVector_getValidity(dv_in, &validity);
	if (err) {
		goto cleanup;
	}
	err = dvFuns->DataVector_newBoolean(data_copy, validity, &dv_out);
	if (err) {
		goto cleanup;
	}
	MArgument_setDataVector(res, dv_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getDataBinary(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv_in = NULL;
	DataVector dv_out = NULL;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	MNumericArray data = NULL;
	MNumericArray data_copy = NULL;
	mint* offsets = NULL;
	mint* offsets_copy = NULL;
	mint length = 0;
	bitvector_t validity = NULL;

	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getDataVector(Args[0]);
	if (dv_in == NULL) {
		goto cleanup;
	}

	err = dvFuns->DataVector_getLength(dv_in, &length);
	if (err) {
		goto cleanup;
	}

	err = dvFuns->DataVector_getValidity(dv_in, &validity);
	if (err) {
		goto cleanup;
	}

	err = dvFuns->DataVector_getDataBinary(dv_in, &data, &offsets);
	if (err) {
		goto cleanup;
	}

	err = naFuns->MNumericArray_clone(data, &data_copy);
	if (err) {
		goto cleanup;
	}

	offsets_copy = (mint*)libData->WL_malloc(sizeof(mint) * (length + 1));
	memcpy(offsets_copy, offsets, sizeof(mint) * (length + 1));

	err = dvFuns->DataVector_newBinary(length, &data_copy, offsets_copy, validity, &dv_out);
	if (err) {
		goto cleanup;
	}
	MArgument_setDataVector(res, dv_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	if (errCode != LIBRARY_NO_ERROR) {
		libData->WL_free(offsets_copy);
		naFuns->MNumericArray_free(data_copy);
	}
	return errCode;
}

EXTERN_C DLLEXPORT int getDataFixedWidthBinary(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv_in = NULL;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	MNumericArray data = NULL;
	MNumericArray data_copy = NULL;

	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getDataVector(Args[0]);
	if (dv_in == NULL) {
		goto cleanup;
	}

	err = dvFuns->DataVector_getDataFixedWidthBinary(dv_in, &data);
	if (err) {
		goto cleanup;
	}
	err = naFuns->MNumericArray_clone(data, &data_copy);
	if (err) {
		goto cleanup;
	}
	MArgument_setMNumericArray(res, data_copy);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getDataDate(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv_in = NULL;
	DataVector dv_out = NULL;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	MNumericArray data = NULL;
	MNumericArray data_copy = NULL;
	mint granularity = -1;
	mint precision = -1;
	char* time_zone = NULL;
	bitvector_t validity = NULL;

	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getDataVector(Args[0]);
	if (dv_in == NULL) {
		goto cleanup;
	}

	err = dvFuns->DataVector_getDataDate(dv_in, &data, &granularity, &precision, &time_zone);
	if (err) {
		goto cleanup;
	}

	err = naFuns->MNumericArray_clone(data, &data_copy);
	if (err) {
		goto cleanup;
	}

	err = dvFuns->DataVector_getValidity(dv_in, &validity);
	if (err) {
		goto cleanup;
	}

	err = dvFuns->DataVector_newDate(&data_copy, granularity, precision, time_zone, validity, &dv_out);
	if (err) {
		goto cleanup;
	}
	MArgument_setDataVector(res, dv_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getDataTime(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv_in = NULL;
	DataVector dv_out = NULL;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	MNumericArray data = NULL;
	MNumericArray data_copy = NULL;
	mint granularity = -1;
	mint precision = -1;
	bitvector_t validity = NULL;

	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getDataVector(Args[0]);
	if (dv_in == NULL) {
		goto cleanup;
	}

	err = dvFuns->DataVector_getDataTime(dv_in, &data, &granularity, &precision);
	if (err) {
		goto cleanup;
	}

	err = naFuns->MNumericArray_clone(data, &data_copy);
	if (err) {
		goto cleanup;
	}

	err = dvFuns->DataVector_getValidity(dv_in, &validity);
	if (err) {
		goto cleanup;
	}

	err = dvFuns->DataVector_newTime(&data_copy, granularity, precision, validity, &dv_out);
	if (err) {
		goto cleanup;
	}
	MArgument_setDataVector(res, dv_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int isNumericDataVector(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataVector dv_in = NULL;
	int errCode = LIBRARY_FUNCTION_ERROR;
	mbool numericQ = False;

	WolframDataVectorLibrary_Functions dvFuns = libData->dataVectorLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getDataVector(Args[0]);
	if (dv_in == NULL) {
		goto cleanup;
	}

	numericQ = dvFuns->DataVector_numericTypeQ(dv_in);
	MArgument_setBoolean(res, numericQ);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getDataStore(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataStore ds = NULL;
	DataVector dv1 = NULL;
	DataVector dv2 = NULL;
	errcode_t err = 0;
	ds = libData->ioLibraryFunctions->createDataStore();
	if (ds == NULL) {
		return LIBRARY_MEMORY_ERROR;
	}
	err = generateNumericDataVector(libData, &dv1);
	if (err) {
		return err;
	}
	err = generateStringDataVector(libData, &dv2);
	if (err) {
		return err;
	}
	char col_name1[] = "column 1";
	libData	->ioLibraryFunctions->DataStore_addNamedDataVector(ds, col_name1, dv1);
	char col_name2[] = "column 2";
	libData->ioLibraryFunctions->DataStore_addNamedDataVector(ds, col_name2, dv2);
	MArgument_setDataStore(res, ds);
    return LIBRARY_NO_ERROR;
}

static DataStore copyDataStore(WolframLibraryData libData, DataStore ds_in) {
	MArgument data;
	char* name = NULL;
	int errCode = LIBRARY_FUNCTION_ERROR;

	if (ds_in == NULL) {
		return NULL;
	}
	DataStore ds_out = libData->ioLibraryFunctions->createDataStore();
	DataStoreNode dsn = libData->ioLibraryFunctions->DataStore_getFirstNode(ds_in);
	while (dsn != NULL) {
		if (libData->ioLibraryFunctions->DataStoreNode_getData(dsn, &data) != 0) {
			goto cleanup;
		}
		if (libData->ioLibraryFunctions->DataStoreNode_getName(dsn, &name) != 0) {
			goto cleanup;
		}
		switch (libData->ioLibraryFunctions->DataStoreNode_getDataType(dsn)) {
			case MType_Boolean: libData->ioLibraryFunctions->DataStore_addNamedBoolean(ds_out, name, MArgument_getBoolean(data)); break;
			case MType_Integer: libData->ioLibraryFunctions->DataStore_addNamedInteger(ds_out, name, MArgument_getInteger(data)); break;
			case MType_Real: libData->ioLibraryFunctions->DataStore_addNamedReal(ds_out, name, MArgument_getReal(data)); break;
			case MType_Complex: libData->ioLibraryFunctions->DataStore_addNamedComplex(ds_out, name, MArgument_getComplex(data)); break;
			case MType_UTF8String: libData->ioLibraryFunctions->DataStore_addNamedString(ds_out, name, MArgument_getUTF8String(data)); break;
			case MType_Tensor: {
				MTensor outtensor = NULL;
				libData->MTensor_clone(MArgument_getMTensor(data), &outtensor);
				libData->ioLibraryFunctions->DataStore_addNamedMTensor(ds_out, name, outtensor);
				break;
			}
			case MType_NumericArray: {
				MNumericArray outnumeric = NULL;
				libData->numericarrayLibraryFunctions->MNumericArray_clone(MArgument_getMNumericArray(data), &outnumeric);
				libData->ioLibraryFunctions->DataStore_addNamedMNumericArray(ds_out, name, outnumeric);
				break;
			}
			case MType_Image: {
				MImage outimg = NULL;
				libData->imageLibraryFunctions->MImage_clone(MArgument_getMImage(data), &outimg);
				libData->ioLibraryFunctions->DataStore_addNamedMImage(ds_out, name, outimg);
				break;
			}
			case MType_SparseArray: {
				MSparseArray outsparse = NULL;
				libData->sparseLibraryFunctions->MSparseArray_clone(MArgument_getMSparseArray(data), &outsparse);
				libData->ioLibraryFunctions->DataStore_addNamedMSparseArray(ds_out, name, outsparse);
				break;
			}
			case MType_DataStore: {
				DataStore outds = NULL;
				outds = libData->ioLibraryFunctions->copyDataStore(MArgument_getDataStore(data));
				libData->ioLibraryFunctions->DataStore_addNamedDataStore(ds_out, name, outds);
				break;
			}
			case MType_DataVector: {
				DataVector outdv = NULL;
				libData->dataVectorLibraryFunctions->DataVector_clone(MArgument_getDataVector(data), &outdv);
				libData->ioLibraryFunctions->DataStore_addNamedDataVector(ds_out, name, outdv);
				break;
			}
			default: goto cleanup;
		}
		dsn = libData->ioLibraryFunctions->DataStoreNode_getNextNode(dsn);
	}
	errCode = LIBRARY_NO_ERROR;

cleanup:
	if (errCode != LIBRARY_NO_ERROR) {
		if (ds_out) {
			libData->ioLibraryFunctions->deleteDataStore(ds_out);
		}
		return NULL;
	}
	return ds_out;
}

EXTERN_C DLLEXPORT int passDataStore(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataStore ds_in = NULL;
	DataStore ds_out = NULL;
	mbool returnCopyQ = False;
	int errCode = LIBRARY_FUNCTION_ERROR;

	if (Argc != 2) {
		goto cleanup;
	}

	ds_in = MArgument_getDataStore(Args[0]);
	if (ds_in == NULL) {
		goto cleanup;
	}
	returnCopyQ = MArgument_getBoolean(Args[1]);
	if (returnCopyQ) {
		ds_out = copyDataStore(libData, ds_in);
		if (ds_out == NULL) {
			goto cleanup;
		}
	} else {
		ds_out = ds_in;
	}
	MArgument_setDataStore(res, ds_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	if (errCode != LIBRARY_NO_ERROR) {
		if (ds_out && ds_out != ds_in) {
			libData->ioLibraryFunctions->deleteDataStore(ds_out);
		}
	}
	return errCode;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// DataVector LLU wrapper tests
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* Returns an input or a copy of an input */
LLU_LIBRARY_FUNCTION(PassDataVector) {
	auto dv = mngr.getGenericDataVector(0);
	auto returnCopyQ = mngr.getBoolean(1);

	if (returnCopyQ) {
		auto dvOut = dv.clone();
		mngr.set(dvOut);
	} else {
		mngr.set(dv);
	}
}

/* Create numeric DataVector */
LLU_LIBRARY_FUNCTION(NewNumericDV) {
	auto num = mngr.getGenericNumericArray(0);
	auto dv = LLU::GenericDataVector {LLU::DV::Type::Numeric, num.clone()};
	mngr.set(dv);
}

/* Create string DataVector */
LLU_LIBRARY_FUNCTION(NewStringDV) {
	auto strings = mngr.getDataList<LLU::NodeType::UTF8String>(0);
	const auto str_views = strings.values();
	auto dv = LLU::GenericDataVector {str_views};
	mngr.set(dv);
}

/* Create boolean DataVector */
LLU_LIBRARY_FUNCTION(NewBooleanDV) {
	auto dv = LLU::GenericDataVector {LLU::BitVector({true, false, false, false})};
	mngr.set(dv);
}

/* Create binary DataVector */
LLU_LIBRARY_FUNCTION(NewBinaryDV) {
	auto binary_data = mngr.getNumericArray<std::uint8_t>(0);
	auto lengths = mngr.getTensor<mint>(1);
	const auto lengths_span = std::span<mint> {lengths.begin(), lengths.end()};
	auto dv = LLU::GenericDataVector {binary_data.clone(), static_cast<mint>(lengths_span.size()), LLU::DV::lengthsToOffsets(lengths_span)};
	mngr.set(dv);
}

/* Create fixed-width binary DataVector */
LLU_LIBRARY_FUNCTION(NewFixedWidthBinaryDV) {
	auto num = mngr.getGenericNumericArray(0);
	auto dv = LLU::GenericDataVector {LLU::DV::Type::FixedWidthBinary, num.clone()};
	mngr.set(dv);
}

/* Create date DataVector */
LLU_LIBRARY_FUNCTION(NewDateDV) {
	auto data = mngr.getGenericNumericArray(0);
	auto granularity = mngr.getInteger<mint>(1);
	auto precision = mngr.getInteger<mint>(2);
	auto time_zone = mngr.getString(3);
	auto dv = LLU::GenericDataVector {data.clone(), granularity, precision, time_zone};
	mngr.set(dv);
}

/* Create time DataVector */
LLU_LIBRARY_FUNCTION(NewTimeDV) {
	auto data = mngr.getGenericNumericArray(0);
	auto granularity = mngr.getInteger<mint>(1);
	auto precision = mngr.getInteger<mint>(2);
	auto dv = LLU::GenericDataVector {data.clone(), granularity, precision};
	mngr.set(dv);
}