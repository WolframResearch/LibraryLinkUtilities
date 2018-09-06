/** 
 * @file	DataListTest.cpp
 * @date	September 6, 2018
 * @author	rafalc
 * @brief	Source code for unit tests of DataStore and its wrapper DataList<T>.
 */

#include <iostream>
#include <list>
#include <string>

#include "mathlink.h"
#include "WolframLibrary.h"
#include "WolframNumericArrayLibrary.h"
#include "WolframSparseLibrary.h"
#include "WolframImageLibrary.h"
#include "WolframIOLibraryFunctions.h"

#include "LLU/LLU.h"
#include "LLU/LibraryLinkFunctionMacro.h"
#include "LLU/Utilities.hpp"
#include "LLU/ML/MLStream.hpp"

using LibraryLinkUtils::MLStream;
namespace ML = LibraryLinkUtils::ML;

/* Initialize Library */

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	std::cout << libData << std::endl;
	return LLErrorCode::NoError;
}

/* Uninitialize Library */
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {
	return;
}

/* Returns an input or a copy of an input */
LIBRARY_LINK_FUNCTION(PassDataStore) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { Argc, Args, Res };
		auto dlIn = mngr.getDataList<MArgumentType::MArgument>(0);
		auto returnCopyQ = mngr.getBoolean(1);

		if (returnCopyQ) {
			auto dlOut = dlIn.clone();
			mngr.setDataList(dlOut);
		} else {
			mngr.setDataList(dlIn);
		}

	} catch(const LibraryLinkError& e) {
		err = e.which();
	} catch(...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

/* Returns a DataStores carrying two input DataStores as nodes */
LIBRARY_LINK_FUNCTION(JoinDataStores) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { Argc, Args, Res };

		auto ds1 = mngr.getDataList<MArgumentType::MArgument>(0);

		auto ds2 = mngr.getDataList<MArgumentType::MArgument>(1);

		auto returnCopyQ = mngr.getBoolean(2);

		if (returnCopyQ) {
			auto dsOut1 = ds1.clone();
			auto dsOut2 = ds2.clone();
			DataList<MArgumentType::DataStore, Passing::Manual> dsOut { dsOut1.getInternal(), dsOut2.getInternal() };
			mngr.setDataList(dsOut);
		} else {
			DataList<MArgumentType::DataStore, Passing::Manual> dsOut { ds1.getInternal(), ds2.getInternal() };
			mngr.setDataList(dsOut);
		}
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}


/* Returns a copy of the input plus its own reference */
LIBRARY_LINK_FUNCTION(TestSelfReferencialDataStore) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { Argc, Args, Res };
		auto dsIn = mngr.getDataList<MArgumentType::MArgument>(0);
		dsIn.push_back<MArgumentType::DataStore>(dsIn.getInternal());
		mngr.setDataList(dsIn);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}


/* Returns a empty DataStore */
LIBRARY_LINK_FUNCTION(EmptyDataStore) {
	LibraryLinkUtils::Unused(libData);
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { Argc, Args, Res };
		DataList<MArgumentType::UTF8String> ds;
		mngr.setDataList(ds);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

/* Reverse each string in a list of strings using DataStore */
LIBRARY_LINK_FUNCTION(ReverseListOfStrings) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { Argc, Args, Res };
		auto ds = mngr.getDataList<MArgumentType::UTF8String>(0);
		DataList<MArgumentType::UTF8String> dsOut;

		using ValueIterator = LibraryLinkUtils::NodeValueIterator<MArgumentType::UTF8String>;
		for (ValueIterator it { ds.begin() }; it != ds.end(); ++it) {
			std::string value { *it };
			std::string reversed { value.rbegin(), value.rend() };    // create reversed copy
			dsOut.push_back(const_cast<char*>(reversed.c_str()));
		}
		mngr.setDataList(dsOut);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_MATHLINK_FUNCTION(ReverseListOfStringsMathLink) {
	auto err = LLErrorCode::NoError;
	try {
		MLStream<ML::Encoding::UTF8> ml { mlp, 1 };
		std::vector<std::string> listOfStrings;
		ml >> listOfStrings;

		ml << ML::List(listOfStrings.size());
		for (const auto& s : listOfStrings) {
			std::string outStr(s.rbegin(), s.rend());	// create reversed copy
			ml << outStr;
		}
	} catch(const LibraryLinkError& e) {
		err = e.which();
	} catch(...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}
