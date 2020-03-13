/**
 * @file	Strings.cpp
 * @date	Mar 23, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Implementation file for all the functionality related to Strings in WSTP
 */

#include "LLU/WSTP/Strings.h"

#include <algorithm>
#include <type_traits>

namespace LLU::WS {

    /// Definitions of configuration parameters, see header file for detailed description
    namespace EncodingConfig {
        long substituteCodeForByteEncoding = 26;
        bool useFastUTF8 = true;
    }

#ifndef _WIN32
    template<>
    GetStringFuncT<CharType<Encoding::Native>> String<Encoding::Native>::Get = [](WSLINK m, const char** strData, int* len, int* charCnt) {
        *len = *charCnt = -1;
        return WSGetString(m, strData);
    };
    template<>
    PutStringFuncT<CharType<Encoding::Native>> String<Encoding::Native>::Put = [](WSLINK m, const char* strData, int) {
        return WSPutString(m, strData);};

    template<>
    ReleaseStringFuncT<CharType<Encoding::Native>> String<Encoding::Native>::Release = [](WSLINK m, const char* strData, int) {
        WSReleaseString(m, strData);
    };
    template<>
    const std::string String<Encoding::Native>::GetFName = "WSGetString";
    template<>
    const std::string String<Encoding::Native>::PutFName = "WSPutString";


    template<>
    GetStringFuncT<CharType<Encoding::Byte>> String<Encoding::Byte>::Get = [](WSLINK m, const unsigned char** strData, int* len, int* charCnt) {
        *charCnt = -1;
        return WSGetByteString(m, strData, len, EncodingConfig::substituteCodeForByteEncoding);
    };
    template<>
    PutStringFuncT<CharType<Encoding::Byte>> String<Encoding::Byte>::Put = WSPutByteString;
    template<>
    ReleaseStringFuncT<CharType<Encoding::Byte>> String<Encoding::Byte>::Release = WSReleaseByteString;
    template<>
    const std::string String<Encoding::Byte>::GetFName = "WSGetByteString";
    template<>
    const std::string String<Encoding::Byte>::PutFName = "WSPutByteString";

    template<>
    GetStringFuncT<CharType<Encoding::UTF8>> String<Encoding::UTF8>::Get = WSGetUTF8String;
    template<>
    PutStringFuncT<CharType<Encoding::UTF8>> String<Encoding::UTF8>::Put = [](WSLINK m, const unsigned char* strData, int len) {
        if (EncodingConfig::useFastUTF8 && std::all_of(strData, strData + len, [](unsigned char strChar) { return strChar <= 127; })) {
            return WSPutByteString(m, strData, len);
        } else {
            return WSPutUTF8String(m, strData, len);
        }
    };
    template<>
    ReleaseStringFuncT<CharType<Encoding::UTF8>> String<Encoding::UTF8>::Release = WSReleaseUTF8String;
    template<>
    const std::string String<Encoding::UTF8>::GetFName = "WSGetUTF8String";
    template<>
    const std::string String<Encoding::UTF8>::PutFName = "WSPut(UTF8/Byte)String";


    template<>
    GetStringFuncT<CharType<Encoding::UTF16>> String<Encoding::UTF16>::Get = WSGetUTF16String;
    template<>
    PutStringFuncT<CharType<Encoding::UTF16>> String<Encoding::UTF16>::Put = WSPutUTF16String;
    template<>
    ReleaseStringFuncT<CharType<Encoding::UTF16>> String<Encoding::UTF16>::Release = WSReleaseUTF16String;
    template<>
    const std::string String<Encoding::UTF16>::GetFName = "WSGetUTF16String";
    template<>
    const std::string String<Encoding::UTF16>::PutFName = "WSPutUTF16String";


    template<>
    GetStringFuncT<CharType<Encoding::UCS2>> String<Encoding::UCS2>::Get = [](WSLINK m, const unsigned short** strData, int* len, int* charCnt) {
        *charCnt = -1;
        return WSGetUCS2String(m, strData, len);
    };
    template<>
    PutStringFuncT<CharType<Encoding::UCS2>> String<Encoding::UCS2>::Put = WSPutUCS2String;
    template<>
    ReleaseStringFuncT<CharType<Encoding::UCS2>> String<Encoding::UCS2>::Release = WSReleaseUCS2String;
    template<>
    const std::string String<Encoding::UCS2>::GetFName = "WSGetUCS2String";
    template<>
    const std::string String<Encoding::UCS2>::PutFName = "WSPutUCS2String";


    template<>
    GetStringFuncT<CharType<Encoding::UTF32>> String<Encoding::UTF32>::Get = [](WSLINK m, const unsigned int** strData, int* len, int* charCnt) {
        *charCnt = -1;
        return WSGetUTF32String(m, strData, len);
    };
    template<>
    PutStringFuncT<CharType<Encoding::UTF32>> String<Encoding::UTF32>::Put = WSPutUTF32String;
    template<>
    ReleaseStringFuncT<CharType<Encoding::UTF32>> String<Encoding::UTF32>::Release = WSReleaseUTF32String;
    template<>
    const std::string String<Encoding::UTF32>::GetFName = "WSGetUTF32String";
    template<>
    const std::string String<Encoding::UTF32>::PutFName = "WSPutUTF32String";
#endif

} /* namespace LLU::WS */
