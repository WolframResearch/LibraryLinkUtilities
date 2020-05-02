/**
 * @file	DataListTest.cpp
 * @date	September 6, 2018
 * @author	rafalc
 * @brief	Source code for unit tests of DataStore and its wrapper DataList<T>.
 */

#include <iostream>
#include <list>
#include <string>

#include "wstp.h"

#include "LLU/Containers/Iterators/DataList.hpp"
#include "LLU/LLU.h"
#include "LLU/LibraryLinkFunctionMacro.h"
#include "LLU/WSTP/WSStream.hpp"
#include "LLU/Utilities.hpp"

namespace WS = LLU::WS;
namespace LLErrorCode = LLU::ErrorCode;

using LLU::WSStream;
using LLU::DataList;
using LLU::GenericDataList;

/* Initialize Library */

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	return LLErrorCode::NoError;
}

/* Uninitialize Library */
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData) {}

/* Returns an input or a copy of an input */
LIBRARY_LINK_FUNCTION(PassDataStore) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr {Argc, Args, Res};
		auto dlIn = mngr.getDataList<LLU::TypedArgument>(0);
		auto returnCopyQ = mngr.getBoolean(1);

		if (returnCopyQ) {
			auto dlOut = dlIn.clone();
			mngr.set(dlOut);
		} else {
			mngr.set(dlIn);
		}

	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

/* Returns a DataStores carrying two input DataStores as nodes */
LIBRARY_LINK_FUNCTION(JoinDataStores) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr {Argc, Args, Res};

		auto ds1 = mngr.getGenericDataList(0);

		auto ds2 = mngr.getGenericDataList(1);

		auto returnCopyQ = mngr.getBoolean(2);

		DataList<GenericDataList> dsOut;
		if (returnCopyQ) {
			dsOut.push_back(ds1.clone());
			dsOut.push_back(ds2.clone());
		} else {
			dsOut.push_back(std::move(ds1));
			dsOut.push_back(std::move(ds2));
		}
		mngr.setDataList(dsOut);
	} catch (const LLU::LibraryLinkError& e) {
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
		LLU::MArgumentManager mngr {Argc, Args, Res};
		auto dsIn = mngr.getGenericDataList(0);
		dsIn.push_back(LLU::TypedArgument {std::in_place_type_t<GenericDataList>(), dsIn.getContainer(), LLU::Ownership::LibraryLink});
		LLU::DataNode<GenericDataList> n{dsIn.getLastNode()};
		mngr.set(dsIn);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

/* Returns a empty DataStore */
LIBRARY_LINK_FUNCTION(EmptyDataStore) {
	LLU::Unused(libData);
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr {Argc, Args, Res};
		DataList<std::string_view> ds;
		mngr.setDataList(ds);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

/* Reverse each string in a list of strings using DataList */
LIBRARY_LINK_FUNCTION(ReverseListOfStrings) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr {Argc, Args, Res};
		auto ds = mngr.getDataList<std::string_view>(0);
		DataList<std::string_view> dsOut;

		for (const auto& node : ds) {
			std::string_view s = node.value();
			std::string reversed {s.rbegin(), s.rend()};	// create reversed copy
			dsOut.push_back(reversed);
		}
		mngr.setDataList(dsOut);
	} catch (const LLU::LibraryLinkError& e) {
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
		if (ds_in == nullptr) {
			throw std::runtime_error("Invalid input DataStore");
		}
		length = libData->ioLibraryFunctions->DataStore_getLength(ds_in);
		if (length <= 0) {
			throw std::runtime_error("Invalid length of input DataStore");
		}

		ds_out = libData->ioLibraryFunctions->createDataStore();
		if (ds_out == nullptr) {
			throw std::runtime_error("Invalid output DataStore");
		}

		dsn = libData->ioLibraryFunctions->DataStore_getFirstNode(ds_in);
		while (dsn != nullptr) {
			if (libData->ioLibraryFunctions->DataStoreNode_getData(dsn, &data) != 0) {
				throw std::runtime_error("Could not get node data");
			}
			dsn = libData->ioLibraryFunctions->DataStoreNode_getNextNode(dsn);
			inStrList.emplace_back(MArgument_getUTF8String(data));
		}

		for (const auto& s : inStrList) {
			std::string outStr(s.rbegin(), s.rend());	 // create reversed copy
			libData->ioLibraryFunctions->DataStore_addString(ds_out, (char*)outStr.c_str());
		}

		MArgument_setDataStore(Res, ds_out);
	} catch (const LLU::LibraryLinkError& e) {
		errCode = e.which();
	} catch (...) {
		errCode = LLErrorCode::FunctionError;
		if (ds_out) {
			libData->ioLibraryFunctions->deleteDataStore(ds_out);
		}
	}
	return errCode;
}

/* Reverse each string in a list of strings using WSTP */
LIBRARY_WSTP_FUNCTION(ReverseListOfStringsWSTP) {
	auto err = LLErrorCode::NoError;
	try {
		WSStream<WS::Encoding::UTF8> ml {mlp, 1};
		std::vector<std::string> listOfStrings;
		ml >> listOfStrings;

		ml << WS::List(static_cast<int>(listOfStrings.size()));
		for (const auto& s : listOfStrings) {
			std::string outStr(s.rbegin(), s.rend());	 // create reversed copy
			ml << outStr;
		}
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(SeparateKeysAndValues) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr {libData, Argc, Args, Res};
		auto dsIn = mngr.getDataList<std::complex<double>>(0);
		DataList<std::string_view> keys;
		DataList<std::complex<double>> values;

		for (auto& [name, value] : dsIn) {
			keys.push_back(name);
			values.push_back(value);
		}

		DataList<GenericDataList> dsOut;
		dsOut.push_back("Keys", std::move(keys));
		dsOut.push_back("Values", std::move(values));

		mngr.set(dsOut);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(GetKeys) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr {Argc, Args, Res};
		auto dsIn = mngr.getDataList<LLU::TypedArgument>(0);
		DataList<std::string_view> keys;

		using NameIterator = LLU::NodeNameIterator<LLU::TypedArgument>;
		for (NameIterator it = dsIn.begin(); it != dsIn.end(); ++it) {
			keys.push_back(*it);
		}

		mngr.setDataList(keys);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(GetValuesReversed) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr {Argc, Args, Res};
		auto dsIn = mngr.getDataList<LLU::TypedArgument>(0);
		DataList<LLU::TypedArgument> values;

		using Iter = LLU::ReverseValueIterator<LLU::TypedArgument>;
		std::move(Iter {dsIn.rbegin()}, Iter {dsIn.rend()}, std::back_inserter(values));

		mngr.setDataList(values);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(FrameDims) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr {Argc, Args, Res};
		auto dsIn = mngr.getDataList<LLU::GenericImage>(0);

		LLU::NumericArray<std::uint64_t> dims {0, {static_cast<mint>(dsIn.size()), 2}};
		mint dimsIndex = 0;

		for (const auto& imgNode : dsIn) {
			dims[dimsIndex++] = static_cast<std::uint64_t>(imgNode.value().rows());
			dims[dimsIndex++] = static_cast<std::uint64_t>(imgNode.value().columns());
		}
		mngr.setNumericArray(dims);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(StringsThroughVectorReversed) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr {Argc, Args, Res};
		auto dsIn = mngr.getDataList<std::string_view>(0);

		using ValueIterator = LLU::NodeValueIterator<std::string_view>;
		std::vector<std::string> vecStr {ValueIterator {dsIn.begin()}, ValueIterator {dsIn.end()}};

		DataList<std::string_view> dsOut;

		std::transform(vecStr.rbegin(), vecStr.rend(), std::back_inserter(dsOut), [](const std::string& s) { return s; });

		mngr.setDataList(dsOut);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(IntsToNumericArray) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr {Argc, Args, Res};
		auto dsIn = mngr.getDataList<mint>(0);

		using ValueIterator = LLU::NodeValueIterator<mint>;
		LLU::NumericArray<mint> ra {ValueIterator {dsIn.begin()}, ValueIterator {dsIn.end()}};

		mngr.setNumericArray(ra);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LIBRARY_LINK_FUNCTION(GetLength) {
	auto err = LLErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr {Argc, Args, Res};
		auto dsIn = mngr.getGenericDataList(0);
		mngr.setInteger(dsIn.getLength());
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

LLU_LIBRARY_FUNCTION(CheckSizeChange) {
	auto n = mngr.getInteger<mint>(0);
	DataList<mint> dsInt;
	DataList<LLU::TypedArgument> dsArg;
	for (int i = 0; i < n; ++i) {
		dsInt.push_back(i);
		dsArg.push_back(static_cast<mint>(i));
	}
	LLU::Tensor<mint> res({dsInt.getLength(), static_cast<mint>(dsInt.size()), dsArg.getLength(), static_cast<mint>(dsArg.size())});
	mngr.setTensor(res);
}