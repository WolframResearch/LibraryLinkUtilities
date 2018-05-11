# Table of contents

1. [Introduction](#introduction)
2. [Motivation](#motivation)
3. [Code Example](#example)
4. [MathLink support](#mathlink)
	1. [Main features](#mathlink-features)
	2. [Example](#mathlink-example)
5. [Limitations](#limitations)
6. [How to Use](#howToUse)
	1. [Quick Guide for I/E developers](#howToUse-quickguide)
	2. [Compilation](#howToUse-compilation)
7. [API reference](#APIreference)
8. [Contributors](#contributors)

<a name="introduction"></a>
# Introduction

_LibraryLink Utilities_ (abbr. LLU) is a set of modern C++ wrappers for most elements of standard LibraryLink C interface. Containers like MImage and MTensor are wrapped in templated classes. Managing MArguments (both input and output) is also delegated to a separate class:

| LibraryLink element 	|    LLU wrapper   	|
|:-------------------:	|:----------------:	|
| MTensor             	| Tensor<T>        	|
| MRawArray           	| RawArray<T>      	|
| MImage              	| Image<T>         	|
| MArgument           	| MArgumentManager 	|
| LinkObject			| MLStream			|

For more details about each class see [the documentation](http://malgorithmswin.wri.wolfram.com:8080/importexport/LLU).

__The project is new and not really field-tested. Please send all suggestions, feature requests and bug reports to <rafalc@wolfram.com>__

<a name="motivation"></a>
# Motivation

_LibraryLink_ is a great tool for connecting Wolfram Language with external libraries and programs written in C and it is widely used internally for developing paclets.
But as more and more paclets are now being developed in modern C++ the integration with _LibraryLink_, although still possible, becomes cumbersome and inelegant. Most significant features missing in _LibraryLink_ are:
* Automatic resource management
* Exception handling
* Iterators for MTensor and MRawArray
* Class-like interface for LibraryLink data structures, for example `rank()` as member function of Tensor class instead of separate function 
`mint (*MTensor_getRank)(MTensor)`, or a copy constructor instead of `int (*MTensor_clone)(MTensor, MTensor*)`
* Type safety

The motivation behind _LibraryLink Utilities_ is to provide the aforementioned features without touching _LibraryLink_ sources.

<a name="example"></a>
# Code Example

Probably the best way to see how to use LLU and what advantages it has over classic _LibraryLink_ is by comparing the same function written in two different styles. Below we will implement a simple function `repeatCharacters` that takes a string `s` and a tensor `t` and returns a new string `s2` that consists of each character `s[i]` from original string but repeated `t[i]` times, so for example

	repeatCharacters("abc", {3, 2, 1})
gives 

	"aaabbc"  

C - style implementation:
```c

	// global variable which is the buffer for strings returned to LibraryLink
	char* outString = NULL;
	
	EXTERN_C DLLEXPORT int repeatCharacters(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
		char* string = NULL;
		MRawArray counts;
		uint8_t* countsData = NULL;
		size_t outStringIndex = 0;
		size_t len, j;
		mint sum = 0;
		mint c;
		
		string = MArgument_getUTF8String(Args[0]);
		counts = MArgument_getMRawArray(Args[1]);
	
		// check RawArray type
		if (libData->rawarrayLibraryFunctions->MRawArray_getType(counts) != MRawArray_Type_Ubit8) {
			libData->UTF8String_disown(string);
			return LIBRARY_TYPE_ERROR;
		}
	
		// check RawArray rank
		if (libData->rawarrayLibraryFunctions->MRawArray_getRank(counts) != 1) {
			libData->UTF8String_disown(string);
			return LIBRARY_RANK_ERROR;
		}
	
		// check if RawArray length is equal to input string length
		len = strlen(string);
		if (libData->rawarrayLibraryFunctions->MRawArray_getFlattenedLength(counts) != len) {
			libData->UTF8String_disown(string);
			return LIBRARY_DIMENSION_ERROR;
		}
	
		// before we allocate memory for the output string, we have to sum all RawArray elements to see how many bytes are needed
		countsData = (uint8_t*) libData->rawarrayLibraryFunctions->MRawArray_getData(counts);
		for (j = 0; j < len; j++) {
			sum += countsData[j];
		}
	
		// free memory owned by global buffer
		free(outString);
		outString = NULL;
	
		// allocate memory for output string, outString has to be a global variable, because it will be returned to LibraryLink
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
```

and C++ version with _LibraryLink Utilities_:

```cpp

	EXTERN_C DLLEXPORT int repeatCharactersNew(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
		auto err = LLErrorCode::NoError;
		try {
			// Create manager object
			MArgumentManager mngr(libData, Argc, Args, Res);
	
			// Read string and RawArray arguments
			auto& string = mngr.getString(0);
			auto counts = mngr.getRawArray<std::uint8_t>(1);
	
			// check RawArray rank
			if (counts.rank() != 1) {
				ErrorManager::throwException(LLErrorName::RankError);
			}
	
			// check if RawArray length is equal to input string length
			if (counts.size() != string.size()) {
				ErrorManager::throwException(LLErrorName::DimensionsError);
			}
	
			// before we allocate memory for the output string, we have to sum all RawArray elements to see how many bytes are needed
			auto sum = std::accumulate(counts.begin(), counts.end(), static_cast<size_t>(0));
	
			// allocate memory for output string
			std::string outString;
			outString.reserve(sum);
	
			// populate output string
			for (mint i = 0; i < counts.size(); i++) {
				outString.append(std::string(counts[i], string[i]));
			}
	
			// clean up and set result
			mngr.setString(std::move(outString));
		}
		catch (LibraryLinkError& e) {
			err = e.id();
		}
		catch (std::exception&) {
			err = LLErrorCode::FunctionError;
		}
		return err;
	}
```
<a name="mathlink"></a>
# MathLink support

_LibraryLink_ allows you to pass LinkObject as argument, which may then be utilized to exchange data between your library and the Kernel using MathLink. 
The original MathLink API is in old C style with error codes, macros, manual memory management, etc. Therefore, __LLU__ provides a wrapper for the LinkObject called `MLStream`.

`MLStream` is actually a class template parameterized by the default encoding to be used for strings, but for the sake of clarity, the template parameter is skipped in the remainder of this README.

<a name="mathlink-features"></a>
## Main features
### Convenient syntax

In this extension to __LLU__ MathLink is interpreted as an i/o stream, so operators << and >> are utilized to make the syntax cleaner and more concise. This means that the framework frees the developer from the responsibility to choose proper MathLink function for the data they intend to read or write.

### Error checking

Each call to MathLink has its return status checked. In case of failure an exception is thrown. Such exceptions carry some debug info to help locate the problem. Sample debug info looks like this:

```
Error code reported by MathLink: 48
"Unable to convert from given character encoding to MathLink encoding"
Additional debug info: MLPutUTF8String
```

### Memory cleanup

You're no longer required to call `MLRelease*` on the data received from MathLink. The framework does it for you.

### Automated handling of common data types

Some sophisticated types can be sent to Mathematica directly via `MLStream` class. For example nested maps:

```cpp
std::map<std::string, std::map<int, std::vector<double>>> myNestedMap
```

Just write `ms << myNestedMap` and you will get a nested Association on the other side. It works in the other direction as well. Obviously, for the above to work, key and value type in the map must be supported by MathLink.

If you have any particular type that you think should be directly supported by `MLStream`, please let me know.

### Easily extendable to custom classes

Suppose you have a structure

```cpp
struct Color {
    double red;
    double green;
    double blue;
};
```

It is enough to overload `operator<<` like this:

```cpp
MLStream& operator<<(MLStream& ms, const Color& c) {
    return ms << ML::Function("RGBColor", 3) << c.red << c.green << c.blue;
}
```

And now you're able to send objects of class `Color` directly via `MLStream`.

<a name="mathlink-example"></a>
## Example
Again, let's compare the same piece of code written in plain _LibraryLink_ with one written with _LLU_ and `MLStream`. Take a look at the code snippet taken from one of the Import/Export paclets:

```cpp
if (!MLNewPacket(mlp)) {
    wsErr = -1;
    goto cleanup;
}
if (!MLPutFunction(mlp, "List", nframes)) {
    wsErr = -1;
    goto cleanup;
}
for (auto& f : extractedFrames) {
    if (!MLPutFunction(mlp, "List", 7)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutFunction(mlp, "Rule", 2)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutString(mlp, "ImageSize")) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutFunction(mlp, "List", 2)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutInteger64(mlp, f->width)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutInteger64(mlp, f->height)) {
        wsErr = -1;
        goto cleanup;
    }
    (...)
    if (!MLPutFunction(mlp, "Rule", 2)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutString(mlp, "ImageOffset")) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutFunction(mlp, "List", 2)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutInteger64(mlp, f->left)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutInteger64(mlp, f->top)) {
        wsErr = -1;
        goto cleanup;
    }
    (...)
    if (!MLPutFunction(mlp, "Rule", 2)) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutString(mlp, "UserInputFlag")) {
        wsErr = -1;
        goto cleanup;
    }
    if (!MLPutSymbol(mlp, f->userInputFlag == true ? "True" : "False")) {
        wsErr = -1;
        goto cleanup;
    }
}
if (!MLEndPacket(mlp)) { 
/* unable to send the end-of-packet sequence to mlp */
}
if (!MLFlush(mlp)){ 
/* unable to flush any buffered output data in mlp */
}
```

and now the same code using `MLStream`:

```cpp
MLStream ms(mlp);

ms << ML::NewPacket;
ms << ML::List(nframes);

for (auto& f : extractedFrames) {
    ms << ML::List(7)
        << ML::Rule 
            << "ImageSize" 
            << ML::List(2) << f->width << f->height
        (...)
        << ML::Rule 
            << "ImageOffset" 
            << ML::List(2) << f->left << f->top
        (...)
        << ML::Rule 
            << "UserInputFlag" 
            << f->userInputFlag
}

ms << ML::EndPacket << ML::Flush;
```
<a name="limitations"></a>
# Limitations with respect to LibraryLink

There are some LibraryLink features currently not covered by _LLU_, most notably:

- Sparse Arrays
- Tensor subsetting: `MTensor_getTensor`
- Raw Array type conversion: `MRawArray_convertType`
- Managed Library Expressions
- Callbacks
- Wolfram IO Library (asynchronous tasks and Data Store)

For now LibraryLink does not allow to write generic code that would clean up memory after Tensors, RawArrays, etc. independently of passing mode used ("Automatic", "Shared", ...). See [this suggestion](http://bugs.wolfram.com/show?number=337331) for more details. In consequence, _LLU_ guarantees to correctly handle only those data structures that were created with _LLU_. 
Structures received as MArguments will not be automatically freed, therefore you may want to use passing modes that do not require clean-up (like "Constant" or Automatic). In case of "Shared" passing, the only guarantee is that `disown()` will be called on destruction of each object that has positive `shareCount()`. 
Please consult [LibraryLink tutorial](https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithMathematica.html#97446640) for more details.

<a name="howToUse"></a>
# How should you incorporate _LibraryLink Utilities_ into your project?

Currently the best way to include LLU into your project is by using git submodule. Submodules are simply git repos inside other repos but working with them may sometimes be tricky. See this excellent [tutorial on submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

For SourceTree users there is also a helpful [blog post](https://blog.sourcetreeapp.com/2012/02/01/using-submodules-and-subrepositories/).

In most cases you will access _LibraryLink Utilities_ in "read-only" manner, i.e. you will just update the submodule to make sure you use the most recent version. 

When you work on your paclet you may occasionally find a bug in LLU or a missing feature. You should either report it or try to make changes yourself following the usual workflow: create new branch - implement changes - open PR - merge. It is possible to do it with LLU as submodule in your project but you should really read the tutorial before you try.

<a name="howToUse-quickguide"></a>
## Quick guide for Import/Export developers
Here is a list of commands that will be useful to developers working on Import/Export paclets (names ending with "Tools"). Usually these paclets have _CPPSource/_ directory containing the source code. It is easy to modify these commands so that they work for arbitrary project.

#### Adding _LibraryLink Utilities_ to your paclet
`git submodule add ssh://git@stash.wolfram.com:7999/imex/librarylinkutilities.git CPPSource/LibraryLinkUtilities`

#### Cloning a project that already uses _LibraryLink Utilities_
`git clone --recursive ssh://git@stash.wolfram.com:7999/imex/<paclet_name_in_lowercase>.git`

#### Updating _LibraryLink Utilities_ in your project
`git submodule update --remote CPPSource/LibraryLinkUtilities/`

<a name="howToUse-compilation"></a>
## Compilation
After checking out the submodule remember to modify your build script accordingly so that _LLU_ sources also get compiled. Since the source code uses C++14 features, you have to make sure you enabled C++14 support in your compiler. *Visual Studio 2015* or later provides the support by default and in *gcc* or *clang* you may have to add **-std=c++14** flag. 

Minimum required version of *gcc* is 5 and for *clang* it is 3.4.


## Paclets that currently use _LibraryLink Utilities_

- [PacletTemplate](https://stash.wolfram.com/projects/IMEX/repos/paclettemplate) - this is a model paclet for Import/Export developers
- [GIFTools](https://stash.wolfram.com/projects/IMEX/repos/giftools)
- [MediaTools](https://stash.wolfram.com/projects/IMEX/repos/mediatools)
- [RAWTools](https://stash.wolfram.com/projects/IMEX/repos/rawtools)

<a name="APIreference"></a>
# API Reference

Doxygen is used to generate documentation for _LibraryLink Utilities_ API. You can browse generated docs online here: 

<http://malgorithmswin.wri.wolfram.com:8080/importexport/LLU>

<a name="contributors"></a>
# Contributors

* Rafał Chojna (<rafalc@wolfram.com>) - main developer
* Sean Cheren  (<scheren@wolfram.com>) - top-level code for error handling