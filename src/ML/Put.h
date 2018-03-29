/** 
 * @file	Put.h
 * @date	Nov 28, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Header file with classes related to sending data through MathLink.
 */
#ifndef LLUTILS_ML_MLPUT_H_
#define LLUTILS_ML_MLPUT_H_

#include <functional>

#include "mathlink.h"

#include "../LibraryLinkError.h"

namespace LibraryLinkUtils {

	namespace ML {

		template<typename T>
		struct PutArray {
			using Func = std::function<int(MLINK, const T*, const int*, const char**, int)>;

			static void put(MLINK m, const T* array, const int* dims, const char** heads, int len) {
				checkError(m, ArrayF(m, array, dims, heads, len), LLErrorName::MLPutArrayError, ArrayFName);
			}

			static void put(MLINK m, const T* array, const int* dims, char** heads, int len) {
				checkError(m, ArrayF(m, array, dims, const_cast<const char**>(heads), len), LLErrorName::MLPutArrayError, ArrayFName);
			}

		private:
			static const std::string ArrayFName;
			static Func ArrayF;
		};

		template<typename T>
		struct PutList {
			using Func = std::function<int(MLINK, const T*, int)>;

			static void put(MLINK m, const T* list, int len) {
				checkError(m, ListF(m, list, len), LLErrorName::MLPutListError, ListFName);
			}

		private:
			static const std::string ListFName;
			static Func ListF;
		};

		template<typename T>
		struct PutScalar {
			using Func = std::function<int(MLINK, T)>;

			static void put(MLINK m, T scalar) {
				checkError(m, ScalarF(m, scalar), LLErrorName::MLPutScalarError, ScalarFName);
			}

		private:
			static const std::string ScalarFName;
			static Func ScalarF;
		};


		template<typename T>
		typename PutArray<T>::Func PutArray<T>::ArrayF = [] (MLINK, const T*, const int*, const char**, int) {
			static_assert(sizeof(T) < 0, "Trying to use ML::PutArray<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename PutList<T>::Func PutList<T>::ListF = [] (MLINK, const T*, int) {
			static_assert(sizeof(T) < 0, "Trying to use ML::PutList<T> for unsupported type T");
			return 0;
		};

		template<typename T>
		typename PutScalar<T>::Func PutScalar<T>::ScalarF = [] (MLINK, T) {
			static_assert(sizeof(T) < 0, "Trying to use ML::PutScalar<T> for unsupported type T");
			return 0;
		};

#ifndef _WIN32

#define ML_PUT_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(T) \
	template<> PutArray<T>::Func PutArray<T>::ArrayF;\
	template<> const std::string PutArray<T>::ArrayFName;\
	template<> PutList<T>::Func PutList<T>::ListF;\
	template<> const std::string PutList<T>::ListFName;\
	template<> PutScalar<T>::Func PutScalar<T>::ScalarF;\
	template<> const std::string PutScalar<T>::ScalarFName;


		ML_PUT_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(unsigned char)
		ML_PUT_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(short)
		ML_PUT_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(int)
		ML_PUT_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(mlint64)
		ML_PUT_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(float)
		ML_PUT_DECLARE_SPECIALIZATIONS_OF_STATIC_MEMBERS(double)


#else

		/* ***************************************************************** */
		/* ********* Template specializations for  unsigned char  ********** */
		/* ***************************************************************** */

		/* PutArray */

		template<>
		PutArray<unsigned char>::Func PutArray<unsigned char>::ArrayF = MLPutInteger8Array;

		template<>
		const std::string PutArray<unsigned char>::ArrayFName = "MLPutInteger8Array";

		/* PutList */

		template<>
		PutList<unsigned char>::Func PutList<unsigned char>::ListF = MLPutInteger8List;

		template<>
		const std::string PutList<unsigned char>::ListFName = "MLPutInteger8List";

		/* PutScalar */

		template<>
		PutScalar<unsigned char>::Func PutScalar<unsigned char>::ScalarF = MLPutInteger8;

		template<>
		const std::string PutScalar<unsigned char>::ScalarFName = "MLPutInteger8";


		/* ***************************************************************** */
		/* ******* Template specializations for  (unsigned) short  ********* */
		/* ***************************************************************** */

		/* PutArray */

		template<>
		PutArray<short>::Func PutArray<short>::ArrayF = MLPutInteger16Array;

		template<>
		const std::string PutArray<short>::ArrayFName = "MLPutInteger16Array";

		/* PutList */

		template<>
		PutList<short>::Func PutList<short>::ListF = MLPutInteger16List;

		template<>
		const std::string PutList<short>::ListFName = "MLPutInteger16List";

		/* PutScalar */

		template<>
		PutScalar<short>::Func PutScalar<short>::ScalarF = MLPutInteger16;

		template<>
		const std::string PutScalar<short>::ScalarFName = "MLPutInteger16";


		/* ***************************************************************** */
		/* ******** Template specializations for  (unsigned) int  ********** */
		/* ***************************************************************** */

		/* PutArray */

		template<>
		PutArray<int>::Func PutArray<int>::ArrayF = MLPutInteger32Array;

		template<>
		const std::string PutArray<int>::ArrayFName = "MLPutInteger32Array";

		/* PutList */

		template<>
		PutList<int>::Func PutList<int>::ListF = MLPutInteger32List;

		template<>
		const std::string PutList<int>::ListFName = "MLPutInteger32List";

		/* PutScalar */

		template<>
		PutScalar<int>::Func PutScalar<int>::ScalarF = MLPutInteger32;

		template<>
		const std::string PutScalar<int>::ScalarFName = "MLPutInteger32";


		/* ***************************************************************** */
		/* *********** Template specializations for  mlint64  ************** */
		/* ***************************************************************** */

		/* PutArray */

		template<>
		PutArray<mlint64>::Func PutArray<mlint64>::ArrayF = MLPutInteger64Array;

		template<>
		const std::string PutArray<mlint64>::ArrayFName = "MLPutInteger64Array";

		/* PutList */

		template<>
		PutList<mlint64>::Func PutList<mlint64>::ListF = MLPutInteger64List;

		template<>
		const std::string PutList<mlint64>::ListFName = "MLPutInteger64List";

		/* PutScalar */

		template<>
		PutScalar<mlint64>::Func PutScalar<mlint64>::ScalarF = MLPutInteger64;

		template<>
		const std::string PutScalar<mlint64>::ScalarFName = "MLPutInteger64";


		/* ***************************************************************** */
		/* ************ Template specializations for  float  *************** */
		/* ***************************************************************** */

		/* PutArray */

		template<>
		PutArray<float>::Func PutArray<float>::ArrayF = MLPutReal32Array;

		template<>
		const std::string PutArray<float>::ArrayFName = "MLPutReal32Array";

		/* PutList */

		template<>
		PutList<float>::Func PutList<float>::ListF = MLPutReal32List;

		template<>
		const std::string PutList<float>::ListFName = "MLPutReal32List";

		/* PutScalar */

		template<>
		PutScalar<float>::Func PutScalar<float>::ScalarF = MLPutReal32;

		template<>
		const std::string PutScalar<float>::ScalarFName = "MLPutReal32";


		/* ***************************************************************** */
		/* *********** Template specializations for  double  *************** */
		/* ***************************************************************** */

		/* PutArray */

		template<>
		PutArray<double>::Func PutArray<double>::ArrayF = MLPutReal64Array;

		template<>
		const std::string PutArray<double>::ArrayFName = "MLPutReal64Array";

		/* PutList */

		template<>
		PutList<double>::Func PutList<double>::ListF = MLPutReal64List;

		template<>
		const std::string PutList<double>::ListFName = "MLPutReal64List";

		/* PutScalar */

		template<>
		PutScalar<double>::Func PutScalar<double>::ScalarF = MLPutReal64;

		template<>
		const std::string PutScalar<double>::ScalarFName = "MLPutReal64";

#endif
	}  /* namespace ML */

} /* namespace LibraryLinkUtils */

#endif /* LLUTILS_ML_MLPUT_H_ */
