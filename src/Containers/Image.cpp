/** 
 * @file	Image.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	18/04/2017
 *
 * @brief	Template specializations of Image::type attribute for all data types that we want to support
 *
 */

#include "LLU/Containers/Image.h"

#include <array>
#include <cstdint>

namespace LibraryLinkUtils {

	//
	//	Template specializations for Bit images
	//

	template<>
	const imagedata_t Image<int8_t>::type = MImage_Type_Bit;

	template<>
	int8_t Image<int8_t>::get(mint row, mint col, mint channel) const {
		raw_t_bit res;
		std::array<mint, 2> pos { { row, col } };
		if (imgFuns->MImage_getBit(internalMI, pos.data(), channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	int8_t Image<int8_t>::get(mint slice, mint row, mint col, mint channel) const {
		raw_t_bit res;
		std::array<mint, 3> pos { { slice, row, col } };
		if (imgFuns->MImage_getBit(internalMI, pos.data(), channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	void Image<int8_t>::set(mint row, mint col, mint channel, int8_t newValue) {
		std::array<mint, 2> pos { { row, col } };
		if (imgFuns->MImage_setBit(internalMI, pos.data(), channel, newValue)) {
			this->indexError();
		}
	}

	template<>
	void Image<int8_t>::set(mint slice, mint row, mint col, mint channel, int8_t newValue) {
		std::array<mint, 3> pos { { slice, row, col } };
		if (imgFuns->MImage_setBit(internalMI, pos.data(), channel, newValue)) {
			this->indexError();
		}
	}

	//
	//	Template specializations for Byte images
	//

	template<>
	const imagedata_t Image<uint8_t>::type = MImage_Type_Bit8;

	template<>
	uint8_t Image<uint8_t>::get(mint row, mint col, mint channel) const {
		raw_t_ubit8 res;
		std::array<mint, 2> pos { { row, col } };
		if (imgFuns->MImage_getByte(internalMI, pos.data(), channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	uint8_t Image<uint8_t>::get(mint slice, mint row, mint col, mint channel) const {
		raw_t_ubit8 res;
		std::array<mint, 3> pos { { slice, row, col } };
		if (imgFuns->MImage_getByte(internalMI, pos.data(), channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	void Image<uint8_t>::set(mint row, mint col, mint channel, uint8_t newValue) {
		std::array<mint, 2> pos { { row, col } };
		if (imgFuns->MImage_setByte(internalMI, pos.data(), channel, newValue)) {
			this->indexError();
		}
	}

	template<>
	void Image<uint8_t>::set(mint slice, mint row, mint col, mint channel, uint8_t newValue) {
		std::array<mint, 3> pos { { slice, row, col } };
		if (imgFuns->MImage_setByte(internalMI, pos.data(), channel, newValue)) {
			this->indexError();
		}
	}

	//
	//	Template specializations for Bit16 images
	//

	template<>
	const imagedata_t Image<uint16_t>::type = MImage_Type_Bit16;

	template<>
	uint16_t Image<uint16_t>::get(mint row, mint col, mint channel) const {
		raw_t_ubit16 res;
		std::array<mint, 2> pos { { row, col } };
		if (imgFuns->MImage_getBit16(internalMI, pos.data(), channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	uint16_t Image<uint16_t>::get(mint slice, mint row, mint col, mint channel) const {
		raw_t_ubit16 res;
		std::array<mint, 3> pos { { slice, row, col } };
		if (imgFuns->MImage_getBit16(internalMI, pos.data(), channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	void Image<uint16_t>::set(mint row, mint col, mint channel, uint16_t newValue) {
		std::array<mint, 2> pos { { row, col } };
		if (imgFuns->MImage_setBit16(internalMI, pos.data(), channel, newValue)) {
			this->indexError();
		}
	}

	template<>
	void Image<uint16_t>::set(mint slice, mint row, mint col, mint channel, uint16_t newValue) {
		std::array<mint, 3> pos { { slice, row, col } };
		if (imgFuns->MImage_setBit16(internalMI, pos.data(), channel, newValue)) {
			this->indexError();
		}
	}

	//
	//	Template specializations for Real32 images
	//

	template<>
	const imagedata_t Image<float>::type = MImage_Type_Real32;

	template<>
	float Image<float>::get(mint row, mint col, mint channel) const {
		raw_t_real32 res;
		std::array<mint, 2> pos { { row, col } };
		if (imgFuns->MImage_getReal32(internalMI, pos.data(), channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	float Image<float>::get(mint slice, mint row, mint col, mint channel) const {
		raw_t_real32 res;
		std::array<mint, 3> pos { { slice, row, col } };
		if (imgFuns->MImage_getReal32(internalMI, pos.data(), channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	void Image<float>::set(mint row, mint col, mint channel, float newValue) {
		std::array<mint, 2> pos { { row, col } };
		if (imgFuns->MImage_setReal32(internalMI, pos.data(), channel, newValue)) {
			this->indexError();
		}
	}

	template<>
	void Image<float>::set(mint slice, mint row, mint col, mint channel, float newValue) {
		std::array<mint, 3> pos { { slice, row, col } };
		if (imgFuns->MImage_setReal32(internalMI, pos.data(), channel, newValue)) {
			this->indexError();
		}
	}

	//
	//	Template specializations for Real images
	//

	template<>
	const imagedata_t Image<double>::type = MImage_Type_Real;

	template<>
	double Image<double>::get(mint row, mint col, mint channel) const {
		raw_t_real64 res;
		std::array<mint, 2> pos { { row, col } };
		if (imgFuns->MImage_getReal(internalMI, pos.data(), channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	double Image<double>::get(mint slice, mint row, mint col, mint channel) const {
		raw_t_real64 res;
		std::array<mint, 3> pos { { slice, row, col } };
		if (imgFuns->MImage_getReal(internalMI, pos.data(), channel, &res)) {
			this->indexError();
		}
		return res;
	}

	template<>
	void Image<double>::set(mint row, mint col, mint channel, double newValue) {
		std::array<mint, 2> pos { { row, col } };
		if (imgFuns->MImage_setReal(internalMI, pos.data(), channel, newValue)) {
			this->indexError();
		}
	}

	template<>
	void Image<double>::set(mint slice, mint row, mint col, mint channel, double newValue) {
		std::array<mint, 3> pos { { slice, row, col } };
		if (imgFuns->MImage_setReal(internalMI, pos.data(), channel, newValue)) {
			this->indexError();
		}
	}

} /* namespace LibraryLinkUtils */

