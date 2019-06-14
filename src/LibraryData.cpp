/**
 * @file	LibDataHolder.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	14/06/2018
 *
 * @brief	Implementation of the LibDataHolder class.
 *
 */

#include "LLU/LibraryData.h"

#include "LLU/ErrorLog/ErrorManager.h"

namespace LLU {

	WolframLibraryData LibraryData::libData = nullptr;

	void LibraryData::setLibraryData(WolframLibraryData ld) {
		libData = ld;
	}

	bool LibraryData::hasLibraryData() {
		return libData != nullptr;
	}

	WolframLibraryData LibraryData::API() {
		if (!libData) {
			ErrorManager::throwException(ErrorName::LibDataError);
		}
		return libData;
	}

	const st_WolframNumericArrayLibrary_Functions * LibraryData::NumericArrayAPI() {
		if (!libData) {
			ErrorManager::throwException(ErrorName::LibDataError);
		}
		return libData->numericarrayLibraryFunctions;
	}

	const st_WolframSparseLibrary_Functions * LibraryData::SparseArrayAPI() {
		if (!libData) {
			ErrorManager::throwException(ErrorName::LibDataError);
		}
		return libData->sparseLibraryFunctions;
	}

	const st_WolframImageLibrary_Functions * LibraryData::ImageAPI() {
		if (!libData) {
			ErrorManager::throwException(ErrorName::LibDataError);
		}
		return libData->imageLibraryFunctions;
	}

	const st_WolframIOLibrary_Functions * LibraryData::DataStoreAPI() {
		if (!libData) {
			ErrorManager::throwException(ErrorName::LibDataError);
		}
		return libData->ioLibraryFunctions;
	}

}
