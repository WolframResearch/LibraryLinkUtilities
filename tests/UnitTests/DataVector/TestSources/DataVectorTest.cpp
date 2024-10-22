/**
 * @file	DataVectorTest.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	March 15, 2024
 * @brief	Source code for unit tests of DataVector.
 */

#include <LLU/LibraryLinkFunctionMacro.h>
#include <LLU/LLU.h>

#include "WolframTabularColumnLibrary.h"

/* Initialize Library */
EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	return LIBRARY_NO_ERROR;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// TabularColumn in plain LibraryLink tests
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
LLU::Int8Array constructValidity(mint length, mint missing_position) {
	LLU::Int8Array validity(1, LLU::MArrayDimensions {length});
	validity[missing_position] = 0;
	return validity;
}

LLU::GenericNumericArray numericArrayFromRawData(void* raw_data, numericarray_data_t type, const LLU::MArrayDimensions& dims) {
	LLU::GenericNumericArray array {type, dims.rank(), dims.data()};
	LLU::asTypedNumericArray(array, [&] <typename T> (LLU::NumericArrayTypedView<T> num_array) {
		const T* typed_data = static_cast<T*>(raw_data);
		std::copy_n(typed_data, dims.flatCount(), num_array.begin());
	});
	return array;
}

LLU::GenericNumericArray numericArrayFromRawData(void* raw_data, numericarray_data_t type, mint num_elems) {
	return numericArrayFromRawData(raw_data, type, LLU::MArrayDimensions {num_elems});
}

static errcode_t generateNumericTabularColumn(WolframLibraryData libData, TabularColumn* res) {
	const mint length = 5;
	const mint missing_position = 3;
	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;
	auto na = LLU::Int8Array(0, LLU::MArrayDimensions {length});
	int8_t v = 0;
	for (auto& elem : na) {
		elem = v++;
	}
	auto validity = constructValidity(length, missing_position);
	return dvFuns->TabularColumn_newNumeric(na.abandonContainer(), validity.getContainer(), res);
}

static errcode_t generateStringTabularColumn(WolframLibraryData libData, TabularColumn* res) {
	const mint size = 5;
	const char* strings[size] = {"one", "two", "three", nullptr, "five"};
	const mint length = 3 + 3 + 5 + 4;	  // missing_position = 3
	const mint missing_position = 3;
	errcode_t err = 0;
	mint i = 0;
	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	auto offsets = LLU::makeUnique<mint[]>(size + 1);
	offsets[0] = 0;
	auto data = LLU::makeUnique<char[]>(length);
	auto buff = data.get();
	for (i = 0; i < size; i++) {
		if (i != missing_position) {
			offsets[i + 1] = offsets[i] + static_cast<mint>(strlen(strings[i]));
			mint nbytes = offsets[i + 1] - offsets[i];
			memcpy(buff, strings[i], nbytes);
			buff += nbytes;
		} else {
			offsets[i + 1] = offsets[i];
		}
	}
	auto validity = constructValidity(size, missing_position);
	err = dvFuns->TabularColumn_newString(size, data.release(), offsets.release(), validity.getContainer(), res);
	if (err) {
		dvFuns->TabularColumn_release(*res);
	}
	return err;
}

EXTERN_C DLLEXPORT int newNumericTabularColumn(WolframLibraryData libData, mint /*Argc*/, MArgument* /*Args*/, MArgument res) {
	TabularColumn dv = nullptr;
	errcode_t err = 0;
	err = generateNumericTabularColumn(libData, &dv);
	if (err) {
		return err;
	}
	MArgument_setTabularColumn(res, dv);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int newStringTabularColumn(WolframLibraryData libData, mint /*Argc*/, MArgument* /*Args*/, MArgument res) {
	TabularColumn dv = nullptr;
	errcode_t err = 0;
	err = generateStringTabularColumn(libData, &dv);
	if (err) {
		return err;
	}
	MArgument_setTabularColumn(res, dv);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int newBooleanTabularColumn(WolframLibraryData libData, mint /*Argc*/, MArgument* /*Args*/, MArgument res) {
	const mint length = 5;
	const mint missing_position = 3;
	TabularColumn dv = nullptr;
	errcode_t err = 0;
	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;
	LLU::Int8Array bvec(0, LLU::MArrayDimensions {length});
	bvec[0] = 1;
	bvec[length - 1] = 1;
	auto validity = constructValidity(length, missing_position);
	err = dvFuns->TabularColumn_newBoolean(bvec.getContainer(), validity.getContainer(), &dv);
	if (err) {
		return err;
	}
	MArgument_setTabularColumn(res, dv);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int newByteArrayTabularColumn(WolframLibraryData libData, mint /*Argc*/, MArgument* /*Args*/, MArgument res) {
	const mint size = 5;
	const mint length = 1 + 2 + 3 + 5;	  // missing_position = 3
	const mint missing_position = 3;
	MNumericArray array = nullptr;
	uint8_t* array_data = nullptr;
	TabularColumn dv = nullptr;
	errcode_t err = 0;
	int errCode = LIBRARY_FUNCTION_ERROR;
	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	auto offsets = LLU::makeUnique<mint[]>(size + 1);
	offsets[0] = 0;
	err = naFuns->MNumericArray_new(MNumericArray_Type_UBit8, 1, &length, &array);
	if (err) {
		return err;
	}
	array_data = (uint8_t*)(naFuns->MNumericArray_getData(array));

	for (mint i = 0; i < size; i++) {
		if (i != missing_position) {
			offsets[i + 1] = offsets[i] + 1 + i;
			for (mint j = offsets[i]; j < offsets[i + 1]; j++) {
				array_data[j] = j;
			}
		} else {
			offsets[i + 1] = offsets[i];
		}
	}
	auto validity = constructValidity(size, missing_position);
	err = dvFuns->TabularColumn_newByteArray(size, array, offsets.release(), validity.getContainer(), &dv);
	if (err) {
		goto cleanup;
	}
	MArgument_setTabularColumn(res, dv);
	errCode = LIBRARY_NO_ERROR;
cleanup:
	if (errCode != LIBRARY_NO_ERROR) {
		dvFuns->TabularColumn_release(dv);
	}
	return errCode;
}

EXTERN_C DLLEXPORT int newFixedWidthByteArrayTabularColumn(WolframLibraryData libData, mint /*Argc*/, MArgument* /*Args*/, MArgument res) {
	const mint length = 5;
	const mint missing_position = 3;
	const mint dims[] = {length, 4};
	TabularColumn dv = nullptr;
	errcode_t err = 0;
	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;
	auto array = LLU::UInt8Array(0, LLU::MArrayDimensions {dims, 2});
	auto* na_data = array.data();
	for (mint i = 0; i < dims[0]; i++) {
		if (i != missing_position) {
			for (mint j = 0; j < dims[1]; j++) {
				na_data[i * dims[1] + j] = i * dims[1] + j;
			}
		}
	}
	auto validity = constructValidity(length, missing_position);
	err = dvFuns->TabularColumn_newFixedWidthByteArray(array.abandonContainer(), validity.getContainer(), &dv);
	if (err) {
		return err;
	}
	MArgument_setTabularColumn(res, dv);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int newDateTabularColumn(WolframLibraryData libData, mint /*Argc*/, MArgument* /*Args*/, MArgument res) {
	TabularColumn dv = nullptr;
	errcode_t err = 0;
	const mint length = 5;
	const mint missing_position = 3;
	MNumericArray na = nullptr;
	int32_t* na_data = nullptr;
	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;
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
	auto validity = constructValidity(length, missing_position);
	if (err) {
		return err;
	}
	err = dvFuns->TabularColumn_newDate(na, -40 /*DAY_GRANULARITY*/, -1 /*unused*/, nullptr, validity.getContainer(), &dv);
	if (err) {
		return err;
	}
	MArgument_setTabularColumn(res, dv);
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int newTimeTabularColumn(WolframLibraryData libData, mint /*Argc*/, MArgument* /*Args*/, MArgument res) {
	int errCode = LIBRARY_FUNCTION_ERROR;
	TabularColumn dv = nullptr;
	errcode_t err = 0;
	const mint length = 5;
	const mint missing_position = 3;
	MNumericArray na = nullptr;
	int32_t* na_data = nullptr;
	WolframNumericArrayLibrary_Functions naFuns = libData->numericarrayLibraryFunctions;
	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;
	err = naFuns->MNumericArray_new(MNumericArray_Type_Bit32, 1, &length, &na);
	if (err) {
		return err;
	}
	na_data = (int32_t*)(naFuns->MNumericArray_getData(na));
	na_data[0] = 7795;
	na_data[1] = 75737;
	na_data[2] = 9427;
	na_data[3] = 0;
	na_data[4] = 22961;
	auto validity = constructValidity(length, missing_position);
	if (err) {
		goto cleanup;
	}
	err = dvFuns->TabularColumn_newTime(na, 0 /*SECOND_GRANULARITY*/, 0 /*SECOND_PREC*/, validity.getContainer(), &dv);
	if (err) {
		goto cleanup;
	}
	MArgument_setTabularColumn(res, dv);
	return LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int passTabularColumn(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	TabularColumn dv_in = nullptr;
	TabularColumn dv_out = nullptr;
	mbool returnCopyQ = False;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;

	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	if (Argc != 2) {
		goto cleanup;
	}
	dv_in = MArgument_getTabularColumn(Args[0]);
	if (dv_in == nullptr) {
		goto cleanup;
	}
	returnCopyQ = MArgument_getBoolean(Args[1]);
	if (returnCopyQ) {
		err = dvFuns->TabularColumn_clone(dv_in, &dv_out);
		if (err) {
			goto cleanup;
		}
	} else {
		dv_out = dv_in;
	}
	MArgument_setTabularColumn(res, dv_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	if (errCode != LIBRARY_NO_ERROR) {
		if (dv_out && dv_out != dv_in) {
			dvFuns->TabularColumn_release(dv_out);
		}
	}
	return errCode;
}

EXTERN_C DLLEXPORT int getTabularColumnLength(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	TabularColumn dv_in = nullptr;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	mint length = 0;

	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getTabularColumn(Args[0]);
	if (dv_in == nullptr) {
		goto cleanup;
	}
	err = dvFuns->TabularColumn_getLength(dv_in, &length);
	if (err) {
		goto cleanup;
	}
	MArgument_setInteger(res, length);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getTabularColumnMissingCount(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	TabularColumn dv_in = nullptr;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	mint missing_count = 0;

	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getTabularColumn(Args[0]);
	if (dv_in == nullptr) {
		goto cleanup;
	}
	err = dvFuns->TabularColumn_getMissingCount(dv_in, &missing_count);
	if (err) {
		goto cleanup;
	}
	MArgument_setInteger(res, missing_count);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getTabularColumnValidity(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	TabularColumn dv_in = nullptr;
	TabularColumn dv_out = nullptr;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	MNumericArray validity = nullptr;

	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getTabularColumn(Args[0]);
	if (dv_in == nullptr) {
		goto cleanup;
	}
	err = dvFuns->TabularColumn_getValidity(dv_in, &validity);
	if (err) {
		goto cleanup;
	}
	err = dvFuns->TabularColumn_newBoolean(validity, nullptr, &dv_out);
	if (err) {
		goto cleanup;
	}
	libData->numericarrayLibraryFunctions->MNumericArray_free(validity);
	MArgument_setTabularColumn(res, dv_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getDataNumeric(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	TabularColumn dv_in = nullptr;
	void* data = nullptr;
	numericarray_data_t data_type;
	mint num_elems;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;

	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	if (Argc != 1) {
		return errCode;
	}
	dv_in = MArgument_getTabularColumn(Args[0]);
	if (dv_in == nullptr) {
		return errCode;
	}
	err = dvFuns->TabularColumn_getLength(dv_in, &num_elems);
	if (err) {
		return errCode;
	}
	err = dvFuns->TabularColumn_getDataNumeric(dv_in, &data, &data_type);
	if (err) {
		return errCode;
	}
	auto na = numericArrayFromRawData(data, data_type, num_elems);
	MArgument_setMNumericArray(res, na.abandonContainer());
	return LIBRARY_NO_ERROR;
}

EXTERN_C DLLEXPORT int getDataString(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	TabularColumn dv_in = nullptr;
	DataStore ds = nullptr;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	char* data = nullptr;
	mint* offsets = nullptr;
	mint length = 0;

	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getTabularColumn(Args[0]);
	if (dv_in == nullptr) {
		goto cleanup;
	}

	err = dvFuns->TabularColumn_getLength(dv_in, &length);
	if (err) {
		goto cleanup;
	}

	err = dvFuns->TabularColumn_getDataString(dv_in, &data, &offsets);
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
	TabularColumn dv_in = nullptr;
	TabularColumn dv_out = nullptr;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	MNumericArray data = nullptr;
	MNumericArray validity = nullptr;

	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getTabularColumn(Args[0]);
	if (dv_in == nullptr) {
		goto cleanup;
	}
	err = dvFuns->TabularColumn_getDataBoolean(dv_in, &data);
	if (err) {
		goto cleanup;
	}
	err = dvFuns->TabularColumn_getValidity(dv_in, &validity);
	if (err) {
		goto cleanup;
	}
	err = dvFuns->TabularColumn_newBoolean(data, validity, &dv_out);
	if (err) {
		goto cleanup;
	}
	MArgument_setTabularColumn(res, dv_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	libData->numericarrayLibraryFunctions->MNumericArray_free(data);
	libData->numericarrayLibraryFunctions->MNumericArray_free(validity);
	return errCode;
}

EXTERN_C DLLEXPORT int getDataBinary(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	TabularColumn dv_in = nullptr;
	TabularColumn dv_out = nullptr;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	uint8_t* data = nullptr;
	mint* offsets = nullptr;
	mint* offsets_copy = nullptr;
	mint length = 0;
	MNumericArray validity = nullptr;

	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	if (Argc != 1) {
		return errCode;
	}
	dv_in = MArgument_getTabularColumn(Args[0]);
	if (dv_in == nullptr) {
		return errCode;
	}

	err = dvFuns->TabularColumn_getLength(dv_in, &length);
	if (err) {
		return errCode;
	}

	err = dvFuns->TabularColumn_getValidity(dv_in, &validity);
	if (err) {
		return errCode;
	}

	err = dvFuns->TabularColumn_getDataByteArray(dv_in, &data, &offsets);
	if (err) {
		return errCode;
	}

	auto na = numericArrayFromRawData(data, MNumericArray_Type_UBit8, offsets[length]);

	offsets_copy = (mint*)libData->WL_malloc(sizeof(mint) * (length + 1));
	memcpy(offsets_copy, offsets, sizeof(mint) * (length + 1));

	err = dvFuns->TabularColumn_newByteArray(length, na.abandonContainer(), offsets_copy, validity, &dv_out);
	if (err) {
		goto cleanup;
	}
	MArgument_setTabularColumn(res, dv_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	if (errCode != LIBRARY_NO_ERROR) {
		libData->WL_free(offsets_copy);
	}
	libData->numericarrayLibraryFunctions->MNumericArray_free(validity);
	return errCode;
}

EXTERN_C DLLEXPORT int getDataFixedWidthBinary(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	TabularColumn dv_in = nullptr;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	uint8_t* data = nullptr;
	mint width, length;
	LLU::GenericNumericArray data_copy;

	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getTabularColumn(Args[0]);
	if (dv_in == nullptr) {
		goto cleanup;
	}

	err = dvFuns->TabularColumn_getLength(dv_in, &length);
	if (err) {
		return errCode;
	}

	err = dvFuns->TabularColumn_getDataFixedWidthByteArray(dv_in, &data, &width);
	if (err) {
		goto cleanup;
	}
	data_copy = numericArrayFromRawData(data, MNumericArray_Type_UBit8, LLU::MArrayDimensions {length, width});

	MArgument_setMNumericArray(res, data_copy.abandonContainer());
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getDataDate(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	TabularColumn dv_in = nullptr;
	TabularColumn dv_out = nullptr;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	void* data = nullptr;
	numericarray_data_t raw_type;
	mint length;
	mint granularity = -1;
	mint precision = -1;
	char* time_zone = nullptr;
	MNumericArray validity = nullptr;

	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	if (Argc != 1) {
		return errCode;
	}
	dv_in = MArgument_getTabularColumn(Args[0]);
	if (dv_in == nullptr) {
		return errCode;
	}

	err = dvFuns->TabularColumn_getLength(dv_in, &length);
	if (err) {
		return errCode;
	}

	err = dvFuns->TabularColumn_getDataDate(dv_in, &data, &raw_type, &granularity, &precision, &time_zone);
	if (err) {
		return errCode;
	}

	auto na = numericArrayFromRawData(data, raw_type, length);

	err = dvFuns->TabularColumn_getValidity(dv_in, &validity);
	if (err) {
		goto cleanup;
	}

	err = dvFuns->TabularColumn_newDate(na.abandonContainer(), granularity, precision, time_zone, validity, &dv_out);
	if (err) {
		goto cleanup;
	}
	MArgument_setTabularColumn(res, dv_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	libData->numericarrayLibraryFunctions->MNumericArray_free(validity);
	return errCode;
}

EXTERN_C DLLEXPORT int getDataTime(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	TabularColumn dv_in = nullptr;
	TabularColumn dv_out = nullptr;
	int errCode = LIBRARY_FUNCTION_ERROR;
	errcode_t err = 0;
	void* data = nullptr;
	numericarray_data_t raw_type;
	mint length;
	mint granularity = -1;
	mint precision = -1;
	MNumericArray validity = nullptr;

	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	if (Argc != 1) {
		return errCode;
	}
	dv_in = MArgument_getTabularColumn(Args[0]);
	if (dv_in == nullptr) {
		return errCode;
	}

	err = dvFuns->TabularColumn_getLength(dv_in, &length);
	if (err) {
		return errCode;
	}

	err = dvFuns->TabularColumn_getDataTime(dv_in, &data, &raw_type, &granularity, &precision);
	if (err) {
		return errCode;
	}

	auto na = numericArrayFromRawData(data, raw_type, length);

	err = dvFuns->TabularColumn_getValidity(dv_in, &validity);
	if (err) {
		goto cleanup;
	}

	err = dvFuns->TabularColumn_newTime(na.abandonContainer(), granularity, precision, validity, &dv_out);
	if (err) {
		goto cleanup;
	}
	MArgument_setTabularColumn(res, dv_out);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	libData->numericarrayLibraryFunctions->MNumericArray_free(validity);
	return errCode;
}

EXTERN_C DLLEXPORT int isNumericTabularColumn(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	TabularColumn dv_in = nullptr;
	int errCode = LIBRARY_FUNCTION_ERROR;
	mbool numericQ = False;

	WolframTabularColumnLibrary_Functions dvFuns = libData->tabularColumnLibraryFunctions;

	if (Argc != 1) {
		goto cleanup;
	}
	dv_in = MArgument_getTabularColumn(Args[0]);
	if (dv_in == nullptr) {
		goto cleanup;
	}

	numericQ = dvFuns->TabularColumn_numericTypeQ(dv_in);
	MArgument_setBoolean(res, numericQ);
	errCode = LIBRARY_NO_ERROR;

cleanup:
	return errCode;
}

EXTERN_C DLLEXPORT int getDataStore(WolframLibraryData libData, mint /*Argc*/, MArgument* /*Args*/, MArgument res) {
	DataStore ds = nullptr;
	TabularColumn dv1 = nullptr;
	TabularColumn dv2 = nullptr;
	errcode_t err = 0;
	ds = libData->ioLibraryFunctions->createDataStore();
	if (ds == nullptr) {
		return LIBRARY_MEMORY_ERROR;
	}
	err = generateNumericTabularColumn(libData, &dv1);
	if (err) {
		return err;
	}
	err = generateStringTabularColumn(libData, &dv2);
	if (err) {
		return err;
	}
	char col_name1[] = "column 1";
	libData	->ioLibraryFunctions->DataStore_addNamedTabularColumn(ds, col_name1, dv1);
	char col_name2[] = "column 2";
	libData->ioLibraryFunctions->DataStore_addNamedTabularColumn(ds, col_name2, dv2);
	MArgument_setDataStore(res, ds);
    return LIBRARY_NO_ERROR;
}

static DataStore copyDataStore(WolframLibraryData libData, DataStore ds_in) {
	MArgument data;
	char* name = nullptr;
	int errCode = LIBRARY_FUNCTION_ERROR;

	if (ds_in == nullptr) {
		return nullptr;
	}
	DataStore ds_out = libData->ioLibraryFunctions->createDataStore();
	DataStoreNode dsn = libData->ioLibraryFunctions->DataStore_getFirstNode(ds_in);
	while (dsn != nullptr) {
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
				MTensor outtensor = nullptr;
				libData->MTensor_clone(MArgument_getMTensor(data), &outtensor);
				libData->ioLibraryFunctions->DataStore_addNamedMTensor(ds_out, name, outtensor);
				break;
			}
			case MType_NumericArray: {
				MNumericArray outnumeric = nullptr;
				libData->numericarrayLibraryFunctions->MNumericArray_clone(MArgument_getMNumericArray(data), &outnumeric);
				libData->ioLibraryFunctions->DataStore_addNamedMNumericArray(ds_out, name, outnumeric);
				break;
			}
			case MType_Image: {
				MImage outimg = nullptr;
				libData->imageLibraryFunctions->MImage_clone(MArgument_getMImage(data), &outimg);
				libData->ioLibraryFunctions->DataStore_addNamedMImage(ds_out, name, outimg);
				break;
			}
			case MType_SparseArray: {
				MSparseArray outsparse = nullptr;
				libData->sparseLibraryFunctions->MSparseArray_clone(MArgument_getMSparseArray(data), &outsparse);
				libData->ioLibraryFunctions->DataStore_addNamedMSparseArray(ds_out, name, outsparse);
				break;
			}
			case MType_DataStore: {
				DataStore outds = nullptr;
				outds = libData->ioLibraryFunctions->copyDataStore(MArgument_getDataStore(data));
				libData->ioLibraryFunctions->DataStore_addNamedDataStore(ds_out, name, outds);
				break;
			}
			case MType_TabularColumn: {
				TabularColumn outdv = nullptr;
				libData->tabularColumnLibraryFunctions->TabularColumn_clone(MArgument_getTabularColumn(data), &outdv);
				libData->ioLibraryFunctions->DataStore_addNamedTabularColumn(ds_out, name, outdv);
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
		return nullptr;
	}
	return ds_out;
}

EXTERN_C DLLEXPORT int passDataStore(WolframLibraryData libData, mint Argc, MArgument* Args, MArgument res) {
	DataStore ds_in = nullptr;
	DataStore ds_out = nullptr;
	mbool returnCopyQ = False;
	int errCode = LIBRARY_FUNCTION_ERROR;

	if (Argc != 2) {
		goto cleanup;
	}

	ds_in = MArgument_getDataStore(Args[0]);
	if (ds_in == nullptr) {
		goto cleanup;
	}
	returnCopyQ = MArgument_getBoolean(Args[1]);
	if (returnCopyQ) {
		ds_out = copyDataStore(libData, ds_in);
		if (ds_out == nullptr) {
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
	auto dv = mngr.getDataVector(0);
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
	auto dv = LLU::DataVector {LLU::DV::Type::Numeric, num.clone()};
	mngr.set(dv);
}

/* Create string DataVector */
LLU_LIBRARY_FUNCTION(NewStringDV) {
	auto strings = mngr.getDataList<LLU::NodeType::UTF8String>(0);
	const auto str_views = strings.values();
	auto dv = LLU::DataVector {str_views};
	mngr.set(dv);
}

/* Create boolean DataVector */
LLU_LIBRARY_FUNCTION(NewBooleanDV) {
	LLU::Int8Array boolean_data(0, LLU::MArrayDimensions {4});
	boolean_data[0] = 1;
	auto dv = LLU::DataVector {boolean_data};
	mngr.set(dv);
}

/* Create binary DataVector */
LLU_LIBRARY_FUNCTION(NewBinaryDV) {
	auto binary_data = mngr.getNumericArray<std::uint8_t>(0);
	auto lengths = mngr.getTensor<mint>(1);
	const auto lengths_span = std::span<mint> {lengths.begin(), lengths.end()};
	auto dv = LLU::DataVector {binary_data.clone(), static_cast<mint>(lengths_span.size()), LLU::DV::lengthsToOffsets(lengths_span)};
	mngr.set(dv);
}

/* Create fixed-width binary DataVector */
LLU_LIBRARY_FUNCTION(NewFixedWidthBinaryDV) {
	auto num = mngr.getGenericNumericArray(0);
	auto dv = LLU::DataVector {LLU::DV::Type::FixedWidthBinary, num.clone()};
	mngr.set(dv);
}

/* Create date DataVector */
LLU_LIBRARY_FUNCTION(NewDateDV) {
	auto data = mngr.getGenericNumericArray(0);
	auto granularity = mngr.getInteger<mint>(1);
	auto precision = mngr.getInteger<mint>(2);
	auto time_zone = mngr.getString(3);
	auto dv = LLU::DataVector {data.clone(), granularity, precision, time_zone};
	mngr.set(dv);
}

/* Create time DataVector */
LLU_LIBRARY_FUNCTION(NewTimeDV) {
	auto data = mngr.getGenericNumericArray(0);
	auto granularity = mngr.getInteger<mint>(1);
	auto precision = mngr.getInteger<mint>(2);
	auto dv = LLU::DataVector {data.clone(), granularity, precision};
	mngr.set(dv);
}