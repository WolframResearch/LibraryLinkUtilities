/** 
 * @file	MLPut.h
 * @date	Nov 28, 2017
 * @author	rafal
 * @brief	<brief description>
 */
#ifndef MLPUT_H_
#define MLPUT_H_

#include <functional>

#include "Utilities.h"

namespace LibraryLinkUtils {

	namespace ML {

		template<typename T>
		struct PutList {
			using Func = std::function<int(MLINK, const T*, int)>;

			static void put(MLINK m, const T* list, int len) {
				checkError(m, ListF(m, list, len), LLErrorCode::MLPutListError, ListFName);
			}

		private:
			static const std::string ListFName;
			static Func ListF;
		};

		template<typename T>
		struct PutScalar {
			using Func = std::function<int(MLINK, T)>;

			static void put(MLINK m, T scalar) {
				checkError(m, ScalarF(m, scalar), LLErrorCode::MLPutScalarError, ScalarFName);
			}

		private:
			static const std::string ScalarFName;
			static Func ScalarF;
		};

		template<typename T>
		struct PutString {
			using Func = std::function<int(MLINK, const T*, int)>;

			static void put(MLINK m, const T* string, int len) {
				checkError(m, StringF(m, string, len), LLErrorCode::MLPutStringError, StringFName);
			}

		private:
			static const std::string StringFName;
			static Func StringF;
		};

	}

} /* namespace LibraryLinkUtils */

#endif /* MLPUT_H_ */
