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
		struct PutArray {
			using Func = std::function<int(MLINK, const T*, const int*, const char**, int)>;

			static void put(MLINK m, const T* array, const int* dims, const char** heads, int len) {
				checkError(m, ArrayF(m, array, dims, heads, len), LLErrorCode::MLPutArrayError, ArrayFName);
			}

			static void put(MLINK m, const T* array, const int* dims, char** heads, int len) {
				checkError(m, ArrayF(m, array, dims, const_cast<const char**>(heads), len), LLErrorCode::MLPutArrayError, ArrayFName);
			}

		private:
			static const std::string ArrayFName;
			static Func ArrayF;
		};

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


		template<typename T>
		typename PutArray<T>::Func PutArray<T>::ArrayF = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::PutArray<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename PutList<T>::Func PutList<T>::ListF = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::PutList<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename PutString<T>::Func PutString<T>::StringF = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::PutString<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename PutScalar<T>::Func PutScalar<T>::ScalarF = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::PutScalar<T> for unsupported type T");
			return 0;
		};



		template<> PutString<char>::Func PutString<char>::StringF;
		template<> const std::string PutString<char>::StringFName;


		template<> PutArray<unsigned char>::Func PutArray<unsigned char>::ArrayF;
		template<> const std::string PutArray<unsigned char>::ArrayFName;

		template<> PutList<unsigned char>::Func PutList<unsigned char>::ListF;
		template<> const std::string PutList<unsigned char>::ListFName;

		template<> PutString<unsigned char>::Func PutString<unsigned char>::StringF;
		template<> const std::string PutString<unsigned char>::StringFName;

		template<> PutScalar<unsigned char>::Func PutScalar<unsigned char>::ScalarF;
		template<> const std::string PutScalar<unsigned char>::ScalarFName;


		template<> PutArray<short>::Func PutArray<short>::ArrayF;
		template<> const std::string PutArray<short>::ArrayFName;

		template<> PutList<short>::Func PutList<short>::ListF;
		template<> const std::string PutList<short>::ListFName;

		template<> PutString<unsigned short>::Func PutString<unsigned short>::StringF;
		template<> const std::string PutString<unsigned short>::StringFName;

		template<> PutScalar<short>::Func PutScalar<short>::ScalarF;
		template<> const std::string PutScalar<short>::ScalarFName;


		template<> PutArray<int>::Func PutArray<int>::ArrayF;
		template<> const std::string PutArray<int>::ArrayFName;

		template<> PutList<int>::Func PutList<int>::ListF;
		template<> const std::string PutList<int>::ListFName;

		template<> PutString<unsigned int>::Func PutString<unsigned int>::StringF;
		template<> const std::string PutString<unsigned int>::StringFName;

		template<> PutScalar<int>::Func PutScalar<int>::ScalarF;
		template<> const std::string PutScalar<int>::ScalarFName;


		template<> PutArray<mlint64>::Func PutArray<mlint64>::ArrayF;
		template<> const std::string PutArray<mlint64>::ArrayFName;

		template<> PutList<mlint64>::Func PutList<mlint64>::ListF;
		template<> const std::string PutList<mlint64>::ListFName;

		template<> PutScalar<mlint64>::Func PutScalar<mlint64>::ScalarF;
		template<> const std::string PutScalar<mlint64>::ScalarFName;


		template<> PutArray<float>::Func PutArray<float>::ArrayF;
		template<> const std::string PutArray<float>::ArrayFName;

		template<> PutList<float>::Func PutList<float>::ListF;
		template<> const std::string PutList<float>::ListFName;

		template<> PutScalar<float>::Func PutScalar<float>::ScalarF;
		template<> const std::string PutScalar<float>::ScalarFName;


		template<> PutArray<double>::Func PutArray<double>::ArrayF;
		template<> const std::string PutArray<double>::ArrayFName;

		template<> PutList<double>::Func PutList<double>::ListF;
		template<> const std::string PutList<double>::ListFName;

		template<> PutScalar<double>::Func PutScalar<double>::ScalarF;
		template<> const std::string PutScalar<double>::ScalarFName;
	}

} /* namespace LibraryLinkUtils */

#endif /* MLPUT_H_ */
