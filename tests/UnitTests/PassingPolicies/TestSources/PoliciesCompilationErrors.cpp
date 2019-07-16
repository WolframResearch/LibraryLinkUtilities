/**
 * @file	PoliciesCompilationErrors.cpp
 * @brief	
 */

#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>

using namespace LLU::Passing;

LIBRARY_LINK_FUNCTION(CreateAutomatic) {
	LLU::NumericArray<uint8_t, Automatic> na { 1, 2, 3, 4, 5 };
	return 0;
}

LIBRARY_LINK_FUNCTION(CreateShared) {
	LLU::NumericArray<uint8_t, Shared> na{1, 2, 3, 4, 5};
	return 0;
}

LIBRARY_LINK_FUNCTION(SharedGenericAsManual) {
	LLU::MArgumentManager mngr{libData, Argc, Args, Res};
	LLU::Tensor<mint, Manual> t = mngr.getGenericTensor<Shared>(0);
	return 0;
}

LIBRARY_LINK_FUNCTION(SharedAsAutomatic) {
	LLU::MArgumentManager mngr{libData, Argc, Args, Res};
	LLU::GenericTensor<Automatic> t = mngr.getGenericTensor<Shared>(0);
	return 0;
}

LIBRARY_LINK_FUNCTION(AutomaticAsShared) {
	LLU::MArgumentManager mngr{libData, Argc, Args, Res};
	LLU::GenericTensor<Shared> t = mngr.getGenericTensor<>(0);
	return 0;
}

LIBRARY_LINK_FUNCTION(DisownManual) {
	LLU::NumericArray<uint8_t> na{1, 2, 3, 4, 5};
	na.disown();
	return 0;
}