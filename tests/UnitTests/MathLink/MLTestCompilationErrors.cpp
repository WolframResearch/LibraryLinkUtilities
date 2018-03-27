/** 
 * @file	MLTestCompilationErrors.cpp
 * @date	Jan 30, 2018
 * @author	Rafal Chojna <rafalc@wolfram.com>
 * @brief	Source code for MathLinkStream unit tests.
 */
#include <iostream>

#include "mathlink.h"

#include "LLU.h"
#include "LibraryLinkFunctionMacro.h"
#include "ML/MathLinkStream.hpp"

using LibraryLinkUtils::MathLinkStream;
namespace ML = LibraryLinkUtils::ML;

// This function should trigger compiler errors
LIBRARY_MATHLINK_FUNCTION(Wrong) {
	auto err = LLErrorCode::NoError;
	try {
		MathLinkStream ml(mlp, "List", 0);

		unsigned int i { 129 };

		ml >> i; 		// ERROR (static_assert): "Calling operator>> with unsupported type."

		ml << i;		// ERROR (static_assert): "Calling operator<< with unsupported type."

		i = ML::GetScalar<unsigned int>::get(mlp);  		// ERROR (static_assert): Trying to use ML::GetScalar<T> for unsupported type T

		ML::PutScalar<unsigned int>::put(mlp, i);			// ERROR (static_assert): Trying to use ML::PutScalar<T> for unsupported type T

		ml << static_cast<mlint64>(i); 		// This should be fine

	} catch (LibraryLinkError& e) {
		err = e.which();
		std::cerr << e.debug() << std::endl;
	} catch (...) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

