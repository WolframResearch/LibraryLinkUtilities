/**
 * @file	LoggerTest.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief 	Unit tests for Logger
 */
#include <chrono>
#include <sstream>
#include <thread>

#include "LLU/ErrorLog/Logger.h"
#include "LLU/LibraryLinkFunctionMacro.h"
#include "LLU/LLU.h"
#include "LLU/ProgressMonitor.h"


LIBRARY_LINK_FUNCTION(GreaterAt) {
	LLU_DEBUG("Library function entered with ", Argc, " arguments.");
	auto err = LLErrorCode::NoError;
	try {
		LLU_DEBUG("Starting try-block, current error code: ", static_cast<int>(err));
		MArgumentManager mngr(Argc, Args, Res);
		auto fileName = mngr.getString(0);
		if (fileName.find(':') != std::string::npos) {
			LLU_WARNING("File name ", fileName, " contains a possibly problematic character \":\".");
		}
		LLU_DEBUG("Input tensor is of type: ", mngr.getTensorType(1));
		if (mngr.getTensorType(1) == MType_Complex) {
			LLU_ERROR("Input tensor contains complex numbers which is not supported");
			mngr.setBoolean(false);
			return err;
		}
		auto t = mngr.getTensor<mint>(1);
		auto index1 = mngr.getInteger<mint>(2);
		auto index2 = mngr.getInteger<mint>(3);
		if (index1 <= 0 || index2 <= 0) {
			ErrorManager::throwExceptionWithDebugInfo(LLErrorName::TensorIndexError,
					"Indices (" + std::to_string(index1) + ", " +  std::to_string(index2) + ") must be positive.");
		}
		LLU_DEBUG("Comparing ", t.at(index1 - 1), " with ", t.at(index2 - 1));
		mngr.setBoolean(t.at(index1 - 1) > t.at(index2 - 1));
	}
	catch (const LibraryLinkError& e) {
		LLU_ERROR("Caught LLU exception ", e.what(), ": ", e.debug());
		err = e.which();
	}
	catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}
