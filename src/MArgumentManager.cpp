/**
 * @file	MArgumentManager.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	18/04/2017
 *
 * @brief	Definitions of non-template member functions and static data members from MArgumentManager class
 *
 */

#include "LLU/MArgumentManager.h"

#include <algorithm>

#include "LLU/Containers/MArray.hpp"

namespace LibraryLinkUtils {

	/* Static data members */

	std::string MArgumentManager::stringResultBuffer;

	/* Constructors */

	MArgumentManager::MArgumentManager(mint Argc, MArgument* Args, MArgument& Res) :
			argc(Argc), args(Args), res(Res) {
		if (!libData)
			ErrorManager::throwException(LLErrorName::MArgumentLibDataError);
		initStringArgs();
	}

	MArgumentManager::MArgumentManager(WolframLibraryData ld, mint Argc, MArgument* Args, MArgument& Res) :
			argc(Argc), args(Args), res(Res) {
		setLibraryData(ld);
		initStringArgs();
	}

	/* Other member functions */

	bool MArgumentManager::getBoolean(unsigned int index) const {
		return (MArgument_getBoolean(getArgs(index)) != False);
	}

	double MArgumentManager::getReal(unsigned int index) const {
		return static_cast<double>(MArgument_getReal(getArgs(index)));
	}

	void MArgumentManager::acquireUTF8String(unsigned int index) {
		if (!stringArgs.at(index)) {
			char* strArg = MArgument_getUTF8String(getArgs(index));
			stringArgs[index].reset(strArg);
		}
	}

	char* MArgumentManager::getCString(unsigned int index) {
		acquireUTF8String(index);
		return stringArgs[index].get();
	}

	std::string MArgumentManager::getString(unsigned int index) {
		acquireUTF8String(index);
		return stringArgs[index].get();
	}

	void MArgumentManager::setString(const std::string& str) {
		stringResultBuffer = str;
		MArgument_setUTF8String(res, const_cast<char*>(stringResultBuffer.c_str()));
	}

	void MArgumentManager::setString(std::string&& str) {
		stringResultBuffer = std::move(str);
		MArgument_setUTF8String(res, const_cast<char*>(stringResultBuffer.c_str()));
	}

	void MArgumentManager::setString(const char* str) {
		stringResultBuffer = str;
		MArgument_setUTF8String(res, const_cast<char*>(stringResultBuffer.c_str()));
	}

	void MArgumentManager::setBoolean(bool result) const noexcept {
		MArgument_setBoolean(res, result? True : False);
	}

	void MArgumentManager::setReal(double result) const noexcept {
		MArgument_setReal(res, result);
	}

	void MArgumentManager::setInteger(mint result) const noexcept {
		MArgument_setInteger(res, result);
	}

	std::complex<double> MArgumentManager::getComplex(unsigned int index) const {
		auto* mc = MArgument_getComplexAddress(getArgs(index));
		return { mc->ri[0], mc->ri[1] };
	}

	void MArgumentManager::setComplex(std::complex<double> c) const noexcept {
		mcomplex mc { { c.real(), c.imag() } };
		MArgument_setComplex(res, mc);
	}

	MNumericArray MArgumentManager::getMNumericArray(unsigned int index) const {
		return MArgument_getMNumericArray(getArgs(index));
	}

	void MArgumentManager::setMNumericArray(MNumericArray ra) {
		MArgument_setMNumericArray(res, ra);
	}	

	MTensor MArgumentManager::getMTensor(unsigned int index) const {
		return MArgument_getMTensor(getArgs(index));
	}

	void MArgumentManager::setMTensor(MTensor t) {
		MArgument_setMTensor(res, t);
	}

	numericarray_data_t MArgumentManager::getNumericArrayType(unsigned int index) const {
		MNumericArray tmp = MArgument_getMNumericArray(getArgs(index));
		return libData->numericarrayLibraryFunctions->MNumericArray_getType(tmp);
	}

	unsigned char MArgumentManager::getTensorType(unsigned int index) const {
		MTensor tmp = MArgument_getMTensor(getArgs(index));
		return static_cast<unsigned char>(libData->MTensor_getType(tmp));
	}

	imagedata_t MArgumentManager::getImageType(unsigned int index) const {
		MImage tmp = MArgument_getMImage(getArgs(index));
		return libData->imageLibraryFunctions->MImage_getDataType(tmp);
	}

	MArgument MArgumentManager::getArgs(unsigned int index) const {
		if (index >= argc)
			ErrorManager::throwExceptionWithDebugInfo(LLErrorName::MArgumentIndexError, "Index " + std::to_string(index) + " out-of-bound when accessing LibraryLink argument");
		return args[index];
	}

	void MArgumentManager::initStringArgs() {
		stringArgs.reserve(argc);
		for (int i = 0; i < argc; ++i) {
			stringArgs.emplace_back(nullptr, libData->UTF8String_disown);
		}
	}

	ProgressMonitor MArgumentManager::getProgressMonitor(double step) const {
		if (argc < 1) {
			ErrorManager::throwExceptionWithDebugInfo(LLErrorName::MArgumentIndexError, "Index too small when accessing ProgressMonitor.");
		}
		auto pmIndex = static_cast<unsigned>(argc - 1); // shared Tensor will be passed as the last argument
		Tensor<double> sharedIndicator = getTensor<double>(pmIndex);
		return ProgressMonitor { std::move(sharedIndicator), step };
	}

	void MArgumentManager::setDataStore(DataStore ds) {
		MArgument_setDataStore(res, ds);
	}

} /* namespace LibraryLinkUtils */
