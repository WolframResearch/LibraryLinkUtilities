/**
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief   Definition and implementation of NumericArrayView and TypedNumericArrayView.
 */
#ifndef LLU_CONTAINERS_VIEWS_NUMERICARRAY_HPP
#define LLU_CONTAINERS_VIEWS_NUMERICARRAY_HPP

#include "LLU/Containers/Generic/NumericArray.hpp"
#include "LLU/Containers/Interfaces.h"
#include "LLU/Containers/Iterators/IterableContainer.hpp"

namespace LLU {

	/**
	 * @brief   Simple, light-weight, non-owning wrappper over MNumericArray.
	 *
	 * Intended for use in functions that only need to access MNumericArray metadata, where it can alleviate the need for introducing template parameters
	 * for MNumericArray passing mode (like in GenericNumericArray) or data type (like in NumericArray class).
	 */
	class NumericArrayView : public NumericArrayInterface {
	public:
		NumericArrayView() = default;

		/* implicit */ NumericArrayView(const GenericNumericArray& gNA) : na{gNA.getContainer()} {}

		/* implicit */ NumericArrayView(MNumericArray mna) : na{mna} {}

		/// @copydoc NumericArrayInterface::getRank()
		mint getRank() const override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getRank(na);
		}

		/// @copydoc NumericArrayInterface::getDimensions()
		mint const* getDimensions() const override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getDimensions(na);
		}

		/// @copydoc NumericArrayInterface::getFlattenedLength()
		mint getFlattenedLength() const override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getFlattenedLength(na);
		}

		/// @copydoc NumericArrayInterface::type()
		numericarray_data_t type() const final {
			return LibraryData::NumericArrayAPI()->MNumericArray_getType(na);
		}

		/// @copydoc NumericArrayInterface::rawData()
		void* rawData() const noexcept override {
			return LibraryData::NumericArrayAPI()->MNumericArray_getData(na);
		}

	private:
		MNumericArray na = nullptr;
	};

	template<typename T>
	class NumericArrayTypedView : public NumericArrayView, public IterableContainer<T> {
	public:
		NumericArrayTypedView() = default;

		/* implicit */ NumericArrayTypedView(const GenericNumericArray& gNA) : NumericArrayView(gNA) {
			if (NumericArrayType<T> != type()) {
				ErrorManager::throwException(ErrorName::NumericArrayTypeError);
			}
		}

		/* implicit */ NumericArrayTypedView(NumericArrayView nav) : NumericArrayView(std::move(nav)) {
			if (NumericArrayType<T> != type()) {
				ErrorManager::throwException(ErrorName::NumericArrayTypeError);
			}
		}

		/* implicit */ NumericArrayTypedView(MNumericArray mna) : NumericArrayView(mna) {
			if (NumericArrayType<T> != type()) {
				ErrorManager::throwException(ErrorName::NumericArrayTypeError);
			}
		}

	private:
		T* getData() const noexcept override {
			return static_cast<T*>(rawData());
		}

		mint getSize() const noexcept override {
			return getFlattenedLength();
		}
	};

	template<typename NumericArrayT, typename F>
	auto asTypedNumericArray(NumericArrayT&& na, F&& callable) {
		switch (na.type()) {
			case MNumericArray_Type_Bit8: return std::forward<F>(callable)(NumericArrayTypedView<std::int8_t> {std::forward<NumericArrayT>(na)});
			case MNumericArray_Type_UBit8: return std::forward<F>(callable)(NumericArrayTypedView<std::uint8_t> {std::forward<NumericArrayT>(na)});
			case MNumericArray_Type_Bit16: return std::forward<F>(callable)(NumericArrayTypedView<std::int16_t> {std::forward<NumericArrayT>(na)});
			case MNumericArray_Type_UBit16: return std::forward<F>(callable)(NumericArrayTypedView<std::uint16_t> {std::forward<NumericArrayT>(na)});
			case MNumericArray_Type_Bit32: return std::forward<F>(callable)(NumericArrayTypedView<std::int32_t> {std::forward<NumericArrayT>(na)});
			case MNumericArray_Type_UBit32: return std::forward<F>(callable)(NumericArrayTypedView<std::uint32_t> {std::forward<NumericArrayT>(na)});
			case MNumericArray_Type_Bit64: return std::forward<F>(callable)(NumericArrayTypedView<std::int64_t> {std::forward<NumericArrayT>(na)});
			case MNumericArray_Type_UBit64: return std::forward<F>(callable)(NumericArrayTypedView<std::uint64_t> {std::forward<NumericArrayT>(na)});
			case MNumericArray_Type_Real32: return std::forward<F>(callable)(NumericArrayTypedView<float> {std::forward<NumericArrayT>(na)});
			case MNumericArray_Type_Real64: return std::forward<F>(callable)(NumericArrayTypedView<double> {std::forward<NumericArrayT>(na)});
			case MNumericArray_Type_Complex_Real32:
				return std::forward<F>(callable)(NumericArrayTypedView<std::complex<float>> {std::forward<NumericArrayT>(na)});
			case MNumericArray_Type_Complex_Real64:
				return std::forward<F>(callable)(NumericArrayTypedView<std::complex<double>> {std::forward<NumericArrayT>(na)});
			default: ErrorManager::throwException(ErrorName::NumericArrayTypeError);
		}
	}

	template<typename F>
	auto asTypedNumericArray(MNumericArray na, F&& callable) {
		return asTypedNumericArray(NumericArrayView {na}, std::forward<F>(callable));
	}
}

#endif	  // LLU_CONTAINERS_VIEWS_NUMERICARRAY_HPP
