/**
 * @file	DataListCompilationErrors.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @date	07.09.18
 * @brief	Source code for DataList unit tests containing functions that should fail at compile stage.
 */

#include "LLU/LLU.h"
#include "LLU/LibraryLinkFunctionMacro.h"
#include "LLU/MArgument.h"


LIBRARY_LINK_FUNCTION(WrongNodeType) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { Argc, Args, Res };
		auto dsIn = mngr.getDataList<MArgumentType::Tensor>(0);
		dsIn.push_back<MArgumentType::DataStore>(dsIn.getInternal()); // compile time error - "Trying to add DataList node of incorrect type."
		mngr.setDataList(dsIn);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(TryToAddMArgument) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr{ Argc, Args, Res };
		auto dsIn = mngr.getDataList<MArgumentType::Real>(0);

		LibraryLinkUtils::Argument<MArgumentType::MArgument>::addDataStoreNode(dsIn.getInternal(), "NoName", Args[0]); // compile time error - attempting to use deleted function

		mngr.setDataList(dsIn);
	}
	catch (const LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}