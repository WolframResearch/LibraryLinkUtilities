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
MANAGED_EXPRESSION_CLASS(MyExpression) {
public:
	MyExpression(mint myID, std::string text) : id {myID}, text {std::move(text)} {
		LLU_DEBUG("MyExpression[", id, "] created.");
	}
	~MyExpression() {
		LLU_DEBUG("MyExpression[", id, "] is dying now.");
	}

	const std::string& getText() const {
		return text;
	}

	void setText(std::string newText) {
		text = std::move(newText);
	}
private:
	mint id;
	std::string text;
};

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	MyExpressionStore.registerType("MyExpression");
	return 0;
}

EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {
	MyExpressionStore.unregisterType(libData);
}

LIBRARY_LINK_FUNCTION(OpenManagedMyExpression) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(Argc, Args, Res);
		auto id = mngr.getInteger<mint>(0);
		auto text = mngr.getString(1);
		MyExpressionStore.createInstance(id, id, text);
	} catch (const LLU::LibraryLinkError &e) {
		err = e.which();
	}
	return err;
}

LIBRARY_LINK_FUNCTION(GetText) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(Argc, Args, Res);
		const MyExpression& myExpr = mngr.getManagedExpression(0, MyExpressionStore);
		mngr.set(myExpr.getText());
	} catch (const LLU::LibraryLinkError &e) {
		err = e.which();
	}
	return err;
}

LIBRARY_LINK_FUNCTION(SetText) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(Argc, Args, Res);
		auto& myExpr = mngr.getManagedExpression(0, MyExpressionStore);
		auto newText = mngr.getString(1);
		myExpr.setText(newText);
	} catch (const LLU::LibraryLinkError &e) {
		err = e.which();
	}
	return err;
}

LIBRARY_LINK_FUNCTION(JoinText) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(Argc, Args, Res);
		const auto& myExpr1 = mngr.getManagedExpression(0, MyExpressionStore);
		const auto& myExpr2 = mngr.getManagedExpression(1, MyExpressionStore);
		mngr.set(myExpr1.getText() + myExpr2.getText());
	} catch (const LLU::LibraryLinkError &e) {
		err = e.which();
	}
	return err;
}

/**
 * Read managed MyExpression via MathLink to a shared pointer.
 */
template<LLU::ML::Encoding EIn, LLU::ML::Encoding EOut>
LLU::MLStream<EIn, EOut>& operator>>(LLU::MLStream<EIn, EOut>& ml, std::shared_ptr<MyExpression>& myExp) {
	ml >> LLU::ML::Function("MyExpression", 1);
	mint myExprID {};
	ml >> myExprID;
	myExp = MyExpressionStore.getInstancePointer(myExprID);
	return ml;
}

/**
 * Get a reference to a managed MyExpression passed via MathLink
 */
template<LLU::ML::Encoding EIn, LLU::ML::Encoding EOut>
MyExpression& getFromMathLink(LLU::MLStream<EIn, EOut> &ml) {
	ml >> LLU::ML::Function("MyExpression", 1);
	mint myExprID{};
	ml >> myExprID;
	return MyExpressionStore.getInstance(myExprID);
}

/// Get two managed MyExpressions via MathLink and swap texts in them
LIBRARY_MATHLINK_FUNCTION(SwapText) {
	using namespace LLU::ML;
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MLStream<Encoding::UTF8> ml(mlp, 2);
		std::shared_ptr<MyExpression> firstExpr;
		ml >> firstExpr;
		auto& secondExpr = getFromMathLink(ml);
		auto tempText = firstExpr->getText();
		firstExpr->setText(secondExpr.getText());
		secondExpr.setText(std::move(tempText));
		ml << Null << EndPacket;
	}
	catch (const LLU::LibraryLinkError &e) {
		err = e.which();
	}
	return err;
}