/**
 * @file	SparseArrayOperations.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	November 25, 2020
 * @brief
 */

#include <LLU/Containers/Views/Tensor.hpp>
#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	return 0;
}

LLU_LIBRARY_FUNCTION(GetImplicitValue) {
	const auto sp = mngr.getGenericSparseArray<LLU::Passing::Constant>(0);
	auto implVal = sp.getImplicitValueAsTensor();
	mngr.set(implVal.clone());
}

LLU_LIBRARY_FUNCTION(GetExplicitValues) {
	const auto sp = mngr.getGenericSparseArray<LLU::Passing::Constant>(0);
	auto explValues = sp.getExplicitValues();
	mngr.set(explValues.clone());
}

LLU_LIBRARY_FUNCTION(GetRowPointers) {
	const auto sp = mngr.getGenericSparseArray<LLU::Passing::Constant>(0);
	auto rowPointers = sp.getRowPointers();
	mngr.set(rowPointers.clone());
}

LLU_LIBRARY_FUNCTION(GetColumnIndices) {
	const auto sp = mngr.getGenericSparseArray<LLU::Passing::Constant>(0);
	auto colIndices = sp.getColumnIndices();
	mngr.set(colIndices.clone());
}

LLU_LIBRARY_FUNCTION(GetExplicitPositions) {
	const auto sp = mngr.getGenericSparseArray<LLU::Passing::Constant>(0);
	auto explPositions = sp.getExplicitPositions();
	mngr.set(explPositions);
}

LLU_LIBRARY_FUNCTION(ToTensor) {
	const auto sp = mngr.getGenericSparseArray<LLU::Passing::Constant>(0);
	auto tensor = sp.toGenericTensor();
	mngr.set(tensor);
}

LLU_LIBRARY_FUNCTION(SetImplicitValue) {
	auto sp = mngr.getGenericSparseArray(0);
	auto implValue = mngr.getGenericTensor(1);
	sp.setImplicitValueFromTensor(implValue);
	mngr.set(sp);
}

template<typename T>
void sparseModifyValues(LLU::SparseArray<T>& sa, LLU::TensorTypedView<T> newValues) {
	auto values = sa.explicitValues();
	if (values.size() < newValues.size()) {
		throw std::runtime_error {"Too many values provided."};
	}
	std::copy(std::cbegin(newValues), std::cend(newValues), std::begin(values));

	/* Recompute explicit positions */
	sa.resparsify();
}

LLU_LIBRARY_FUNCTION(ModifyValues) {
	auto sp = mngr.getGenericSparseArray<LLU::Passing::Shared>(0);
	auto values = mngr.getGenericTensor<LLU::Passing::Constant>(1);
	auto dataType = sp.type();
	if (dataType != values.type()) {
		throw std::runtime_error {"Inconsistent types."};
	}
	LLU::asTypedSparseArray(sp, [&values](auto&& sparseArray) {
		using T = typename std::remove_reference_t<decltype(sparseArray)>::value_type;
		sparseModifyValues(sparseArray, LLU::TensorTypedView<T> {values});
	});
}

LLU_LIBRARY_FUNCTION(ModifyValues2) {
	auto values = mngr.getGenericTensor<LLU::Passing::Constant>(1);
	mngr.operateOnSparseArray<LLU::Passing::Shared>(0, [](auto&& sparseArray, const LLU::GenericTensor& values) {
		using T = typename std::remove_reference_t<decltype(sparseArray)>::value_type;
		sparseModifyValues(sparseArray, LLU::TensorTypedView<T> {values});
	}, values);
}

LLU_LIBRARY_FUNCTION(GetImplicitValueTyped) {
	const auto sp = mngr.getSparseArray<double, LLU::Passing::Constant>(0);
	auto implVal = sp.implicitValue();
	mngr.set(implVal);
}

LLU_LIBRARY_FUNCTION(GetExplicitValuesTyped) {
	const auto sp = mngr.getSparseArray<double, LLU::Passing::Constant>(0);
	auto explValues = sp.explicitValues();
	mngr.set(explValues.clone());
}

LLU_LIBRARY_FUNCTION(GetRowPointersTyped) {
	const auto sp = mngr.getSparseArray<double, LLU::Passing::Constant>(0);
	auto rowPointers = sp.rowPointers();
	mngr.set(rowPointers.clone());
}

LLU_LIBRARY_FUNCTION(GetColumnIndicesTyped) {
	const auto sp = mngr.getSparseArray<double, LLU::Passing::Constant>(0);
	auto colIndices = sp.columnIndices();
	mngr.set(colIndices.clone());
}

LLU_LIBRARY_FUNCTION(GetExplicitPositionsTyped) {
	const auto sp = mngr.getSparseArray<double, LLU::Passing::Constant>(0);
	auto explPositions = sp.explicitPositions();
	mngr.set(explPositions);
}

LLU_LIBRARY_FUNCTION(ToTensorTyped) {
	const auto sp = mngr.getSparseArray<double, LLU::Passing::Constant>(0);
	auto tensor = sp.toTensor();
	mngr.set(tensor);
}

LLU_LIBRARY_FUNCTION(SetImplicitValueTyped) {
	auto sp = mngr.getSparseArray<double>(0);
	auto implValue = mngr.getReal(1);
	sp.setImplicitValue(implValue);
	mngr.set(sp);
}