CMake utility functions
================================

When LLU became a common library used in many Import/Export paclets, we started adding CMake utilities there which are not directly related to LLU but they
are common building blocks of paclets' build routines. For instance, things like downloading pre-built dependencies from CVS, locating Mathematica installation,
determining build platform and SystemID, etc. are all done in many paclets, so LLU offers utility functions for each of those tasks located in
:file:`cmake/WolframUtilities.cmake`.

Each function is documented in the source code but at some point we may move the documentation here.
