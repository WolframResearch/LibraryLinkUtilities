/** 
 * @file	Image.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	18/04/2017
 *
 * @brief	Template specializations of Image::type attribute for all data types that we want to support
 *
 */

#include "LLU/Containers/Image.h"

#include <cstdint>

namespace LLU {
/// @cond
	//
	//	Template specializations for Bit images
	//

	template<>
	const imagedata_t TypedImage<int8_t>::type = MImage_Type_Bit;

	template<>
	int8_t TypedImage<int8_t>::getValueAt(mint* position, mint channel) const {
		raw_t_bit res;
		if (LibraryData::ImageAPI()->MImage_getBit(this->getInternal(), position, channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	void TypedImage<int8_t>::setValueAt(mint* position, mint channel, int8_t newValue) {
		if (LibraryData::ImageAPI()->MImage_setBit(this->getInternal(), position, channel, newValue)) {
			this->indexError();
		}
	}

	//
	//	Template specializations for Byte images
	//

	template<>
	const imagedata_t TypedImage<uint8_t>::type = MImage_Type_Bit8;

	template<>
	uint8_t TypedImage<uint8_t>::getValueAt(mint* position, mint channel) const {
		raw_t_ubit8 res;
		if (LibraryData::ImageAPI()->MImage_getByte(this->getInternal(), position, channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	void TypedImage<uint8_t>::setValueAt(mint* position, mint channel, uint8_t newValue) {
		if (LibraryData::ImageAPI()->MImage_setByte(this->getInternal(), position, channel, newValue)) {
			this->indexError();
		}
	}

	//
	//	Template specializations for Bit16 images
	//

	template<>
	const imagedata_t TypedImage<uint16_t>::type = MImage_Type_Bit16;

	template<>
	uint16_t TypedImage<uint16_t>::getValueAt(mint* position, mint channel) const {
		raw_t_ubit16 res;
		if (LibraryData::ImageAPI()->MImage_getBit16(this->getInternal(), position, channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	void TypedImage<uint16_t>::setValueAt(mint* position, mint channel, uint16_t newValue) {
		if (LibraryData::ImageAPI()->MImage_setBit16(this->getInternal(), position, channel, newValue)) {
			this->indexError();
		}
	}

	//
	//	Template specializations for Real32 images
	//

	template<>
	const imagedata_t TypedImage<float>::type = MImage_Type_Real32;

	template<>
	float TypedImage<float>::getValueAt(mint* position, mint channel) const {
		raw_t_real32 res;
		if (LibraryData::ImageAPI()->MImage_getReal32(this->getInternal(), position, channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	void TypedImage<float>::setValueAt(mint* position, mint channel, float newValue) {
		if (LibraryData::ImageAPI()->MImage_setReal32(this->getInternal(), position, channel, newValue)) {
			this->indexError();
		}
	}

	//
	//	Template specializations for Real images
	//

	template<>
	const imagedata_t TypedImage<double>::type = MImage_Type_Real;

	template<>
	double TypedImage<double>::getValueAt(mint* position, mint channel) const {
		raw_t_real64 res;
		if (LibraryData::ImageAPI()->MImage_getReal(this->getInternal(), position, channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	void TypedImage<double>::setValueAt(mint* position, mint channel, double newValue) {
		if (LibraryData::ImageAPI()->MImage_setReal(this->getInternal(), position, channel, newValue)) {
			this->indexError();
		}
	}
/// @endcond
} /* namespace LLU */

