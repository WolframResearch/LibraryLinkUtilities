===========================================
Function arguments
===========================================

Passing data between Wolfram Language and external C/C++ libraries is a core feature of LibraryLink. This is far from a straightforward task, because in
Wolfram Language everything is an expression and variable's type can change at run time, whereas C and C++ are both statically typed. Apart from that,
every C/C++ library may define custom data types it uses.

LibraryLink does the heavy lifting by providing translation between popular Wolfram Language expression types and corresponding C types. For instance, when you pass
a ``String`` expression to the library function, you will receive a null-terminated ``char*`` in the C code, passing a ``NumericArray`` will yield
an object of type ``MNumericArray``. There is still some work needed to translate LibraryLink types into external library types.

In practice, what you will receive in a library function as input arguments from Wolfram Language is an array of ``MArgument``, which is a union type::

	typedef union {
		mbool *boolean;
		mint *integer;
		mreal *real;
		mcomplex *cmplex;
		MTensor *tensor;
		MSparseArray *sparse;
		MNumericArray *numeric;
		MImage *image;
		char **utf8string;
	} MArgument;


Similarly, there is one ``MArgument`` to store the result of your library function that you want to return to the Wolfram Language. You must also remember that
some types of arguments need special treatment, for example you must call ``UTF8String_disown`` on string arguments to avoid memory leaks.


LLU hides all those implementation details in the :cpp:class:`MArgumentManager<LLU::MArgumentManager>` class. You still need to know what are actual
argument types but you can now extract arguments using member functions like :cpp:func:`getInteger<LLU::MArgumentManager::getInteger>`,
:cpp:func:`getString<LLU::MArgumentManager::getString>` etc. and set the resulting value with :cpp:func:`set<LLU::MArgumentManager::set>` without
worrying about memory management.

Examples
================

Write a library function that adds two integers

.. code-block:: cpp
   :linenos:

   EXTERN_C DLLEXPORT int AddTwoIntegers(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
       MArgumentManager(libData, Argc, Args, Res);
       auto n1 = mngr.getInteger<mint>(0);  // get first (index = 0) argument, which is of type mint
       auto n2 = mngr.getInteger<mint>(1);  // get second argument which is also an integer

       mngr.set(n1 + n2);  // set the sum of arguments to be the result
       return ErrorCode::NoError;
   }

Such function, when compiled into a shared library, say :file:`myLib.so`, could then be loaded into WolframLanguage and used like this:

.. code-block:: mma

   AddInts = LibraryFunctionLoad["myLib.so", "AddTwoIntegers", {Integer, Integer}, Integer];

   AddInts[17, 25]
   (* = 42 *)


API reference
================

.. doxygenclass:: LLU::MArgumentManager
   :members: