/**
 * @file	ManagedExprTest.cpp
 * @brief	
 */
#include <functional>

#include <LLU/LLU.h>
#include <LLU/ErrorLog/Logger.h>
#include <LLU/LibraryLinkFunctionMacro.h>
#include <LLU/ManagedExpression.hpp>

/**
 * Sample class to be "managed" by WL.
 * The only requirement is for the class to have a public constructor.
 */
class MyExpression {
public:
	explicit MyExpression(mint myID, std::string text) : id {myID}, text {std::move(text)} {
		LLU_DEBUG("MyExpression[", id, "] created.");
	}
	~MyExpression() {
		LLU_DEBUG("MyExpression[", id, "] is dying now.");
	}

	const std::string& getText() const {
		return text;
	}
private:
	mint id;
	std::string text;
};

LLU::ManagedExpressionStore<MyExpression> myExpressionStore;


template<>
void LLU::ManageInstance<MyExpression>(WolframLibraryData, mbool mode, mint id) {
	myExpressionStore.manageInstance(mode, id);
}

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	myExpressionStore.registerType("MyExpression");
	return 0;
}

EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {
	myExpressionStore.unregisterType(libData);
}

LIBRARY_LINK_FUNCTION(OpenManagedMyExpression) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(Argc, Args, Res);
		auto id = mngr.getInteger<mint>(0);
		auto text = mngr.getString(1);
		myExpressionStore.createInstance(id, id, text);
	} catch (const LLU::LibraryLinkError &e) {
		err = e.which();
	}
	return err;
}

LIBRARY_LINK_FUNCTION(GetText) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(Argc, Args, Res);
		auto myExpr = mngr.getManagedExpression(0, myExpressionStore);
		mngr.set(myExpr.getText());
	} catch (const LLU::LibraryLinkError &e) {
		err = e.which();
	}
	return err;
}