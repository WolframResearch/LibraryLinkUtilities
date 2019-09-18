/** 
 * @file	Release.h
 * @date	Nov 28, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Header file with classes responsible for releasing memory allocated by MathLink when receiving data.
 */
#ifndef LLUTILS_ML_MLRELEASE_H_
#define LLUTILS_ML_MLRELEASE_H_

#include <functional>

#include "mathlink.h"
#include "LLU/Utilities.hpp"


namespace LLU {

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
		typename ReleaseArray<T>::Func ReleaseArray<T>::Release  = [] (MLINK, T*, int*, char**, int) {
			static_assert(dependent_false_v<T>, "Trying to use ML::ReleaseArray<T>::Release for unsupported type T");
		};

		template<typename T>
		typename ReleaseList<T>::Func ReleaseList<T>::Release  = [] (MLINK, T*, int) {
			static_assert(dependent_false_v<T>, "Trying to use ML::ReleaseList<T>::Release for unsupported type T");
		};

#ifndef _WIN32

#define ML_RELEASE_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(T) \
	template<> ReleaseArray<T>::Func ReleaseArray<T>::Release;\
	template<> ReleaseList<T>::Func ReleaseList<T>::Release;

		ML_RELEASE_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(unsigned char)
		ML_RELEASE_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(short)
		ML_RELEASE_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(int)
		ML_RELEASE_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(mlint64)
		ML_RELEASE_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(float)
		ML_RELEASE_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(double)
#else

		template<>
		ReleaseArray<unsigned char>::Func ReleaseArray<unsigned char>::Release = MLReleaseInteger8Array;

		template<>
		ReleaseList<unsigned char>::Func ReleaseList<unsigned char>::Release = MLReleaseInteger8List;

		template<>
		ReleaseArray<short>::Func ReleaseArray<short>::Release = MLReleaseInteger16Array;

		template<>
		ReleaseList<short>::Func ReleaseList<short>::Release = MLReleaseInteger16List;

		template<>
		ReleaseArray<int>::Func ReleaseArray<int>::Release = MLReleaseInteger32Array;

		template<>
		ReleaseList<int>::Func ReleaseList<int>::Release = MLReleaseInteger32List;

		template<>
		ReleaseArray<mlint64>::Func ReleaseArray<mlint64>::Release = MLReleaseInteger64Array;

		template<>
		ReleaseList<mlint64>::Func ReleaseList<mlint64>::Release = MLReleaseInteger64List;

		template<>
		ReleaseArray<float>::Func ReleaseArray<float>::Release = MLReleaseReal32Array;

		template<>
		ReleaseList<float>::Func ReleaseList<float>::Release = MLReleaseReal32List;

		template<>
		ReleaseArray<double>::Func ReleaseArray<double>::Release = MLReleaseReal64Array;

		template<>
		ReleaseList<double>::Func ReleaseList<double>::Release = MLReleaseReal64List;
#endif
	} /* namespace ML */

} /* namespace LLU */

#endif /* LLUTILS_ML_MLRELEASE_H_ */
