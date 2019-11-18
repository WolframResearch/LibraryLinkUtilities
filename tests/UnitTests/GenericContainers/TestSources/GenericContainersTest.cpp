/**
 * @file	PoliciesTest.cpp
 * @brief	Unit tests for passing policies and related functionality
 */

#include <LLU/ErrorLog/Logger.h>
#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>

using namespace LLU::Passing;
using namespace LLU::ErrorCode;

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	return 0;
}

LIBRARY_LINK_FUNCTION(IsOwnerAutomatic) {
	LLU::MArgumentManager mngr {libData, Argc, Args, Res};
	auto img = mngr.getGenericImage(0);
	mngr.set(img.isOwner());
	return NoError;
}

LIBRARY_LINK_FUNCTION(IsOwnerManual) {
	LLU::MArgumentManager mngr {libData, Argc, Args, Res};
	auto t = mngr.getGenericTensor<Manual>(0);
	mngr.set(t.isOwner());
	return NoError;
}

LIBRARY_LINK_FUNCTION(IsOwnerShared) {
	LLU::MArgumentManager mngr {libData, Argc, Args, Res};
	auto na = mngr.getGenericNumericArray<Shared>(0);
	mngr.set(na.isOwner());
	return NoError;
}

LIBRARY_LINK_FUNCTION(CloneAutomatic) {
	LLU::MArgumentManager mngr {libData, Argc, Args, Res};
	auto img = mngr.getGenericImage(0);
	LLU::GenericImage<Manual> clone {img};
	mngr.set(clone);
	return (!img.isOwner() && !clone.isOwner()) ? NoError : MemoryError;
}

LIBRARY_LINK_FUNCTION(CloneManual) {
	LLU::MArgumentManager mngr {libData, Argc, Args, Res};
	auto t = mngr.getGenericTensor<Manual>(0);
	LLU::GenericTensor<Manual> clone {t};
	LLU::Tensor<mint> tensor {std::move(t)};
	tensor[0] = -324;
	mngr.set(clone);
	return (!t.isOwner() && tensor.isOwner() && !clone.isOwner()) ? NoError : MemoryError;
}

LIBRARY_LINK_FUNCTION(CloneShared) {
	LLU::MArgumentManager mngr {libData, Argc, Args, Res};
	auto na = mngr.getGenericNumericArray<Shared>(0);
	LLU::GenericNumericArray<Manual> clone {na};
	mngr.set(clone);
	return (na.isOwner() && !clone.isOwner()) ? NoError : MemoryError;
}

LIBRARY_LINK_FUNCTION(MoveAutomatic) {
	LLU::MArgumentManager mngr {libData, Argc, Args, Res};
	auto img = mngr.getGenericImage(0);
	LLU_DEBUG("Automatic arg is owner: ", img.isOwner());
	LLU::GenericImage<Automatic> clone = std::move(img);	// we can create Automatic container if we move from another Automatic
	LLU_DEBUG("Automatic arg is owner: ", img.isOwner(), ", clone is owner: ", clone.isOwner());
	mngr.set(clone);
	LLU_DEBUG("Automatic arg is owner: ", img.isOwner(), ", clone is owner: ", clone.isOwner());
	return (!img.isOwner() && !clone.isOwner()) ? NoError : MemoryError;
}

LIBRARY_LINK_FUNCTION(MoveManual) {
	LLU::MArgumentManager mngr {libData, Argc, Args, Res};
	auto t = mngr.getGenericTensor<Manual>(0);
	LLU_DEBUG("Manual arg is owner: ", t.isOwner());
	LLU::Tensor<mint> tensor {std::move(t)};
	tensor[0] = -324;
	LLU::GenericTensor<Manual> clone = std::move(tensor);
	LLU_DEBUG("Manual arg is owner: ", t.isOwner(), ", clone is owner: ", clone.isOwner());
	mngr.set(clone);
	LLU_DEBUG("Manual arg is owner: ", t.isOwner(), ", clone is owner: ", clone.isOwner());
	return (!t.isOwner() && !tensor.isOwner() && !clone.isOwner()) ? NoError : MemoryError;
}

LIBRARY_LINK_FUNCTION(MoveShared) {
	LLU::MArgumentManager mngr {libData, Argc, Args, Res};
	auto na = mngr.getGenericNumericArray<Shared>(0);
	LLU_DEBUG("Shared arg is owner: ", na.isOwner());
	LLU::GenericNumericArray<Shared> clone = std::move(na);
	LLU_DEBUG("Shared arg is owner: ", na.isOwner(), ", clone is owner: ", clone.isOwner());
	mngr.set(clone);
	LLU_DEBUG("Shared arg is owner: ", na.isOwner(), ", clone is owner: ", clone.isOwner());
	return (!na.isOwner() && clone.isOwner()) ? NoError : MemoryError;
}