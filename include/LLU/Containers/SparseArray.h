/**
 * @file	SparseArray.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	November 25, 2020
 * @brief	Templated C++ wrapper for MSparseArray
 */
#ifndef LLU_CONTAINERS_SPARSEARRAY_H
#define LLU_CONTAINERS_SPARSEARRAY_H

#include "LLU/Containers/Generic/SparseArray.hpp"
#include "LLU/Containers/Tensor.h"

namespace LLU {

	/**
	 * @brief  Strongly typed wrapper for MSparseArray.
	 * @tparam T - any type supported by MSparseArray (mint, double or std::complex<double>)
	 */
	template<typename T>
	class SparseArray : public GenericSparseArray {
	public:
		using value_type = T;

		/**
		 *  @brief  Default constructor, creates a SparseArray that does not wrap over any raw MSparseArray
		 */
		SparseArray() = default;

		/**
		 *   @brief     Constructs SparseArray based on MSparseArray
		 *   @param[in] t - LibraryLink structure to be wrapped
		 *   @param[in] owner - who manages the memory the raw MSparseArray
		 *   @throws    ErrorName::SparseArrayTypeError - if the SparseArray template type \b T does not match the actual data type of the MSparseArray
		 **/
		SparseArray(MSparseArray t, Ownership owner);

		/**
		 *   @brief     Create new SparseArray from a GenericSparseArray
		 *   @param[in] t - generic SparseArray to be wrapped into SparseArray class
		 *   @throws	ErrorName::SparseArrayTypeError - if the SparseArray template type \b T does not match the actual data type of the generic SparseArray
		 **/
		explicit SparseArray(GenericSparseArray t);

		/**
		 * @brief   Create a new SparseArray from positions, values, dimensions and an implicit value
		 * @param   positions - positions of all the explicit values in the array
		 * @param   values - explicit values to be stored in the array
		 * @param   dimensions - dimensions of the new SparseArray
		 * @param   implicitValue - implicit value (the one that is not stored) of the new SparseArray
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_fromExplicitPositions.html>
		 */
		SparseArray(const Tensor<mint>& positions, const Tensor<T>& values, const Tensor<mint>& dimensions, T implicitValue);

		/**
		 * @brief   Create a new SparseArray from data array and an implicit value
		 * @param   data - a tensor whose contents will be copied and sparsified
		 * @param   implicitValue - implicit value (the one that is not stored) of the new SparseArray
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_fromMTensor.html>
		 */
		SparseArray(const Tensor<T>& data, T implicitValue);

		/**
		 * @brief   Create a copy of given SparseArray with different implicit value
		 * @param   s - other SparseArray of type T
		 * @param   implicitValue - implicit value (the one that is not stored) of the new SparseArray
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_resetImplicitValue.html>
		 */
		SparseArray(const SparseArray& s, T implicitValue);

		/**
		 * @brief   Get the rank (number of dimensions) of this sparse array.
		 * @return  the rank of this array
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getRank.html>
		 */
		mint rank() const {
			return getRank();
		}

		/**
		 * @brief   Get the implicit value of this sparse array.
		 * @return  implicit value (the one that is not stored)
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getImplicitValue.html>
		 */
		T implicitValue() const;

		/**
		 * @brief   Change the implicit value of this array.
		 * @param   newImplicitValue - new implicit value
		 * @note    The underlying MSparseArray object may be replaced in the process.
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_resetImplicitValue.html>
		 */
		void setImplicitValue(T newImplicitValue);

		/**
		 * @brief   Get a tensor with the values corresponding to the explicitly stored positions in the sparse array.
		 * @return  Tensor of rank 1 with length equal to the number of explicit positions in the array
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getExplicitValues.html>
		 */
		Tensor<T> explicitValues() const;

		/**
		 * @brief   Get a row pointer array for this sparse array.
		 * @details The values returned are the cumulative number of explicitly represented elements for each row, so the values will be non-decreasing.
		 * @return  Tensor of integers of rank 1
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getRowPointers.html>
		 */
		Tensor<mint> rowPointers() const;

		/**
		 * @brief   Get the column indices for the explicitly stored positions in this sparse array.
		 * @details The first dimension of the resulting tensor is the number of explicit positions, and the second dimension is equal to rank() - 1.
		 * @return  Tensor of rank 2
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getColumnIndices.html>
		 */
		Tensor<mint> columnIndices() const;

		/**
		 * @brief   Get the explicitly specified positions in this sparse array.
		 * @details The first dimension of the resulting tensor is the number of explicit positions, and the second dimension is equal to rank().
		 * @return  Tensor of rank 2
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getExplicitPositions.html>
		 */
		[[nodiscard]] Tensor<mint> explicitPositions() const;

		/**
		 * @brief   Expand this sparse array to a regular tensor
		 * @return  Tensor of the same data type as this array
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_toMTensor.html>
		 */
		[[nodiscard]] Tensor<T> toTensor() const;
	};

	/**
	 * Take a SparseArray-like object \p t and a function \p callable and call the function with a SparseArrayTypedView created from \p t
	 * @tparam  SparseArrayT - a SparseArray-like type (GenericSparseArray, SparseArrayView or MNumericArray)
	 * @tparam  F - any callable object
	 * @param   t - SparseArray-like object on which an operation will be performed
	 * @param   callable - a callable object that can be called with a SparseArrayTypedView of any type
	 * @return  result of calling \p callable on a SparseArrayTypedView over \p t
	 */
	template<typename F>
	auto asTypedSparseArray(const GenericSparseArray& sa, F&& callable) {
		switch (sa.type()) {
			case MType_Integer: return std::forward<F>(callable)(SparseArray<mint> {sa.getContainer(), Ownership::LibraryLink});
			case MType_Real: return std::forward<F>(callable)(SparseArray<double> {sa.getContainer(), Ownership::LibraryLink});
			case MType_Complex: return std::forward<F>(callable)(SparseArray<std::complex<double>> {sa.getContainer(), Ownership::LibraryLink});
			default: ErrorManager::throwException(ErrorName::SparseArrayTypeError);
		}
	}

	/// @cond
	// Overload of asTypedSparseArray for MSparseArray
	template<typename F>
	auto asTypedSparseArray(MSparseArray t, F&& callable) {
		return asTypedSparseArray(GenericSparseArray {t, Ownership::LibraryLink}, std::forward<F>(callable));
	}
	/// @endcond

	template<typename T>
	SparseArray<T>::SparseArray(GenericSparseArray t) : GenericSparseArray(std::move(t)) {
		if (TensorType<T> != GenericSparseArray::type()) {
			ErrorManager::throwException(ErrorName::SparseArrayTypeError);
		}
	}

	template<typename T>
	SparseArray<T>::SparseArray(MSparseArray t, Ownership owner) : SparseArray(GenericSparseArray {t, owner}) {}

	template<typename T>
	SparseArray<T>::SparseArray(const Tensor<mint>& positions, const Tensor<T>& values, const Tensor<mint>& dimensions, T implicitValue) :
		GenericSparseArray(positions, values, dimensions, Tensor<T>(implicitValue, {})){}

	template<typename T>
	SparseArray<T>::SparseArray(const Tensor<T>& data, T implicitValue) : GenericSparseArray(data, Tensor<T>(implicitValue, {})) {}


	template<typename T>
	SparseArray<T>::SparseArray(const SparseArray& s, T implicitValue) : GenericSparseArray(s, Tensor<T>(implicitValue, {})) {}

	template<typename T>
	T SparseArray<T>::implicitValue() const {
		return Tensor<T> {GenericSparseArray::getImplicitValueAsTensor()}.front();
	}

	template<typename T>
	void SparseArray<T>::setImplicitValue(T newImplicitValue) {
		GenericSparseArray::setImplicitValueFromTensor(Tensor<T>(newImplicitValue, {}));
	}

	template<typename T>
	Tensor<T> SparseArray<T>::explicitValues() const {
		return Tensor<T> {GenericSparseArray::getExplicitValues()};
	}

	template<typename T>
	Tensor<mint> SparseArray<T>::rowPointers() const {
		return Tensor<mint> {MContainer::getRowPointers()};
	}
	template<typename T>
	Tensor<mint> SparseArray<T>::columnIndices() const {
		return Tensor<mint> {MContainer::getColumnIndices()};
	}
	template<typename T>
	Tensor<mint> SparseArray<T>::explicitPositions() const {
		return Tensor<mint> {MContainer::getExplicitPositions()};
	}
	template<typename T>
	Tensor<T> SparseArray<T>::toTensor() const {
		return Tensor<T> {MContainer::toGenericTensor()};
	}

	
} // namespace LLU

#endif	  // LLU_CONTAINERS_SPARSEARRAY_H
