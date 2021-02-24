/**
 * @file    SparseArray.hpp
 * @brief   GenericSparseArray definition and implementation
 */
#ifndef LLU_CONTAINERS_GENERIC_SPARSEARRAY_HPP
#define LLU_CONTAINERS_GENERIC_SPARSEARRAY_HPP

#include "LLU/Containers/Generic/Base.hpp"
#include "LLU/Containers/Generic/Tensor.hpp"

namespace LLU {

	template<>
	class MContainer<MArgumentType::SparseArray>;

	/// MContainer specialization for MSparseArray is called GenericSparseArray
	using GenericSparseArray = MContainer<MArgumentType::SparseArray>;

	/**
	 *  @brief  MContainer specialization for MSparseArray
	 */
	template<>
	class MContainer<MArgumentType::SparseArray> : public MContainerBase<MArgumentType::SparseArray> {
	public:
		/// Inherit constructors from MContainerBase
		using MContainerBase<MArgumentType::SparseArray>::MContainerBase;

		/**
		 * @brief   Default constructor, the MContainer does not manage any instance of MSparseArray.
		 */
		MContainer() = default;

		/**
		 * @brief   Create a new SparseArray from positions, values, dimensions and an implicit value
		 * @param   positions - positions of all the explicit values in the array
		 * @param   values - explicit values to be stored in the array
		 * @param   dimensions - dimensions of the new SparseArray
		 * @param   implicitValue - implicit value (the one that is not stored) of the new SparseArray
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_fromExplicitPositions.html>
		 */
		MContainer(const GenericTensor& positions, const GenericTensor& values, const GenericTensor& dimensions, const GenericTensor& implicitValue);

		/**
		 * @brief   Create a new SparseArray from data array and an implicit value
		 * @param   data - a tensor whose contents will be copied and sparsified
		 * @param   implicitValue - implicit value (the one that is not stored) of the new SparseArray
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_fromMTensor.html>
		 */
		MContainer(const GenericTensor& data, const GenericTensor& implicitValue);

		/**
		 * @brief   Create a copy of given SparseArray with different implicit value
		 * @param   s - other SparseArray
		 * @param   implicitValue - implicit value (the one that is not stored) of the new SparseArray
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_resetImplicitValue.html>
		 */
		MContainer(const GenericSparseArray& s, const GenericTensor& implicitValue);

		/**
		 * @brief   Clone this MContainer, performs a deep copy of the underlying MSparseArray.
		 * @note    The cloned MContainer always belongs to the library (Ownership::Library) because LibraryLink has no idea of its existence.
		 * @return  new MContainer, by value
		 */
		MContainer clone() const {
			return MContainer {cloneContainer(), Ownership::Library};
		}

		/**
		 * @brief   Get the implicit value of this sparse array.
		 * @return  Rank 0 tensor of the same type as the value type of this sparse array.
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getImplicitValue.html>
		 */
		GenericTensor getImplicitValueAsTensor() const;

		/**
		 * @brief   Change the implicit value of this array.
		 * @param   implicitValue - new implicit value
		 * @note    The underlying MSparseArray object may be replaced in the process.
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_resetImplicitValue.html>
		 */
		void setImplicitValueFromTensor(const GenericTensor& implicitValue);

		/**
		 * @brief   Get the rank (number of dimensions) of this sparse array.
		 * @return  the rank of this array
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getRank.html>
		 */
		mint getRank() const {
			return LibraryData::SparseArrayAPI()->MSparseArray_getRank(this->getContainer());
		}

		/**
		 * @brief   Get dimensions of this sparse array.
		 * @return  a read-only raw array of container dimensions
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getDimensions.html>
		 */
		mint const* getDimensions() const {
			return LibraryData::SparseArrayAPI()->MSparseArray_getDimensions(this->getContainer());
		}

		/**
		 * @brief   Get a tensor with the values corresponding to the explicitly stored positions in the sparse array.
		 * @return  GenericTensor of rank 1 with length equal to the number of explicit positions in the array or an empty wrapper for "pattern sparse arrays"
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getExplicitValues.html>
		 */
		GenericTensor getExplicitValues() const;

		/**
		 * @brief   Get a row pointer array for this sparse array.
		 * @details The values returned are the cumulative number of explicitly represented elements for each row, so the values will be non-decreasing.
		 * @return  GenericTensor of rank 1 and integer type or an empty GenericTensor
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getRowPointers.html>
		 */
		GenericTensor getRowPointers() const;

		/**
		 * @brief   Get the column indices for the explicitly stored positions in this sparse array.
		 * @details The first dimension of the resulting tensor is the number of explicit positions, and the second dimension is equal to getRank() - 1.
		 * @return  GenericTensor of rank 2 or an empty GenericTensor
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getColumnIndices.html>
		 */
		GenericTensor getColumnIndices() const;

		/**
		 * @brief   Get the explicitly specified positions in this sparse array.
		 * @details The first dimension of the resulting tensor is the number of explicit positions, and the second dimension is equal to getRank().
		 * @return  GenericTensor of rank 2 or an empty GenericTensor
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_getExplicitPositions.html>
		 */
		[[nodiscard]] GenericTensor getExplicitPositions() const;

		/**
		 * @brief   Expand this sparse array to a regular tensor
		 * @return  GenericTensor of the same data type as this array
		 * @see     <https://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_toMTensor.html>
		 */
		[[nodiscard]] GenericTensor toGenericTensor() const;

		/**
		 * @brief   Use current implicit value to recalculate the sparse array after the data has been modified.
		 */
		void resparsify();

		/**
		 * @brief   Get the data type of this MSparseArray
		 * @return  type of elements (MType_Integer, MType_Real or MType_Complex)
		 */
		mint type() const;

	private:

		/**
		 *   @brief Make a deep copy of the raw container
		 *   @see   <http://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_clone.html>
		 **/
		Container cloneImpl() const override;

		/**
		 * @copydoc MContainer<MArgumentType::Image>::shareCount()
		 * @see 	<http://reference.wolfram.com/language/LibraryLink/ref/callback/MSparseArray_shareCount.html>
		 */
		mint shareCountImpl() const noexcept override {
			return LibraryData::SparseArrayAPI()->MSparseArray_shareCount(this->getContainer());
		}

		///@copydoc   MContainer<MArgumentType::DataStore>::pass
		void passImpl(MArgument& res) const noexcept override {
			MArgument_setMSparseArray(res, this->getContainer());
		}
	};

}  // namespace LLU

#endif	  // LLU_CONTAINERS_GENERIC_SPARSEARRAY_HPP
