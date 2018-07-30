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
	WolframRawArrayLibrary_Functions LibDataHolder::raFuns = nullptr;
	WolframImageLibrary_Functions LibDataHolder::imgFuns = nullptr;

	void LibDataHolder::setLibraryData(WolframLibraryData ld) {
		libData = ld? std::make_unique<st_WolframLibraryData>(*ld) : nullptr;
		raFuns = ld? libData->rawarrayLibraryFunctions : nullptr;
		imgFuns = ld? libData->imageLibraryFunctions : nullptr;
	}

	WolframLibraryData LibDataHolder::getLibraryData() noexcept {
		return libData.get();
	}

}
