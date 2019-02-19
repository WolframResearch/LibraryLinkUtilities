/** 
 * @file	Tensor.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	18/04/2017
 *
 * @brief	Template specialization of certain methods from TypedTensor class for all underlying data types that we support
 *
 */

#include "LLU/Containers/Tensor.h"

#include <complex>

namespace LibraryLinkUtils {
	template<>
	const mint TypedTensor<mint>::type = MType_Integer;
	template<>
	const mint TypedTensor<double>::type = MType_Real;
	template<>
	const mint TypedTensor<std::complex<double>>::type = MType_Complex;

	template<>
	mint* TypedTensor<mint>::getData() const noexcept {
		return this->libData->MTensor_getIntegerData(this->getInternal());
	}

	template<>
	double* TypedTensor<double>::getData() const noexcept {
		return this->libData->MTensor_getRealData(this->getInternal());
	}

	template<>
	std::complex<double>* TypedTensor<std::complex<double>>::getData() const noexcept {
		return reinterpret_cast<std::complex<double>*>(this->libData->MTensor_getComplexData(this->getInternal()));
	}
} /* namespace LibraryLinkUtils */
