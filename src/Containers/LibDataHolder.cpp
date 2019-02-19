/**
 * @file	LibDataHolder.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	14/06/2018
 *
 * @brief	Implementation of the LibDataHolder class.
 *
 */

#include "LLU/Containers/LibDataHolder.h"

namespace LibraryLinkUtils {

	std::unique_ptr<st_WolframLibraryData> LibDataHolder::libData;
	WolframNumericArrayLibrary_Functions LibDataHolder::naFuns = nullptr;
	WolframImageLibrary_Functions LibDataHolder::imgFuns = nullptr;
	WolframIOLibrary_Functions LibDataHolder::ioFuns = nullptr;

	void LibDataHolder::setLibraryData(WolframLibraryData ld) {
		libData = ld? std::make_unique<st_WolframLibraryData>(*ld) : nullptr;
		naFuns = ld? libData->numericarrayLibraryFunctions : nullptr;
		imgFuns = ld? libData->imageLibraryFunctions : nullptr;
		ioFuns = ld? libData->ioLibraryFunctions : nullptr;
	}

	WolframLibraryData LibDataHolder::getLibraryData() {
		return libData.get();
	}

	WolframNumericArrayLibrary_Functions LibDataHolder::getNumericArrayFunctions() {
		if(!naFuns) {
			//TODO throw
		}
		return naFuns;
	}

	WolframImageLibrary_Functions LibDataHolder::getImageFunctions() {
		if(!imgFuns) {
			//TODO throw
		}
		return imgFuns;
	}

	WolframIOLibrary_Functions LibDataHolder::getIOFunctions() {
		if(!ioFuns) {
			//TODO throw
		}
		return ioFuns;
	}

}
