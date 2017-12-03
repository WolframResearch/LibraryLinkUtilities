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
		using StringData = std::unique_ptr<const T[], ReleaseString<T>>;

		template<typename T>
		using ListData = std::unique_ptr<T[], ReleaseList<T>>;

		template<typename T>
		using ArrayData = std::unique_ptr<T[], ReleaseArray<T>>;

		template<typename T>
		struct GetArray {
			using Func = std::function<int(MLINK, T**, int**, char***, int*)>;

			static ArrayData<T> get(MLINK m) {
				T* rawResult;
				int* dims;
				char** heads;
				int rank;
				checkError(m, ArrayF(m, &rawResult, &dims, &heads, &rank), LLErrorCode::MLGetArrayError, ArrayFName);
				return { rawResult, ReleaseArray<T> { m, dims, heads, rank } };
			}

		private:
			static const std::string ArrayFName;
			static Func ArrayF;
		};

		template<typename T>
		struct GetList {
			using Func = std::function<int(MLINK, T**, int*)>;

			static ListData<T> get(MLINK m) {
				T* rawResult;
				int len;
				checkError(m, ListF(m, &rawResult, &len), LLErrorCode::MLGetListError, ListFName);
				return { rawResult, ReleaseList<T> { m, len } };
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

			static StringData<T> get(MLINK m) {
				const T* rawResult;
				int bytes, characters;
				checkError(m, StringF(m, &rawResult, &bytes, &characters), LLErrorCode::MLGetStringError, StringFName);
				return { rawResult, ReleaseString<T> { m, bytes , characters} };
			}

		private:
			static const std::string StringFName;
			static Func StringF;
		};
	}

} /* namespace LibraryLinkUtils */

#endif /* MLGET_H_ */
