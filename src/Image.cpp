/** 
 * @file	Image.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	18/04/2017
 *
 * @brief	Template specializations of Image::type attribute for all data types that we want to support
 *
 */

#include "Image.h"

#include <cstdint>

namespace LibraryLinkUtils {

	template<>
	const imagedata_t Image<int8_t>::type = MImage_Type_Bit;

	template<>
	const imagedata_t Image<uint8_t>::type = MImage_Type_Bit8;

	template<>
	const imagedata_t Image<uint16_t>::type = MImage_Type_Bit16;

	template<>
	const imagedata_t Image<float>::type = MImage_Type_Real32;

	template<>
	const imagedata_t Image<double>::type = MImage_Type_Real;

} /* namespace LibraryLinkUtils */

