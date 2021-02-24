/**
 * @file	SparseArray.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	November 25, 2020
 * @brief   Implementation of GenericSparseArray and SparseArray<T>
 */

#include "LLU/Containers/SparseArray.h"

namespace LLU {

	GenericSparseArray::MContainer(const GenericTensor& positions, const GenericTensor& values, const GenericTensor& dimensions,
								   const GenericTensor& implicitValue) {
		Container tmp {};
		if (0 != LibraryData::SparseArrayAPI()->MSparseArray_fromExplicitPositions(positions.getContainer(), values.getContainer(), dimensions.getContainer(),
																				   implicitValue.getContainer(), &tmp)) {
			ErrorManager::throwException(ErrorName::SparseArrayFromPositionsError);
		}
		this->reset(tmp);
	}

	GenericSparseArray::MContainer(const GenericTensor& data, const GenericTensor& implicitValue) {
		Container tmp {};
		if (0 != LibraryData::SparseArrayAPI()->MSparseArray_fromMTensor(data.getContainer(), implicitValue.getContainer(), &tmp)) {
			ErrorManager::throwException(ErrorName::SparseArrayFromTensorError);
		}
		this->reset(tmp);
	}

	GenericSparseArray::MContainer(const GenericSparseArray& s, const GenericTensor& implicitValue) {
		Container tmp {};
		if (0 != LibraryData::SparseArrayAPI()->MSparseArray_resetImplicitValue(s.getContainer(), implicitValue.getContainer(), &tmp)) {
			ErrorManager::throwException(ErrorName::SparseArrayImplicitValueResetError);
		}
		this->reset(tmp);
	}

	auto GenericSparseArray::cloneImpl() const -> Container {
		Container tmp {};
		if (0 != LibraryData::SparseArrayAPI()->MSparseArray_clone(this->getContainer(), &tmp)) {
			ErrorManager::throwException(ErrorName::SparseArrayCloneError);
		}
		return tmp;
	}

	GenericTensor GenericSparseArray::getImplicitValueAsTensor() const {
		if (auto* implValue = LibraryData::SparseArrayAPI()->MSparseArray_getImplicitValue(this->getContainer()); implValue) {
			return {*implValue, Ownership::LibraryLink};
		}
		ErrorManager::throwException(ErrorName::SparseArrayImplicitValueError);
	}

	void MContainer<MArgumentType::SparseArray>::setImplicitValueFromTensor(const GenericTensor& implicitValue) {
		Container rawSparse = getContainer();
		if (0 != LibraryData::SparseArrayAPI()->MSparseArray_resetImplicitValue(rawSparse, implicitValue.getContainer(), &rawSparse)) {
			ErrorManager::throwException(ErrorName::SparseArrayImplicitValueResetError);
		}
		this->reset(rawSparse);
	}

	GenericTensor MContainer<MArgumentType::SparseArray>::getExplicitValues() const {
		if (auto* explicitValues = LibraryData::SparseArrayAPI()->MSparseArray_getExplicitValues(this->getContainer()); explicitValues) {
			return {*explicitValues, Ownership::LibraryLink};
		}
		ErrorManager::throwException(ErrorName::SparseArrayExplicitValuesError);
	}

	GenericTensor MContainer<MArgumentType::SparseArray>::getRowPointers() const {
		if (auto* rowPointers = LibraryData::SparseArrayAPI()->MSparseArray_getRowPointers(this->getContainer()); rowPointers) {
			return {*rowPointers, Ownership::LibraryLink};
		}
		ErrorManager::throwException(ErrorName::SparseArrayRowPointersError);
	}

	GenericTensor MContainer<MArgumentType::SparseArray>::getColumnIndices() const {
		if (auto* colIndices = LibraryData::SparseArrayAPI()->MSparseArray_getColumnIndices(this->getContainer()); colIndices) {
			return {*colIndices, Ownership::LibraryLink};
		}
		ErrorManager::throwException(ErrorName::SparseArrayColumnIndicesError);
	}

	GenericTensor MContainer<MArgumentType::SparseArray>::getExplicitPositions() const {
		MTensor mt {};
		if (0 != LibraryData::SparseArrayAPI()->MSparseArray_getExplicitPositions(getContainer(), &mt)) {
			ErrorManager::throwException(ErrorName::SparseArrayExplicitPositionsError);
		}
		return {mt, Ownership::Library};
	}

	GenericTensor MContainer<MArgumentType::SparseArray>::toGenericTensor() const {
		MTensor mt {};
		if (0 != LibraryData::SparseArrayAPI()->MSparseArray_toMTensor(getContainer(), &mt)) {
			ErrorManager::throwException(ErrorName::SparseArrayToTensorError);
		}
		return {mt, Ownership::Library};
	}

	void GenericSparseArray::resparsify() {
		setImplicitValueFromTensor(GenericTensor{});
	}

	mint GenericSparseArray::type() const {
		return getImplicitValueAsTensor().type();
	}

}	 // namespace LLU