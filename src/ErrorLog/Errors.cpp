/**
 * @file	Errors.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	March 31, 2019
 */

#include "LLU/ErrorLog/Errors.h"

namespace LibraryLinkUtils {

	#define LLU_DEFINE_ERROR_NAME(errorIdentifier) const std::string errorIdentifier = #errorIdentifier

	namespace LLErrorName {
		LLU_DEFINE_ERROR_NAME(VersionError);
		LLU_DEFINE_ERROR_NAME(FunctionError);
		LLU_DEFINE_ERROR_NAME(MemoryError);
		LLU_DEFINE_ERROR_NAME(NumericalError);
		LLU_DEFINE_ERROR_NAME(DimensionsError);
		LLU_DEFINE_ERROR_NAME(RankError);
		LLU_DEFINE_ERROR_NAME(TypeError);
		LLU_DEFINE_ERROR_NAME(NoError);

		LLU_DEFINE_ERROR_NAME(LibDataError);

		LLU_DEFINE_ERROR_NAME(MArgumentIndexError);
		LLU_DEFINE_ERROR_NAME(MArgumentNumericArrayError);
		LLU_DEFINE_ERROR_NAME(MArgumentTensorError);
		LLU_DEFINE_ERROR_NAME(MArgumentImageError);

		LLU_DEFINE_ERROR_NAME(ErrorManagerThrowIdError);
		LLU_DEFINE_ERROR_NAME(ErrorManagerThrowNameError);
		LLU_DEFINE_ERROR_NAME(ErrorManagerCreateNameError);

		LLU_DEFINE_ERROR_NAME(NumericArrayNewError);
		LLU_DEFINE_ERROR_NAME(NumericArrayCloneError);
		LLU_DEFINE_ERROR_NAME(NumericArrayTypeError);
		LLU_DEFINE_ERROR_NAME(NumericArraySizeError);
		LLU_DEFINE_ERROR_NAME(NumericArrayIndexError);
		LLU_DEFINE_ERROR_NAME(NumericArrayConversionError);

		LLU_DEFINE_ERROR_NAME(TensorNewError);
		LLU_DEFINE_ERROR_NAME(TensorCloneError);
		LLU_DEFINE_ERROR_NAME(TensorTypeError);
		LLU_DEFINE_ERROR_NAME(TensorSizeError);
		LLU_DEFINE_ERROR_NAME(TensorIndexError);

		LLU_DEFINE_ERROR_NAME(ImageNewError);
		LLU_DEFINE_ERROR_NAME(ImageCloneError);
		LLU_DEFINE_ERROR_NAME(ImageTypeError);
		LLU_DEFINE_ERROR_NAME(ImageSizeError);
		LLU_DEFINE_ERROR_NAME(ImageIndexError);

		LLU_DEFINE_ERROR_NAME(MLTestHeadError);
		LLU_DEFINE_ERROR_NAME(MLPutSymbolError);
		LLU_DEFINE_ERROR_NAME(MLPutFunctionError);
		LLU_DEFINE_ERROR_NAME(MLTestSymbolError);
		LLU_DEFINE_ERROR_NAME(MLWrongSymbolForBool);
		LLU_DEFINE_ERROR_NAME(MLGetListError);
		LLU_DEFINE_ERROR_NAME(MLGetScalarError);
		LLU_DEFINE_ERROR_NAME(MLGetStringError);
		LLU_DEFINE_ERROR_NAME(MLGetArrayError);
		LLU_DEFINE_ERROR_NAME(MLPutListError);
		LLU_DEFINE_ERROR_NAME(MLPutScalarError);
		LLU_DEFINE_ERROR_NAME(MLPutStringError);
		LLU_DEFINE_ERROR_NAME(MLPutArrayError);
		LLU_DEFINE_ERROR_NAME(MLGetSymbolError);
		LLU_DEFINE_ERROR_NAME(MLGetFunctionError);
		LLU_DEFINE_ERROR_NAME(MLPacketHandleError);
		LLU_DEFINE_ERROR_NAME(MLFlowControlError);
		LLU_DEFINE_ERROR_NAME(MLTransferToLoopbackError);
		LLU_DEFINE_ERROR_NAME(MLCreateLoopbackError);
		LLU_DEFINE_ERROR_NAME(MLLoopbackStackSizeError);

		LLU_DEFINE_ERROR_NAME(DLNullRawNode);
		LLU_DEFINE_ERROR_NAME(DLInvalidNodeType);
		LLU_DEFINE_ERROR_NAME(DLGetNodeDataError);
		LLU_DEFINE_ERROR_NAME(DLNullRawDataStore);
		LLU_DEFINE_ERROR_NAME(DLPushBackTypeError);

		LLU_DEFINE_ERROR_NAME(ArgumentCreateNull);
		LLU_DEFINE_ERROR_NAME(ArgumentAddNodeMArgument);

		LLU_DEFINE_ERROR_NAME(Aborted);
	}

} /* namespace LibraryLinkUtils */