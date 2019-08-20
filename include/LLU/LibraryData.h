/**
 * @file
 * Definition of the LibraryData class.
 */
#ifndef LLUTILS_LIBRARYDATA_H
#define LLUTILS_LIBRARYDATA_H

#include <memory>

#include "WolframLibrary.h"
#include "WolframImageLibrary.h"
#include "WolframNumericArrayLibrary.h"
#include "WolframIOLibraryFunctions.h"
#include "WolframSparseLibrary.h"

namespace LLU {

	/**
	 * @struct 	LibraryData
	 * @brief	This structure offers a static copy of WolframLibData accessible throughout the whole life of the DLL.
	 */
	struct LibraryData {
		/**
		 *   @brief         Set WolframLibraryData structure as static member of LibDataHolder. Call this function in WolframLibrary_initialize.
		 *   @param[in]     ld - WolframLibraryData passed to every library function via LibraryLink
		 *   @warning		This function must be called before constructing the first MArgumentManager unless you use a constructor that takes WolframLibraryData as argument
		 **/
		static void setLibraryData(WolframLibraryData ld);

		/**
		 *
		 * @return
		 */
		static bool hasLibraryData();

		/**
		 *   @brief         Get currently owned WolframLibraryData.
		 *   @return     	a non-owning pointer to current instance of st_WolframLibraryData stored in LibDataHolder or nullptr in case there is none
		 **/
		static WolframLibraryData API();

		static const st_WolframNumericArrayLibrary_Functions* NumericArrayAPI();

		static const st_WolframSparseLibrary_Functions* SparseArrayAPI();

		static const st_WolframImageLibrary_Functions* ImageAPI();

		static const st_WolframIOLibrary_Functions* DataStoreAPI();

	private:
		static WolframLibraryData libData;
	};

}

#endif //LLUTILS_LIBRARYDATA_H
