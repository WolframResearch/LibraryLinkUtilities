/**
 * @file
 * Definition of the LibraryData class.
 */
#ifndef LLUTILS_LIBRARYDATA_H
#define LLUTILS_LIBRARYDATA_H

#include <memory>

#include "WolframIOLibraryFunctions.h"
#include "WolframImageLibrary.h"
#include "WolframLibrary.h"
#include "WolframNumericArrayLibrary.h"
#include "WolframSparseLibrary.h"

namespace LLU {

	/**
	 * @struct 	LibraryData
	 * @brief	This structure offers a static copy of WolframLibData accessible throughout the whole life of the DLL.
	 */
	struct LibraryData {
		/**
		 *   @brief     Set WolframLibraryData structure as static member of LibDataHolder. Call this function in WolframLibrary_initialize.
		 *   @param[in] ld - WolframLibraryData passed to every library function via LibraryLink
		 *   @warning	This function must be called before constructing the first MArgumentManager
		 *   unless you use a constructor that takes WolframLibraryData as argument
		 **/
		static void setLibraryData(WolframLibraryData ld);

		/**
		 * @brief   Check if libData is populated
		 * @return  true iff the libData is not a nullptr
		 */
		static bool hasLibraryData();

		/**
		 *   @brief     Get currently owned WolframLibraryData.
		 *   @return    a non-owning pointer to current instance of st_WolframLibraryData statically stored by LibraryData
		 *   @throws    ErrorName::LibDataError - if libData is nullptr
		 **/
		static WolframLibraryData API();

		/**
		 * @brief   Get a pointer to structure with function pointers to MNumericArray API
		 * @return  a pointer to raw LibraryLink MNumericArray API
		 */
		static const st_WolframNumericArrayLibrary_Functions* NumericArrayAPI();

		/**
		 * @brief   Get a pointer to structure with function pointers to MSparseArray API
		 * @return  a pointer to raw LibraryLink MSparseArray API
		 */
		static const st_WolframSparseLibrary_Functions* SparseArrayAPI();

		/**
		 * @brief   Get a pointer to structure with function pointers to MImage API
		 * @return  a pointer to raw LibraryLink MImage API
		 */
		static const st_WolframImageLibrary_Functions* ImageAPI();

		/**
		 * @brief   Get a pointer to structure with function pointers to DataStore API
		 * @return  a pointer to raw LibraryLink DataStore API
		 */
		static const st_WolframIOLibrary_Functions* DataStoreAPI();

	private:
		/// A copy of WolframLibraryData that will be accessible to all parts of LLU
		static WolframLibraryData libData;
	};

}

#endif	  // LLUTILS_LIBRARYDATA_H
