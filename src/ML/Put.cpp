/**
 * @file	Put.cpp
 * @date	Nov 28, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Implementation file with classes related to sending data through WSTP.
 */

#ifndef _WIN32

#include "../../include/LLU/ML/Put.h"

#include "wstp.h"

#include "../../include/LLU/Utilities.hpp"

namespace LLU {

	namespace ML {

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

	}

} /* namespace LLU */

#endif
