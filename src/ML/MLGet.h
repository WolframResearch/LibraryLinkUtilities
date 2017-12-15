/** 
 * @file	MLGet.h
 * @date	Nov 28, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Header file with classes related to reading data from MathLink.
 */
#ifndef LLUTILS_ML_MLGET_H_
#define LLUTILS_ML_MLGET_H_

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



		template<typename T>
		typename GetArray<T>::Func GetArray<T>::ArrayF  = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::GetArray<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename GetList<T>::Func GetList<T>::ListF  = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::GetList<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename GetString<T>::Func GetString<T>::StringF = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::GetString<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename GetScalar<T>::Func GetScalar<T>::ScalarF = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::GetScalar<T> for unsupported type T");
			return 0;
		};


		template<> GetString<char>::Func GetString<char>::StringF;
		template<> const std::string GetString<char>::StringFName;


		template<> GetArray<unsigned char>::Func GetArray<unsigned char>::ArrayF;
		template<> const std::string GetArray<unsigned char>::ArrayFName;

		template<> GetList<unsigned char>::Func GetList<unsigned char>::ListF;
		template<> const std::string GetList<unsigned char>::ListFName;

		template<> GetString<unsigned char>::Func GetString<unsigned char>::StringF;
		template<> const std::string GetString<unsigned char>::StringFName;

		template<> GetScalar<unsigned char>::Func GetScalar<unsigned char>::ScalarF;
		template<> const std::string GetScalar<unsigned char>::ScalarFName;


		template<> GetArray<short>::Func GetArray<short>::ArrayF;
		template<> const std::string GetArray<short>::ArrayFName;

		template<> GetList<short>::Func GetList<short>::ListF;
		template<> const std::string GetList<short>::ListFName;

		template<> GetString<unsigned short>::Func GetString<unsigned short>::StringF;
		template<> const std::string GetString<unsigned short>::StringFName;

		template<> GetScalar<short>::Func GetScalar<short>::ScalarF;
		template<> const std::string GetScalar<short>::ScalarFName;


		template<> GetArray<int>::Func GetArray<int>::ArrayF;
		template<> const std::string GetArray<int>::ArrayFName;

		template<> GetList<int>::Func GetList<int>::ListF;
		template<> const std::string GetList<int>::ListFName;

		template<> GetString<unsigned int>::Func GetString<unsigned int>::StringF;
		template<> const std::string GetString<unsigned int>::StringFName;

		template<> GetScalar<int>::Func GetScalar<int>::ScalarF;
		template<> const std::string GetScalar<int>::ScalarFName;


		template<> GetArray<mlint64>::Func GetArray<mlint64>::ArrayF;
		template<> const std::string GetArray<mlint64>::ArrayFName;

		template<> GetList<mlint64>::Func GetList<mlint64>::ListF;
		template<> const std::string GetList<mlint64>::ListFName;

		template<> GetScalar<mlint64>::Func GetScalar<mlint64>::ScalarF;
		template<> const std::string GetScalar<mlint64>::ScalarFName;



		template<> GetArray<float>::Func GetArray<float>::ArrayF;
		template<> const std::string GetArray<float>::ArrayFName;

		template<> GetList<float>::Func GetList<float>::ListF;
		template<> const std::string GetList<float>::ListFName;

		template<> GetScalar<float>::Func GetScalar<float>::ScalarF;
		template<> const std::string GetScalar<float>::ScalarFName;


		template<> GetArray<double>::Func GetArray<double>::ArrayF;
		template<> const std::string GetArray<double>::ArrayFName;

		template<> GetList<double>::Func GetList<double>::ListF;
		template<> const std::string GetList<double>::ListFName;

		template<> GetScalar<double>::Func GetScalar<double>::ScalarF;
		template<> const std::string GetScalar<double>::ScalarFName;

	}

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_ML_MLGET_H_ */
