/** 
 * @file	Get.cpp
 * @date	Nov 28, 2017
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Implementation file with classes related to reading data from MathLink.
 */
#ifndef _WIN32

#include "LLU/ML/Get.h"

#include "mathlink.h"

namespace LibraryLinkUtils::ML {


    /* ***************************************************************** */
    /* ********* Template specializations for  unsigned char  ********** */
    /* ***************************************************************** */

    /* GetArray */

    template<>
    GetArray<unsigned char>::Func GetArray<unsigned char>::ArrayF = MLGetInteger8Array;

    template<>
    const std::string GetArray<unsigned char>::ArrayFName = "MLGetInteger8Array";

    /* GetList */

    template<>
    GetList<unsigned char>::Func GetList<unsigned char>::ListF = MLGetInteger8List;

    template<>
    const std::string GetList<unsigned char>::ListFName = "MLGetInteger8List";

    /* GetScalar */

    template<>
    GetScalar<unsigned char>::Func GetScalar<unsigned char>::ScalarF = MLGetInteger8;

    template<>
    const std::string GetScalar<unsigned char>::ScalarFName = "MLGetInteger8";


    /* ***************************************************************** */
    /* ******* Template specializations for  (unsigned) short  ********* */
    /* ***************************************************************** */

    /* GetArray */

    template<>
    GetArray<short>::Func GetArray<short>::ArrayF = MLGetInteger16Array;

    template<>
    const std::string GetArray<short>::ArrayFName = "MLGetInteger16Array";

    /* GetList */

    template<>
    GetList<short>::Func GetList<short>::ListF = MLGetInteger16List;

    template<>
    const std::string GetList<short>::ListFName = "MLGetInteger16List";

    /* GetScalar */

    template<>
    GetScalar<short>::Func GetScalar<short>::ScalarF = MLGetInteger16;

    template<>
    const std::string GetScalar<short>::ScalarFName = "MLGetInteger16";


    /* ***************************************************************** */
    /* ******** Template specializations for  (unsigned) int  ********** */
    /* ***************************************************************** */

    /* GetArray */

    template<>
    GetArray<int>::Func GetArray<int>::ArrayF = MLGetInteger32Array;

    template<>
    const std::string GetArray<int>::ArrayFName = "MLGetInteger32Array";

    /* GetList */

    template<>
    GetList<int>::Func GetList<int>::ListF = MLGetInteger32List;

    template<>
    const std::string GetList<int>::ListFName = "MLGetInteger32List";

    /* GetScalar */

    template<>
    GetScalar<int>::Func GetScalar<int>::ScalarF = MLGetInteger32;

    template<>
    const std::string GetScalar<int>::ScalarFName = "MLGetInteger32";


    /* ***************************************************************** */
    /* *********** Template specializations for  mlint64  ************** */
    /* ***************************************************************** */

    /* GetArray */

    template<>
    GetArray<mlint64>::Func GetArray<mlint64>::ArrayF = MLGetInteger64Array;

    template<>
    const std::string GetArray<mlint64>::ArrayFName = "MLGetInteger64Array";

    /* GetList */

    template<>
    GetList<mlint64>::Func GetList<mlint64>::ListF = MLGetInteger64List;

    template<>
    const std::string GetList<mlint64>::ListFName = "MLGetInteger64List";

    /* GetScalar */

    template<>
    GetScalar<mlint64>::Func GetScalar<mlint64>::ScalarF = MLGetInteger64;

    template<>
    const std::string GetScalar<mlint64>::ScalarFName = "MLGetInteger64";


    /* ***************************************************************** */
    /* ************ Template specializations for  float  *************** */
    /* ***************************************************************** */

    /* GetArray */

    template<>
    GetArray<float>::Func GetArray<float>::ArrayF = MLGetReal32Array;

    template<>
    const std::string GetArray<float>::ArrayFName = "MLGetReal32Array";

    /* GetList */

    template<>
    GetList<float>::Func GetList<float>::ListF = MLGetReal32List;

    template<>
    const std::string GetList<float>::ListFName = "MLGetReal32List";

    /* GetScalar */

    template<>
    GetScalar<float>::Func GetScalar<float>::ScalarF = MLGetReal32;

    template<>
    const std::string GetScalar<float>::ScalarFName = "MLGetReal32";


    /* ***************************************************************** */
    /* *********** Template specializations for  double  *************** */
    /* ***************************************************************** */

    /* GetArray */

    template<>
    GetArray<double>::Func GetArray<double>::ArrayF = MLGetReal64Array;

    template<>
    const std::string GetArray<double>::ArrayFName = "MLGetReal64Array";

    /* GetList */

    template<>
    GetList<double>::Func GetList<double>::ListF = MLGetReal64List;

    template<>
    const std::string GetList<double>::ListFName = "MLGetReal64List";

    /* GetScalar */

    template<>
    GetScalar<double>::Func GetScalar<double>::ScalarF = MLGetReal64;

    template<>
    const std::string GetScalar<double>::ScalarFName = "MLGetReal64";

} /* namespace LibraryLinkUtils::ML */

#endif
