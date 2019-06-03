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
#include "LLU/Containers/Iterators/DataList.hpp"

using LibraryLinkUtils::MLStream;
namespace ML = LibraryLinkUtils::ML;

/* Initialize Library */

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	return LLErrorCode::NoError;
}

/* Uninitialize Library */
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData) {
}

/* Returns an input or a copy of an input */
LIBRARY_LINK_FUNCTION(PassDataStore) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { Argc, Args, Res };
		auto dlIn { mngr.getDataList<MArgumentType::MArgument>(0) };
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
			DataList<MArgumentType::DataStore, Passing::Manual> dsOut { dsOut1.getContainer(), dsOut2.getContainer() };
			mngr.setDataList(dsOut);
		} else {
			DataList<MArgumentType::DataStore, Passing::Manual> dsOut { ds1.getContainer(), ds2.getContainer() };
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
		dsIn.push_back<MArgumentType::DataStore>(dsIn.getContainer());
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
		for (ValueIterator it = ds.begin(); it != ds.end(); ++it) {
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

/* Reverse each string in a list of strings using raw DataStore */
LIBRARY_LINK_FUNCTION(ReverseListOfStringsLibraryLink) {
	auto errCode = LLErrorCode::NoError;
	DataStore ds_out = nullptr;
	try {
		DataStore ds_in = nullptr;

		DataStoreNode dsn = nullptr;
		mint length = 0;
		std::list<std::string> inStrList;
		MArgument data;

		/* Argument checking */
		if (Argc != 1) {
			throw std::runtime_error("Invalid number of args");
		}

		ds_in = MArgument_getDataStore(Args[0]);
		if(ds_in == nullptr) {
			throw std::runtime_error("Invalid input DataStore");
		}
		length = libData->ioLibraryFunctions->DataStore_getLength(ds_in);
		if(length <= 0) {
			throw std::runtime_error("Invalid length of input DataStore");
		}

		ds_out = libData->ioLibraryFunctions->createDataStore();
		if(ds_out == nullptr) {
			throw std::runtime_error("Invalid output DataStore");
		}

		dsn = libData->ioLibraryFunctions->DataStore_getFirstNode(ds_in);
		while(dsn != nullptr) {
			if(libData->ioLibraryFunctions->DataStoreNode_getData(dsn, &data) != 0) {
				throw std::runtime_error("Could not get node data");
			}
			dsn = libData->ioLibraryFunctions->DataStoreNode_getNextNode(dsn);
			inStrList.emplace_back(MArgument_getUTF8String(data));
		}

		for (const auto& s : inStrList) {
			std::string outStr(s.rbegin(), s.rend());	// create reversed copy
			libData->ioLibraryFunctions->DataStore_addString(ds_out, (char*)outStr.c_str());
		}

		MArgument_setDataStore(Res, ds_out);
	} catch (const LibraryLinkError& e) {
		errCode = e.which();
	} catch (...) {
		errCode = LLErrorCode::FunctionError;
		if(ds_out) {
			libData->ioLibraryFunctions->deleteDataStore(ds_out);
		}
	}
	return errCode;
}

/* Reverse each string in a list of strings using MathLink */
LIBRARY_MATHLINK_FUNCTION(ReverseListOfStringsMathLink) {
	auto err = LLErrorCode::NoError;
	try {
		MLStream<ML::Encoding::UTF8> ml { mlp, 1 };
		std::vector<std::string> listOfStrings;
		ml >> listOfStrings;

		ml << ML::List(static_cast<int>(listOfStrings.size()));
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

LIBRARY_LINK_FUNCTION(SeparateKeysAndValues) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { libData, Argc, Args, Res };
		auto dsIn = mngr.getDataList<MArgumentType::Complex>(0);
		DataList<MArgumentType::UTF8String> keys;
		DataList<MArgumentType::Complex> values;

		for(auto&& listElem : dsIn) {
			std::cout << listElem.getName() << std::endl;
			keys.push_back(const_cast<char*>(listElem.getName().c_str()));
			values.push_back(listElem.getValue());
		}

		DataList<MArgumentType::DataStore> dsOut { {"Keys", keys.abandonContainer()}, {"Values", values.abandonContainer()}};
		mngr.setDataList(dsOut);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(GetKeys) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { Argc, Args, Res };
		auto dsIn = mngr.getDataList<MArgumentType::MArgument>(0);
		DataList<MArgumentType::UTF8String> keys;

		using NameIterator = LibraryLinkUtils::NodeNameIterator<MArgumentType::MArgument>;

		for(NameIterator it = dsIn.begin(); it != dsIn.end(); ++it) {
			keys.push_back(const_cast<char*>(it->c_str()));
		}
		mngr.setDataList(keys);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(GetValuesReversed) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { Argc, Args, Res };
		auto dsIn = mngr.getDataList<MArgumentType::MArgument>(0);
		DataList<MArgumentType::MArgument> values;

		using ValueIterator = LibraryLinkUtils::ReverseValueIterator<MArgumentType::MArgument>;

		for(ValueIterator it = dsIn.rbegin(); it != dsIn.rend(); ++it) {
			values.push_back(*it, it.getNode().getRawType());
		}
		mngr.setDataList(values);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(FrameDims) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { Argc, Args, Res };
		auto dsIn = mngr.getDataList<MArgumentType::Image>(0);

		NumericArray<std::uint64_t> dims { 0, { static_cast<mint>(dsIn.size()), 2} };
		mint dimsIndex = 0;

		using ValueIterator = LibraryLinkUtils::NodeValueIterator<MArgumentType::Image>;
		for(ValueIterator it = dsIn.begin(); it != dsIn.end(); ++it) {
			Image<float> im { *it };
			dims[dimsIndex++] = static_cast<std::uint64_t>(im.dimension(0));
			dims[dimsIndex++] = static_cast<std::uint64_t>(im.dimension(1));
		}
		mngr.setNumericArray(dims);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(StringsThroughVectorReversed) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { Argc, Args, Res };
		auto dsIn = mngr.getDataList<MArgumentType::UTF8String>(0);


		using ValueIterator = LibraryLinkUtils::NodeValueIterator<MArgumentType::UTF8String>;
		std::vector<std::string> vecStr { ValueIterator { dsIn.begin() }, ValueIterator { dsIn.end() } };

		DataList<MArgumentType::UTF8String> dsOut;

		std::transform(vecStr.rbegin(), vecStr.rend(), std::back_inserter(dsOut), [](const std::string& s){
			return const_cast<char*>(s.c_str());
		});

		mngr.setDataList(dsOut);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(IntsToNumericArray) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr { Argc, Args, Res };
		auto dsIn = mngr.getDataList<MArgumentType::Integer>(0);

		using ValueIterator = LibraryLinkUtils::NodeValueIterator<MArgumentType::Integer>;
		NumericArray<mint> ra { ValueIterator { dsIn.begin() }, ValueIterator { dsIn.end() } };

		mngr.setNumericArray(ra);
	} catch (const LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}
