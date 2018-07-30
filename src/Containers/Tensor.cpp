/** 
 * @file	Tensor.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	18/04/2017
 *
 * @brief	Template specialization of certain methods from Tensor class for all underlying data types that we support
 *
 */

#include "LLU/Containers/Tensor.h"

#include <complex>

namespace LibraryLinkUtils {
	template<>
	const unsigned char Tensor<mint>::type = MType_Integer;
	template<>
	const unsigned char Tensor<double>::type = MType_Real;
	template<>
	const unsigned char Tensor<std::complex<double>>::type = MType_Complex;

	template<>
	mint* Tensor<mint>::getData() const noexcept {
		return this->libData->MTensor_getIntegerData(internalMT);
	}

	template<>
	double* Tensor<double>::getData() const noexcept {
		return this->libData->MTensor_getRealData(internalMT);
	}

	template<>
	std::complex<double>* Tensor<std::complex<double>>::getData() const noexcept {
		return reinterpret_cast<std::complex<double>*>(this->libData->MTensor_getComplexData(internalMT));
	}
} /* namespace LibraryLinkUtils */
