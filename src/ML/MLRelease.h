/** 
 * @file	MLRelease.h
 * @date	Nov 28, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Header file with classes responsible for releasing memory allocated by MathLink when receiving data.
 */
#ifndef LLUTILS_ML_MLRELEASE_H_
#define LLUTILS_ML_MLRELEASE_H_

#include <functional>

#include "mathlink.h"

namespace LibraryLinkUtils {

	namespace ML {

		template<typename T>
		struct ReleaseList {
			using Func = std::function<void(MLINK, T*, int)>;

			ReleaseList() = default;
			ReleaseList(MLINK m, int l) : m(m), length(l) {}

			void operator()(T* data) {
				Release(m, data, length);
			}

			int getLength() const {
				return length;
			}

		private:
			static Func Release;

			MLINK m = nullptr;
			int length = 0;
		};


		template<typename T>
		struct ReleaseString {
			using Func = std::function<void(MLINK, const T*, int)>;

			ReleaseString() = default;
			ReleaseString(MLINK m, int l, int c) : m(m), length(l), chars(c) {}

			void operator()(const T* data) {
				Release(m, data, length);
			}

			int getLength() const {
				return length;
			}

			int getCharacters() const {
				return chars;
			}
		private:
			static Func Release;

			MLINK m = nullptr;
			int length = 0;
			int chars = 0;
		};

		template<typename T>
		struct ReleaseArray {
			using Func = std::function<void(MLINK, T*, int*, char**, int)>;

			ReleaseArray() = default;
			ReleaseArray(MLINK m, int* d, char** h, int r) : m(m), dims(d), heads(h), rank(r) {}

			void operator()(T* data) {
				Release(m, data, dims, heads, rank);
			}

			int* getDims() const {
				return dims;
			}

			char** getHeads() const {
				return heads;
			}

			int getRank() const {
				return rank;
			}
		private:
			static Func Release;

			MLINK m = nullptr;
			int* dims = nullptr;
			char** heads = nullptr;
			int rank = 0;
		};

		template<typename T>
		typename ReleaseArray<T>::Func ReleaseArray<T>::Release  = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::ReleaseArray<T>::Release for unsupported type T");
		};

		template<typename T>
		typename ReleaseList<T>::Func ReleaseList<T>::Release  = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::ReleaseList<T>::Release for unsupported type T");
		};

		template<typename T>
		typename ReleaseString<T>::Func ReleaseString<T>::Release = [] (auto&&...) {
			static_assert(sizeof(T) < 0, "Trying to use ML::ReleaseString<T>::Release for unsupported type T");
		};


		template<> ReleaseString<char>::Func ReleaseString<char>::Release;


		template<> ReleaseArray<unsigned char>::Func ReleaseArray<unsigned char>::Release;

		template<> ReleaseList<unsigned char>::Func ReleaseList<unsigned char>::Release;

		template<> ReleaseString<unsigned char>::Func ReleaseString<unsigned char>::Release;


		template<> ReleaseArray<short>::Func ReleaseArray<short>::Release;

		template<> ReleaseList<short>::Func ReleaseList<short>::Release;

		template<> ReleaseString<unsigned short>::Func ReleaseString<unsigned short>::Release;


		template<> ReleaseArray<int>::Func ReleaseArray<int>::Release;

		template<> ReleaseList<int>::Func ReleaseList<int>::Release;

		template<> ReleaseString<unsigned int>::Func ReleaseString<unsigned int>::Release;


		template<> ReleaseArray<mlint64>::Func ReleaseArray<mlint64>::Release;

		template<> ReleaseList<mlint64>::Func ReleaseList<mlint64>::Release;


		template<> ReleaseArray<float>::Func ReleaseArray<float>::Release;

		template<> ReleaseList<float>::Func ReleaseList<float>::Release;


		template<> ReleaseArray<double>::Func ReleaseArray<double>::Release;

		template<> ReleaseList<double>::Func ReleaseList<double>::Release;

	}
} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_ML_MLRELEASE_H_ */
