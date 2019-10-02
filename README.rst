LibraryLink Utilities
=================================================

:dfn:`LibraryLink Utilities` (abbr. LLU) is a set of modern C++ wrappers over most parts of :term:`LibraryLink` - the Wolfram Language framework for connecting
to C/C++ libraries. LLU started as an internal Wolfram project which can now be used as a stand-alone library and is available publicly on GitHub.

This documentation is split into modules, each describes a core part of LLU providing general overview, examples and detailed description of related C++ entities.

.. For more details about each class see [the documentation](http://algorithms.wolfram.com:8080/documentation/LibraryLinkUtilities).

Please use GitHub to report bugs, make suggestions and request features.


Motivation
------------------------------

:term:`LibraryLink` is a great tool for connecting Wolfram Language with external libraries and programs written in C and it is widely used internally for developing paclets.
But as more and more paclets are now being developed in modern C++ the integration with *LibraryLink*, although still possible, becomes cumbersome and inelegant.
Most significant features missing in *LibraryLink* from the standpoint of C++ developer are:

* automatic resource management
* exception handling
* container iterators
* class-like interface for data structures, for example :cpp:`rank()` as member function of :cpp:`Tensor` class instead of separate function ``mint MTensor_getRank(MTensor)``, or a copy constructor instead of ``int MTensor_clone(MTensor, MTensor*)``
* type safety

The motivation behind *LibraryLink Utilities* is to provide the aforementioned features without touching *LibraryLink* sources.



Example
----------------------------------

Probably the best way to see how to use :abbr:`LLU (LibraryLink Utilities)` and what advantages it has over classic :term:`LibraryLink` is by comparing
the same function written in two different styles.
Below we will implement a simple function ``repeatCharacters`` that takes a string ``s`` and a tensor ``t`` and returns a new string ``s2`` that consists of each
character ``s[i]`` from original string but repeated ``t[i]`` times, so for example

.. code-block:: cpp

   repeatCharacters("abc", {3, 2, 1})

gives

.. parsed-literal::

   "aaabbc"

First, C - style implementation:

.. code-block:: c
   :dedent: 1

	// global variable which is the buffer for strings returned to LibraryLink
	char* outString = NULL;

	EXTERN_C DLLEXPORT int repeatCharacters(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	    char* string = NULL;
	    MNumericArray counts;
	    uint8_t* countsData = NULL;
	    size_t outStringIndex = 0;
	    size_t len, j;
	    mint sum = 0;
	    mint c;

	    string = MArgument_getUTF8String(Args[0]);
	    counts = MArgument_getMNumericArray(Args[1]);

	    // check NumericArray type
	    if (libData->numericarrayLibraryFunctions->MNumericArray_getType(counts) != MNumericArray_Type_UBit8) {
	        libData->UTF8String_disown(string);
	        return LIBRARY_TYPE_ERROR;
	    }

	    // check NumericArray rank
	    if (libData->numericarrayLibraryFunctions->MNumericArray_getRank(counts) != 1) {
	        libData->UTF8String_disown(string);
	        return LIBRARY_RANK_ERROR;
	    }

	    // check if NumericArray length is equal to input string length
	    len = strlen(string);
	    if (libData->numericarrayLibraryFunctions->MNumericArray_getFlattenedLength(counts) != len) {
	        libData->UTF8String_disown(string);
	        return LIBRARY_DIMENSION_ERROR;
	    }

	    // before we allocate memory for the output string, we have to sum all NumericArray elements
		// to see how many bytes are needed
	    countsData = (uint8_t*) libData->numericarrayLibraryFunctions->MNumericArray_getData(counts);
	    for (j = 0; j < len; j++) {
	        sum += countsData[j];
	    }

	    // free memory owned by global buffer, if any (for example from the previous call to this function)
	    free(outString);
	    outString = NULL;

	    // allocate memory for output string, outString has to be a global variable,
		// because it will be returned to LibraryLink
	    outString = (char*) malloc(sum + 1);
	    if (!outString) {
	        libData->UTF8String_disown(string);
	        return LIBRARY_FUNCTION_ERROR;
	    }

	    // populate output string
	    for (j = 0; j < len; j++) {
	        for (c = 0; c < countsData[j]; c++) {
	            outString[outStringIndex++] = string[j];
	        }
	    }

	    // add null terminator
	    outString[sum] = '\0';

	    // clean up and set result
	    libData->UTF8String_disown(string);
	    MArgument_setUTF8String(Res, outString);

	    return LIBRARY_NO_ERROR;
	}


and corresponding C++ version written with *LibraryLink Utilities*:

.. code-block:: cpp
   :dedent: 1

	EXTERN_C DLLEXPORT int repeatCharactersLLU(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
	    auto err = LLU::ErrorCode::NoError;
	    try {
	        // Create manager object
	        LLU::MArgumentManager mngr(libData, Argc, Args, Res);

	        // Read string and NumericArray arguments
	        auto string = mngr.getString(0);
	        auto counts = mngr.getNumericArray<std::uint8_t>(1);

	        // check NumericArray rank
	        if (counts.rank() != 1) {
	            LLU::ErrorManager::throwException(LLU::ErrorName::RankError);
	        }

	        // check if NumericArray length is equal to input string length
	        if (counts.size() != string.size()) {
	            LLU::ErrorManager::throwException(LLU::ErrorName::DimensionsError);
	        }

	        // before we allocate memory for the output string, we have to sum all NumericArray elements
			// to see how many bytes are needed
	        auto sum = std::accumulate(counts.begin(), counts.end(), static_cast<size_t>(0));

	        // allocate memory for output string
	        std::string outString;
	        outString.reserve(sum);

	        // populate output string
	        for (mint i = 0; i < counts.size(); i++) {
	            outString.append(std::string(counts[i], string[i]));
	        }

	        // clean up and set result
	        mngr.set(std::move(outString));
	    }
	    catch (const LLU::LibraryLinkError& e) {
	        err = e.which();
	    }
	    catch (const std::exception&) {
	        err = LLU::ErrorCode::FunctionError;
	    }
	    return err;
	}

Limitations with respect to LibraryLink
---------------------------------------------

There are some *LibraryLink* features currently not covered by *LLU*, most notably:

- Sparse Arrays
- Tensor subsetting: `MTensor_getTensor`
- Callbacks
- Wolfram IO Library (asynchronous tasks)


Glossary
================

.. glossary::

	LibraryLink
		Wolfram Language framework

	LibraryLink Utilities
	LLU
		wrapper over :term:`LibraryLink`

	library function
		a C/C++ function with special signature

		.. code-block:: cpp

		   int f (WolframLibraryData libData, mint Argc, MArgument* Args, MArgument Res);

		or

		.. code-block:: cpp

		   int f (WolframLibraryData libData, MLINK mlp);

		Such functions can later be loaded into Wolfram Language using :wl:`LibraryFunctionLoad`.

Contributors
==================

* Rafał Chojna (rafalc) - main developer
* Sean Cheren  (scheren) - top-level code for error handling, CMake improvements
* Rebecca Frederick (rebeccaf) - CMake improvements