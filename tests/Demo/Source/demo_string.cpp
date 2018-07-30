/*
 An example that demonstrates using Shared memory management for 
 communicating between Mathematica and a Wolfram Library.
 */

#include <algorithm>
#include <cstring>
#include <string>

#include "WolframLibrary.h"

#include "LLU/MArgumentManager.h"
#include "LLU/LibraryLinkError.h"

using namespace LibraryLinkUtils;

EXTERN_C DLLEXPORT mint WolframLibrary_getVersion() {
	return WolframLibraryVersion;
}

EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	MArgumentManager::setLibraryData(libData);
	return 0;
}

static std::string s;

EXTERN_C DLLEXPORT void WolframLibrary_uninitialize(WolframLibraryData libData) {

}

EXTERN_C DLLEXPORT int countSubstring(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		const auto& instring = mngr.getString(0);
		const auto& substring = mngr.getString(1);

		mint c = 0;
		mint n = instring.length();
		mint slen = substring.length();
		if (n > slen) {
			n -= slen;
			for (mint i = 0; i <= n; i++) {
				if (!strncmp(instring.c_str() + i, substring.c_str(), slen)) {
					c++;
				}
			}
		}
		mngr.setInteger(c);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int encodeString(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		s = mngr.getString(0);
		auto shift = mngr.getInteger<mint>(1);

		/* Find shift mod 127 so we only
		 deal with positive numbers below */
		shift = shift % 127;
		if (shift < 0)
			shift += 127;

		shift -= 1;

		for (auto& c : s) {
			/* Error for non ASCII string */
			if (c & 128)
				return LIBRARY_FUNCTION_ERROR;
			c = ((c + shift) % 127) + 1;
		}
		mngr.setString(s);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

EXTERN_C DLLEXPORT int reverseString(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	auto err = LLErrorCode::NoError;
	try {
		MArgumentManager mngr(Argc, Args, Res);
		s = mngr.getString(0);

		for (auto& c : s) {
			/* Error for non ASCII string */
			if (c & 128)
				return LIBRARY_FUNCTION_ERROR;
		}

		mint n = s.length() / 2;
		mint len = s.length() - 1; /* For index origin 0 */
		for (mint i = 0; i < n; i++) {
			std::swap(s[i], s[len - i]);
		}
		mngr.setString(s);
	}
	catch (LibraryLinkError& e) {
		err = e.which();
	}
	catch (std::exception& e) {
		err = LLErrorCode::FunctionError;
	}
	return err;
}

