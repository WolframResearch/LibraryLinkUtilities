## LibraryLink Utilities

_LibraryLink Utilities_ (abbr. LLU) is a set of modern C++ wrappers for most elements of standard LibraryLink C interface. Containers like MImage and MTensor are wrapped in templated classes. Managing MArguments (both input and output) is also delegated to a separate class:

| LibraryLink element 	|    LLU wrapper   	|
|:-------------------:	|:----------------:	|
| MTensor             	| Tensor<T>        	|
| MRawArray           	| RawArray<T>      	|
| MImage              	| Image<T>         	|
| MArgument           	| MArgumentManager 	|

For more details about each class see [the documentation](https://files.wolfram.com/temp-store/rafalc/LibraryLinkUtilities/index.html).

__The project is new and not really field-tested. Please send all your suggestions and bugs to <rafalc@wolfram.com>__

## Motivation

_LibraryLink_ is a great tool for connecting Wolfram Language with external libraries and programs written in C and it is widely used internally for developing paclets.
But as more and more paclets are now being developed in modern C++ the integration with _LibraryLink_, although still possible, becomes cumbersome and inelegant. Most significant features missing in _LibraryLink_ are:
* Automatic resource management
* Exception handling
* Iterators for MTensor and MRawArray
* Type safety

The motivation behind _LibraryLink Utilities_ is to provide the aforementioned features without touching _LibraryLink_ sources.

## Code Example

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

	EXTERN_C DLLEXPORT int repeatCharacters(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
		LLErrorCode err = LLErrorCode::NoError;
		try {
			// Create manager object
			MArgumentManager mngr(Argc, Args, Res);
	
			// Read string and RawArray arguments
			auto& string = mngr.getString(0);
			auto counts = mngr.getRawArray<std::uint8_t>(1);
	
			// check RawArray rank
			if (counts.rank() != 1) {
				throw LLError(LLErrorCode::RankError);
			}
	
			// check if RawArray length is equal to input string length
			if (counts.size() != string.size()) {
				throw LLError(LLErrorCode::DimensionsError);
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
	
			// no clean-up, just set the result
			mngr.setString(std::move(outString));
		}
		catch (LLError& e) {
			err = e.which();
		}
		catch (std::exception&) {
			err = LLErrorCode::FunctionError;
		}
		return static_cast<int>(err);
	}
```
### Paclets that currently use _LibraryLink Utilities_

- [PacletTemplate](https://stash.wolfram.com/projects/IMEX/repos/paclettemplate) - this is a model paclet for Import/Export developers
- [GIFTools](https://stash.wolfram.com/projects/IMEX/repos/giftools)
- [MediaTools](https://stash.wolfram.com/projects/IMEX/repos/mediatools)
- [HDF5Tools](https://stash.wolfram.com/projects/IMEX/repos/hdf5tools)

## How should you incorporate _LibraryLink Utilities_ into your project?

Currently the best way to include LLU into your project is by using git submodule. Submodules are simply git repos inside other repos but working with them may sometimes be tricky. See this excellent [tutorial on submodules](https://git-scm.com/book/en/v2/Git-Tools-Submodules).

For SourceTree users there is also a helpful [blog post](https://blog.sourcetreeapp.com/2012/02/01/using-submodules-and-subrepositories/).

In most cases you will access _LibraryLink Utilities_ in "read-only" manner, i.e. you will just update the submodule to make sure you use the most recent version. 

When you work on your paclet you may occasionally find a bug in LLU or a missing feature. You should either report it or try to make changes yourself following the usual workflow: create new branch - implement changes - open PR - merge. It is possible to do it with LLU as submodule in your project but you should really read the tutorial before you try.

__Remember to modify your build script so that LLU sources also get compiled__

### Quick guide for Import/Export developers
Here is a list of commands that will be useful to developers working on Import/Export paclets (names ending with "Tools"). Usually these paclets have _CPPSource/_ directory containing the source code. It is easy to modify these commands so that they work for arbitrary project.

#### Adding _LibraryLink Utilities_ to your paclet
`git submodule add ssh://git@stash.wolfram.com:7999/imex/librarylinkutilities.git CPPSource/LibraryLinkUtilities`

#### Cloning a project that already uses _LibraryLink Utilities_
`git clone --recursive ssh://git@stash.wolfram.com:7999/imex/<paclet_name_in_lowercase>.git`

#### Updating _LibraryLink Utilities_ in your project
`git submodule update --remote CPPSource/LibraryLinkUtilities/`


## API Reference

Doxygen is used to generate documentation for _LibraryLink Utilities_ API. You can browse generated docs online here: 

<https://files.wolfram.com/temp-store/rafalc/LibraryLinkUtilities/index.html>

## ToDo
Here is a list of LLU features that should be implemented, improved or more comprehensively tested:

- Implement creating new Image from data via `Image<T>::createInternal()`
- Make sure all MArgument passing modes ("Constant", "Shared", etc.) are correctly handled

## Contributors

Rafał Chojna - <rafalc@wolfram.com>