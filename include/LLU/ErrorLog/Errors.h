/**
 * @file	Errors.h
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	March 31, 2019
 * @brief	Definitions of error names and error codes used across LLU.
 */
#ifndef LLUTILS_ERRORS_H
#define LLUTILS_ERRORS_H

#include <string>

namespace LLU {

	/**
	 * @brief Error codes predefined in Library Link
	 */
	namespace ErrorCode {

		// Original LibraryLink error codes:
		constexpr int VersionError = 7;		  ///< same as LIBRARY_VERSION_ERROR
		constexpr int FunctionError = 6;	  ///< same as LIBRARY_FUNCTION_ERROR
		constexpr int MemoryError = 5;		  ///< same as LIBRARY_MEMORY_ERROR
		constexpr int NumericalError = 4;	  ///< same as LIBRARY_NUMERICAL_ERROR
		constexpr int DimensionsError = 3;	  ///< same as LIBRARY_DIMENSIONS_ERROR
		constexpr int RankError = 2;		  ///< same as LIBRARY_RANK_ERROR
		constexpr int TypeError = 1;		  ///< same as LIBRARY_TYPE_ERROR
		constexpr int NoError = 0;			  ///< same as LIBRARY_NO_ERROR
	}

	/**
	 * @brief Names of all errors used across LLU
	 */
	namespace ErrorName {

		// Original LibraryLink error codes:
		extern const std::string VersionError;		 ///< same as LIBRARY_VERSION_ERROR
		extern const std::string FunctionError;		 ///< same as LIBRARY_FUNCTION_ERROR
		extern const std::string MemoryError;		 ///< same as LIBRARY_MEMORY_ERROR
		extern const std::string NumericalError;	 ///< same as LIBRARY_NUMERICAL_ERROR
		extern const std::string DimensionsError;	 ///< same as LIBRARY_DIMENSIONS_ERROR
		extern const std::string RankError;			 ///< same as LIBRARY_RANK_ERROR
		extern const std::string TypeError;			 ///< same as LIBRARY_TYPE_ERROR
		extern const std::string NoError;			 ///< same as LIBRARY_NO_ERROR

		// LibraryData errors
		extern const std::string LibDataError;	  ///< WolframLibraryData is not set

		// MArgument errors:
		extern const std::string MArgumentIndexError;			///< wrong argument index
		extern const std::string MArgumentNumericArrayError;	///< error involving NumericArray argument
		extern const std::string MArgumentTensorError;			///< error involving Tensor argument
		extern const std::string MArgumentImageError;			///< error involving Image argument

		// ErrorManager errors:
		extern const std::string ErrorManagerThrowIdError;		 ///< trying to throw exception with non-existent id
		extern const std::string ErrorManagerThrowNameError;	 ///< trying to throw exception with non-existent name
		extern const std::string ErrorManagerCreateNameError;	 ///< trying to register exception with already existing name

		// NumericArray errors:
		extern const std::string NumericArrayNewError;			 ///< creating new NumericArray failed
		extern const std::string NumericArrayCloneError;		 ///< NumericArray cloning failed
		extern const std::string NumericArrayTypeError;			 ///< NumericArray type mismatch
		extern const std::string NumericArraySizeError;			 ///< wrong assumption about NumericArray size
		extern const std::string NumericArrayIndexError;		 ///< trying to access non-existing element
		extern const std::string NumericArrayConversionError;	 ///< conversion from NumericArray of different type failed

		// MTensor errors:
		extern const std::string TensorNewError;	  ///< creating new MTensor failed
		extern const std::string TensorCloneError;	  ///< MTensor cloning failed
		extern const std::string TensorTypeError;	  ///< Tensor type mismatch
		extern const std::string TensorSizeError;	  ///< wrong assumption about Tensor size
		extern const std::string TensorIndexError;	  ///< trying to access non-existing element

		// MImage errors:
		extern const std::string ImageNewError;		 ///< creating new MImage failed
		extern const std::string ImageCloneError;	 ///< MImage cloning failed
		extern const std::string ImageTypeError;	 ///< Image type mismatch
		extern const std::string ImageSizeError;	 ///< wrong assumption about Image size
		extern const std::string ImageIndexError;	 ///< trying to access non-existing element

		// General container errors:
		extern const std::string CreateFromNullError;		   ///< attempting to create a generic container from nullptr
		extern const std::string MArrayElementIndexError;	   ///< attempting to access MArray element at invalid index
		extern const std::string MArrayDimensionIndexError;	   ///< attempting to access MArray dimension at invalid index

		// MathLink errors:
		extern const std::string MLNullMlinkError;			   ///< Trying to create MLStream with NULL MLINK
		extern const std::string MLTestHeadError;			   ///< MLTestHead failed (wrong head or number of arguments)
		extern const std::string MLPutSymbolError;			   ///< MLPutSymbol failed
		extern const std::string MLPutFunctionError;		   ///< MLPutFunction failed
		extern const std::string MLTestSymbolError;			   ///< MLTestSymbol failed (different symbol on the link than expected)
		extern const std::string MLWrongSymbolForBool;		   ///< Tried to read something else than "True" or "False" as boolean
		extern const std::string MLGetListError;			   ///< Could not get list from MathLink
		extern const std::string MLGetScalarError;			   ///< Could not get scalar from MathLink
		extern const std::string MLGetStringError;			   ///< Could not get string from MathLink
		extern const std::string MLGetArrayError;			   ///< Could not get array from MathLink
		extern const std::string MLPutListError;			   ///< Could not send list via MathLink
		extern const std::string MLPutScalarError;			   ///< Could not send scalar via MathLink
		extern const std::string MLPutStringError;			   ///< Could not send string via MathLink
		extern const std::string MLPutArrayError;			   ///< Could not send array via MathLink
		extern const std::string MLGetSymbolError;			   ///< MLGetSymbol failed
		extern const std::string MLGetFunctionError;		   ///< MLGetFunction failed
		extern const std::string MLPacketHandleError;		   ///< One of the packet handling functions failed
		extern const std::string MLFlowControlError;		   ///< One of the flow control functions failed
		extern const std::string MLTransferToLoopbackError;	   ///< Something went wrong when transferring expressions from loopback link
		extern const std::string MLCreateLoopbackError;		   ///< Could not create a new loopback link
		extern const std::string MLLoopbackStackSizeError;	   ///< Loopback stack size too small to perform desired action

		// DataList errors:
		extern const std::string DLNullRawNode;			 ///< DataStoreNode passed to Node wrapper was null
		extern const std::string DLInvalidNodeType;		 ///< DataStoreNode passed to Node wrapper carries data of invalid type
		extern const std::string DLGetNodeDataError;	 ///< DataStoreNode_getData failed
		extern const std::string DLNullRawDataStore;	 ///< DataStore passed to DataList was null
		extern const std::string DLPushBackTypeError;	 ///< Element to be added to the DataList has incorrect type

		// MArgument errors:
		extern const std::string ArgumentCreateNull;		  ///< Trying to create Argument object from nullptr
		extern const std::string ArgumentAddNodeMArgument;	  ///< Trying to add DataStore Node of type MArgument (aka MType_Undef)

		// ProgressMonitor errors:
		extern const std::string Aborted;	 ///< Computation aborted by the user

		// ManagedExpression errors:
		extern const std::string ManagedExprInvalidID;	  ///< Given number is not an ID of any existing managed expression
		extern const std::string MLEDynamicTypeError;	  ///< Invalid dynamic type requested for a Managed Library Expression

		// FileUtilities errors:
		extern const std::string PathNotValidated;		///< Given file path could not be validated under desired open mode
		extern const std::string InvalidOpenMode;		///< Specified open mode is invalid
		extern const std::string OpenFileFailed;		///< Could not open file
	}

}

#endif	  // LLUTILS_ERRORS_H
