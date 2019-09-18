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
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr { Argc, Args, Res };
		auto dsIn = mngr.getDataList<LLU::MArgumentType::Tensor>(0);
		dsIn.push_back<LLU::MArgumentType::DataStore>(dsIn.getContainer()); // compile time error - "Trying to add DataList node of incorrect type."
		mngr.setDataList(dsIn);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLU::ErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(TryToAddMArgument) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr{ Argc, Args, Res };
		auto dsIn = mngr.getDataList<LLU::MArgumentType::Real>(0);

		LLU::Argument<LLU::MArgumentType::MArgument>::addDataStoreNode(dsIn.getContainer(), "NoName", Args[0]); // compile time error - attempting to use deleted function

		mngr.setDataList(dsIn);
	}
	catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	}
	catch (...) {
		err = LLU::ErrorCode::FunctionError;
	}
	return err;
}
