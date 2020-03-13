/**
 * @file	MArgumentManagerCompilationErrors.cpp
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Source code for MArgumentManager unit tests containing functions that should fail at compile stage.
 */
#include <tuple>

#include <LLU/LLU.h>
#include <LLU/LibraryLinkFunctionMacro.h>

class A {};

LLU_LIBRARY_FUNCTION(UnregisteredArg) {
	[[maybe_unused]] auto f = mngr.get<float>(0);
	[[maybe_unused]] auto a = mngr.get<A>(1);
}

LLU_LIBRARY_FUNCTION(UnregisteredRetType) {
	A a;
	mngr.set(a);
}

LLU_LIBRARY_FUNCTION(GetArgsWithIndices) {
	[[maybe_unused]] auto [x, y] = mngr.get<unsigned short, mint>({1, 2, 3});
}