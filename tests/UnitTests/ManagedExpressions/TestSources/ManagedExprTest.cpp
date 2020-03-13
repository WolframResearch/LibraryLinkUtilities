/**
 * @file	ManagedExprTest.cpp
 * @brief
 */
#include <functional>

#include <LLU/ErrorLog/Logger.h>
#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>
#include <LLU/ManagedExpression.hpp>

/**
 * Sample class to be "managed" by WL.
 * The only requirement is for the class to have a public constructor.
 */
class MyExpression {
public:
	MyExpression(mint myID, std::string text) : id {myID}, text {std::move(text)} {
		LLU_DEBUG("MyExpression[", id, "] created.");
	}
	virtual ~MyExpression() {
		LLU_DEBUG("MyExpression[", id, "] is dying now.");
	}

	const std::string& getText() const {
		return text;
	}

	virtual void setText(std::string newText) {
		text = std::move(newText);
	}

protected:
	mint id;

private:
	std::string text;
};

DEFINE_MANAGED_STORE_AND_SPECIALIZATION(MyExpression)

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	LLU::LibraryData::setLibraryData(libData);
	MyExpressionStore.registerType("MyExpression");
	return 0;
}

EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {
	MyExpressionStore.unregisterType(libData);
}

LLU_LIBRARY_FUNCTION(GetManagedExpressionCount) {
	mngr.set(static_cast<mint>(MyExpressionStore.size()));
}

LLU_LIBRARY_FUNCTION(GetManagedExpressionTexts) {
	LLU::DataList<LLU::MArgumentType::UTF8String> texts;
	for (const auto& expr : MyExpressionStore) {
		texts.push_back(std::to_string(expr.first), const_cast<char*>(expr.second->getText().c_str()));
	}
	mngr.set(texts);
}

LIBRARY_LINK_FUNCTION(OpenManagedMyExpression) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(Argc, Args, Res);
		auto id = mngr.getInteger<mint>(0);
		auto text = mngr.getString(1);
		MyExpressionStore.createInstance(id, id, text);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	}
	return err;
}

LLU_LIBRARY_FUNCTION(ReleaseExpression) {
	auto id = mngr.getInteger<mint>(0);
	mngr.set(static_cast<mint>(MyExpressionStore.releaseInstance(id)));
}

LIBRARY_LINK_FUNCTION(GetText) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(Argc, Args, Res);
		const MyExpression& myExpr = mngr.getManagedExpression(0, MyExpressionStore);
		mngr.set(myExpr.getText());
	} catch (const LLU::LibraryLinkError& e) {
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
	} catch (const LLU::LibraryLinkError& e) {
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
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	}
	return err;
}

LIBRARY_LINK_FUNCTION(GetMyExpressionStoreName) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(Argc, Args, Res);
		mngr.set(MyExpressionStore.getExpressionName());
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	}
	return err;
}

/**
 * Read managed MyExpression via WSTP to a shared pointer.
 */
template<LLU::WS::Encoding EIn, LLU::WS::Encoding EOut>
LLU::WSStream<EIn, EOut>& operator>>(LLU::WSStream<EIn, EOut>& ws, std::shared_ptr<MyExpression>& myExp) {
	ws >> LLU::WS::Function("MyExpression", 1);
	mint myExprID {};
	ws >> myExprID;
	myExp = MyExpressionStore.getInstancePointer(myExprID);
	return ws;
}

/**
 * Get a reference to a managed MyExpression passed via WSTP
 */
template<LLU::WS::Encoding EIn, LLU::WS::Encoding EOut>
MyExpression& getFromWSTP(LLU::WSStream<EIn, EOut>& ws) {
	ws >> LLU::WS::Function("MyExpression", 1);	   // Watch out for context here!
	mint myExprID {};							   // In paclets the function head will usually be XXXTools`Private`MyExpression
	ws >> myExprID;
	return MyExpressionStore.getInstance(myExprID);
}

/// Get two managed MyExpressions via WSTP and swap texts in them
LIBRARY_WSTP_FUNCTION(SwapText) {
	namespace WS = LLU::WS;
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::WSStream<WS::Encoding::UTF8> ws(mlp, 2);
		std::shared_ptr<MyExpression> firstExpr;
		ws >> firstExpr;
		auto& secondExpr = getFromWSTP(ws);
		auto tempText = firstExpr->getText();
		firstExpr->setText(secondExpr.getText());
		secondExpr.setText(std::move(tempText));
		ws << WS::Null << WS::EndPacket;
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	}
	return err;
}

LIBRARY_WSTP_FUNCTION(SetTextWS) {
	namespace WS = LLU::WS;
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::WSStream<WS::Encoding::UTF8> ws(mlp, 2);
		auto& myExpr = getFromWSTP(ws);
		std::string newText;
		ws >> newText;
		myExpr.setText(std::move(newText));
		ws << WS::Null << WS::EndPacket;
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	}
	return err;
}

// Create a subclass of MyExpression and use it as Managed Expression with the same Store
// All previously defined Get/Set functions should work on MLEs that are actually MyChildExpressions

class MyChildExpression : public MyExpression {
public:
	MyChildExpression(mint myID, std::string text) : MyExpression(myID, "") {
		setText(std::move(text));
		LLU_DEBUG("MyChildExpression[", id, "] created.");
	}

	~MyChildExpression() override {
		LLU_DEBUG("MyChildExpression[", id, "] is dying now.");
	}

	mint getCounter() {
		return ++counter;
	}

	void setText(std::string newText) override {
		MyExpression::setText(std::string {"I'm a subclass! Here is your text: "} + std::move(newText));
	}

private:
	mint counter = 0;
};

LIBRARY_LINK_FUNCTION(OpenManagedMyChildExpression) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(Argc, Args, Res);
		auto id = mngr.getInteger<mint>(0);
		auto text = mngr.getString(1);
		MyExpressionStore.createInstance<MyChildExpression>(id, id, text);
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	}
	return err;
}

LIBRARY_LINK_FUNCTION(GetCounter) {
	auto err = LLU::ErrorCode::NoError;
	try {
		LLU::MArgumentManager mngr(Argc, Args, Res);
		MyChildExpression& myExpr = mngr.getManagedExpression<MyExpression, MyChildExpression>(0, MyExpressionStore);
		mngr.set(myExpr.getCounter());
	} catch (const LLU::LibraryLinkError& e) {
		err = e.which();
	}
	return err;
}
