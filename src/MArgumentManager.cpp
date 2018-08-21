/**
 * @file	MArgumentManager.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	18/04/2017
 *
 * @brief	Definitions of non-template member functions and static data members from MArgumentManager class
 *
 */

#include "MArgumentManager.h"

#include <algorithm>

#include "MArray.hpp"

namespace LibraryLinkUtils {

	/* Static data members */

	std::string MArgumentManager::stringResultBuffer;

	/* Constructors */

	MArgumentManager::MArgumentManager(mint Argc, MArgument* Args, MArgument& Res) :
			argc(Argc), args(Args), res(Res), stringArgs(static_cast<decltype(stringArgs)::size_type>(Argc)) {
		if (!libData)
			ErrorManager::throwException(LLErrorName::MArgumentInitError);

	}

	MArgumentManager::MArgumentManager(WolframLibraryData ld, mint Argc, MArgument* Args, MArgument& Res) :
			argc(Argc), args(Args), res(Res), stringArgs(static_cast<decltype(stringArgs)::size_type>(Argc)) {
		setLibraryData(ld);
	}

	/* Other member functions */

	bool MArgumentManager::getBoolean(unsigned int index) const {
		return (MArgument_getBoolean(getArgs(index)) != False);
	}

	double MArgumentManager::getReal(unsigned int index) const {
		return static_cast<double>(MArgument_getReal(getArgs(index)));
	}

	std::string& MArgumentManager::getString(unsigned int index) {
		char* strArg = MArgument_getUTF8String(getArgs(index));
		if (!stringArgs.at(index)) {
			stringArgs[index] = std::make_unique<std::string>(strArg);
			libData->UTF8String_disown(strArg);
		}
		return *stringArgs.at(index);
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

	MRawArray MArgumentManager::getMRawArray(unsigned int index) const {
		return MArgument_getMRawArray(getArgs(index));
	}

	MTensor MArgumentManager::getMTensor(unsigned int index) const {
		return MArgument_getMTensor(getArgs(index));
	}

	rawarray_t MArgumentManager::getRawArrayType(unsigned int index) const {
		MRawArray tmp = MArgument_getMRawArray(getArgs(index));
		return libData->rawarrayLibraryFunctions->MRawArray_getType(tmp);
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
			ErrorManager::throwException(LLErrorName::MArgumentIndexError, "Index " + std::to_string(index) + " out-of-bound when accessing LibraryLink argument");
		return args[index];
	}

} /* namespace LibraryLinkUtils */
