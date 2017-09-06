/* Include required header */

#include <unordered_map>
#include <memory>

#include "WolframLibrary.h"
#include "MArgumentManager.h"
#include "Tensor.h"

using namespace LibraryLinkUtils;

static std::unordered_map<mint, std::unique_ptr<Tensor<mint>>>map;

enum {
	A_ = 0,
	C_,
	M_,
	X_
};

DLLEXPORT void manage_instance(WolframLibraryData libData, mbool mode, mint id) {
	if (mode == 0) {
		map[id] = std::make_unique<Tensor<mint>>();
	}
	else {
		map.erase(id);
	}
}

EXTERN_C DLLEXPORT int releaseInstance(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument res) {
	MArgumentManager mngr(Argc, Args, res);
	auto id = mngr.getInteger<mint>(0);
	return libData->releaseManagedLibraryExpression("LCG", id);
}

EXTERN_C DLLEXPORT int setInstanceState(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, res);
		auto id = mngr.getInteger<mint>(0);
		auto T = mngr.getTensor<mint>(1);

		if (T.rank() != 1)
			ErrorManager::throwException("RankError");
		if (T.dimension(0) != 4)
			ErrorManager::throwException("DimensionsError");

		map[id] = std::make_unique<Tensor<mint>>(T);

	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int getInstanceState(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto id = mngr.getInteger<mint>(0);

		auto* T = map.at(id).get();

		auto Tcopy = *T;
		mngr.setTensor(Tcopy);

	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int generateFromInstance(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		auto id = mngr.getInteger<mint>(0);

		auto* T = map.at(id).get();
		auto* p = T->data();
		if (!p)
			ErrorManager::throwException("FunctionError");

		auto a = p[A_];
		auto c = p[C_];
		auto m = p[M_];
		auto x = p[X_];

		auto Tdims = mngr.getTensor<mint>(1);
		if (Tdims.rank() != 1) {
			ErrorManager::throwException("TypeError");
		}

		Tensor<double> Tres(0., std::move(Tdims));

		for (auto& d : Tres) {
			x = (a * x + c) % m;
			d = static_cast<double>(x) / m;
		}
		p[X_] = x;

		mngr.setTensor(Tres);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int getAllInstanceIDs(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		//auto id = mngr.getInteger<mint>(0);

		std::vector<mint> keys;
		keys.reserve(map.size());
		for (const auto& pair : map) {
			keys.push_back(pair.first);
		}

		Tensor<mint> Tres(keys);
		mngr.setTensor(Tres);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

/* Return the version of Library Link */
EXTERN_C DLLEXPORT mint WolframLibrary_getVersion() {
	return WolframLibraryVersion;
}

/* Initialize Library */
EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	return libData->registerLibraryExpressionManager("LCG", manage_instance);
}

/* Uninitialize Library */
EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {
	libData->unregisterLibraryExpressionManager("LCG");
}

