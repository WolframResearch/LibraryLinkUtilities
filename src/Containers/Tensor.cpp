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

namespace LLU {
	/// @cond
	template<>
	mint* TypedTensor<mint>::getData() const noexcept {
		return LibraryData::API()->MTensor_getIntegerData(this->getInternal());
	}

	template<>
	double* TypedTensor<double>::getData() const noexcept {
		return LibraryData::API()->MTensor_getRealData(this->getInternal());
	}

	template<>
	std::complex<double>* TypedTensor<std::complex<double>>::getData() const noexcept {
		return reinterpret_cast<std::complex<double>*>(LibraryData::API()->MTensor_getComplexData(this->getInternal()));
	}
	/// @endcond
} /* namespace LLU */
