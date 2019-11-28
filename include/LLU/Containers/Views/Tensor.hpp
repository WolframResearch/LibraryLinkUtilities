/**
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief
 */
#ifndef LLU_CONTAINERS_VIEWS_TENSOR_HPP
#define LLU_CONTAINERS_VIEWS_TENSOR_HPP

#include "LLU/Containers/Generic/Tensor.hpp"
#include "LLU/Containers/Interfaces.h"
#include "LLU/Containers/Iterators/IterableContainer.hpp"

namespace LLU {

	/**
	 * @brief   Simple, light-weight, non-owning wrappper over MTensor.
	 *
	 * Intended for use in functions that only need to access MTensor metadata, where it can alleviate the need for introducing template parameters
	 * for MTensor passing mode (like in GenericTensor) or data type (like in Tensor class).
	 */
	class TensorView : public TensorInterface {
	public:
		TensorView() = default;

		template<class Passing>
		/* implicit */ TensorView(const GenericTensor<Passing>& gTen) : t {gTen.getContainer()} {}

		/* implicit */ TensorView(MTensor mt) : t {mt} {}

		/// @copydoc TensorInterface::getRank()
		mint getRank() const override {
			return LibraryData::API()->MTensor_getRank(t);
		}

		/// @copydoc TensorInterface::getDimensions()
		mint const* getDimensions() const override {
			return LibraryData::API()->MTensor_getDimensions(t);
		}

		/// @copydoc TensorInterface::getFlattenedLength()
		mint getFlattenedLength() const override {
			return LibraryData::API()->MTensor_getFlattenedLength(t);
		}

		/// @copydoc TensorInterface::type()
		mint type() const override {
			return LibraryData::API()->MTensor_getType(t);
		}

		/// @copybrief TensorInterface::rawData()
		void* rawData() const override {
			switch (type()) {
				case MType_Integer: return LibraryData::API()->MTensor_getIntegerData(t);
				case MType_Real: return LibraryData::API()->MTensor_getRealData(t);
				case MType_Complex: return LibraryData::API()->MTensor_getComplexData(t);
				default: ErrorManager::throwException(ErrorName::TensorTypeError);
			}
		}

	private:
		MTensor t = nullptr;
	};

	template<typename T>
	class TensorTypedView : public TensorView, public IterableContainer<T> {
	public:
		TensorTypedView() = default;

		template<class Passing>
		/* implicit */ TensorTypedView(const GenericTensor<Passing>& gTen) : TensorView(gTen) {
			if (TensorType<T> != type()) {
				ErrorManager::throwException(ErrorName::TensorTypeError);
			}
		}

		/* implicit */ TensorTypedView(TensorView tv) : TensorView(std::move(tv)) {
			if (TensorType<T> != type()) {
				ErrorManager::throwException(ErrorName::TensorTypeError);
			}
		}

		/* implicit */ TensorTypedView(MTensor mt) : TensorView(mt) {
			if (TensorType<T> != type()) {
				ErrorManager::throwException(ErrorName::TensorTypeError);
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

	template<typename TensorT, typename F>
	auto asTypedTensor(TensorT&& t, F&& callable) {
		switch (t.type()) {
			case MType_Integer: return std::forward<F>(callable)(TensorTypedView<mint> {std::forward<TensorT>(t)});
			case MType_Real: return std::forward<F>(callable)(TensorTypedView<double> {std::forward<TensorT>(t)});
			case MType_Complex: return std::forward<F>(callable)(TensorTypedView<std::complex<double>> {std::forward<TensorT>(t)});
			default: ErrorManager::throwException(ErrorName::TensorTypeError);
		}
	}

	template<typename F>
	auto asTypedTensor(MTensor t, F&& callable) {
		return asTypedTensor(TensorView {t}, std::forward<F>(callable));
	}

}

#endif	  // LLU_CONTAINERS_VIEWS_TENSOR_HPP
