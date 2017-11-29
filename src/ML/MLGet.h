/** 
 * @file	MLGet.h
 * @date	Nov 28, 2017
 * @author	rafal
 * @brief	<brief description>
 */
#ifndef MLGET_H_
#define MLGET_H_

#include <functional>
#include <memory>
#include <string>

#include "Utilities.h"
#include "MLRelease.h"

namespace LibraryLinkUtils {

	namespace ML {

		template<typename T>
		struct GetList {
			using Func = std::function<int(MLINK, T**, int*)>;
			using MLList = std::unique_ptr<T[], ReleaseList<T>>;

			static MLList get(MLINK m) {
				T* rawResult;
				int len;
				checkError(m, ListF(m, &rawResult, &len), LLErrorCode::MLGetListError, ListFName);
				return MLList { rawResult, ReleaseList<T> { m, len } };
			}

		private:
			static const std::string ListFName;
			static Func ListF;
		};

		template<typename T>
		struct GetScalar {
			using Func = std::function<int(MLINK, T*)>;

			static T get(MLINK m) {
				T rawResult;
				checkError(m, ScalarF(m, &rawResult), LLErrorCode::MLGetScalarError, ScalarFName);
				return rawResult;
			}

		private:
			static const std::string ScalarFName;
			static Func ScalarF;
		};

		template<typename T>
		struct GetString {
			using Func = std::function<int(MLINK, const T**, int*, int*)>;
			using MLString = std::unique_ptr<const T[], ReleaseString<T>>;

			static MLString get(MLINK m) {
				const T* rawResult;
				int bytes, characters;
				checkError(m, StringF(m, &rawResult, &bytes, &characters), LLErrorCode::MLGetStringError, StringFName);
				return MLString { rawResult, ReleaseString<T> { m, bytes , characters} };
			}

		private:
			static const std::string StringFName;
			static Func StringF;
		};
	}

} /* namespace LibraryLinkUtils */

#endif /* MLGET_H_ */
