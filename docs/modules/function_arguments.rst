===========================================
Function arguments
===========================================

Passing data between Wolfram Language and external C/C++ libraries is a core feature of LibraryLink. This is far from a straightforward task, because in
Wolfram Language everything is an expression and variable's type can change at run time, whereas C and C++ variables are statically typed. Apart from that,
every C/C++ library may define custom data types it uses.

LibraryLink does the heavy lifting by providing translation between popular Wolfram Language expression types and corresponding C types. For instance, when you pass
a ``String`` expression to the library function, you will receive a null-terminated ``char*`` in the C code, or passing a ``NumericArray`` will yield
an object of type ``MNumericArray``.

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

Developers are encouraged to consult the `official LibraryLink guide <https://reference.wolfram.com/language/LibraryLink/tutorial/LibraryStructure.html#606935091>`_.

LLU hides all those implementation details in the :cpp:class:`MArgumentManager<LLU::MArgumentManager>` class. You still need to know what are actual
argument types but you can now extract arguments using member functions like :cpp:func:`getInteger<LLU::MArgumentManager::getInteger>`,
:cpp:func:`getString<LLU::MArgumentManager::getString>` etc. and set the resulting value with :cpp:func:`set<LLU::MArgumentManager::set>` without
worrying about memory management.

Examples
================

Write a library function that adds two integers

.. code-block:: cpp

   EXTERN_C DLLEXPORT int AddTwoIntegers(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
       MArgumentManager mngr {libData, Argc, Args, Res};
       auto n1 = mngr.get<mint>(0);  // get first (index = 0) argument, which is of type mint
       auto n2 = mngr.get<mint>(1);  // get second argument which is also an integer

       mngr.set(n1 + n2);  // set the sum of arguments to be the result
       return ErrorCode::NoError;
   }

Such function, when compiled into a shared library, say :file:`myLib.so`, could be loaded into WolframLanguage and used like this:

.. code-block:: mma

   AddInts = LibraryFunctionLoad["myLib.so", "AddTwoIntegers", {Integer, Integer}, Integer];

   AddInts[17, 25]
   (* = 42 *)


User-defined types
=====================

LibraryLink supports a number of types as function arguments and for the majority of use cases the built-in types are enough. However, imagine you are writing
a library that operates on financial data and it processes amounts of money. For example, in Wolfram Language you work with expressions like
``Quantity[20.3, "USD"]`` and in C++ you have a corresponding structure:

.. code-block:: cpp

   struct Money {
       double amount;
       std::string currency;
   };

If you want to write a library function that takes an amount of money and a currency and converts that amount to the given currency, you will probably choose
``{Real, String, String}`` for argument types (``Quantity`` would be split into Real and String and the second String is for the new currency)
and ``"DataStore"`` for the return type. This requires some extra code on the Wolfram Language side to extract Real and String from the Quantity and
on the C++ side to construct a DataStore from a Money object. Having large number of functions in the library that may repeat those translations, you will
probably decide to factor this extra code to helper functions.

You could then use your library in Wolfram Language as follows:

.. code-block:: mma
   :force:

   (* Load raw library function that operates on basic LibraryLink types *)
   $ConvertMoney = LibraryFunctionLoad["myLib.so", "ConvertMoney", {Real, String, String}, "DataStore"];

   (* Create a higher-level wrapper for users of your package *)
   ConvertMoney[amount_Quantity, newCurrency_String] := With[
      {
         rawlibraryResult = $ConvertMoney[QuantityMagnitude[amount], QuantityUnit[amount], newCurrency];
      },
      $dataStoreToQuantity[rawLibraryResult]  (* $dataStoreToQuantity is a small utility function, omitted for brevity *)
   ];

   ConvertMoney[Quantity[50., "USD"], "PLN"]
   (* = Quantity[XXX, "PLN"] *)

The implementation of ``ConvertMoney`` in C++ would go along the lines:

.. code-block:: cpp

   EXTERN_C DLLEXPORT int ConvertMoney(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
       MArgumentManager mngr {libData, Argc, Args, Res};
       auto amount = mngr.get<double>(0);
       auto oldCurrency = mngr.get<std::string>(1);
       auto newCurrency = mngr.get<std::string>(2);

       auto moneyToConvert = Money { amount, oldCurrency };
       Money converted = myLib::convert(moneyToConvert, newCurrency);

       mngr.set(myLib::MoneyToDataList(converted));  // myLib::MoneyToDataList is a helper function to convert Money object to a DataList
       return ErrorCode::NoError;
   }


This is a fine code and if you are satisfied with it, you can stop reading here. However, it is possible with LLU to implement the same functionality like this:

.. code-block:: mma
   :force:

   (* Load "ConvertMoney" function from "myLib.so" and assign it to ConvertMoney symbol *)
   `LLU`LoadLibraryFunction[ConvertMoney, "myLib.so", "ConvertMoney", {"Money", String}, "Money"];

   (* No need for separate higher-level wrapper because the types are translated by LLU now. *)

   ConvertMoney[Quantity[50., "USD"], "PLN"]
   (* = Quantity[XXX, "PLN"] *)

and in C++

.. code-block:: cpp

   EXTERN_C DLLEXPORT int ConvertMoney(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
       MArgumentManager mngr {libData, Argc, Args, Res};
       auto moneyToConvert = mngr.get<Money>(0);
       auto newCurrency = mngr.get<std::string>(2);  // under the hood Money object is still sent as two values (Real + String), so new currency has index 2

       Money converted = myLib::convert(moneyToConvert, newCurrency);

       mngr.set(converted);
       return ErrorCode::NoError;
   }

The point is to delegate the translation between your types and LibraryLink types to LLU, so that you can write cleaner code that does not distract readers
with technicalities.
To achieve this, you need to teach LLU to understand your types. Here is how you register ``"Money"`` as a library function argument type, the values of which
are of the form ``Quantity[_Real, _String]``:

.. code-block:: mma
   :force:

   `LLU`MArgumentType["Money", {Real, String}, (Sequence[QuantityMagnitude[#], QuantityUnit[#]]) &];

The second argument is the list of basic LibraryLink types that constitute to a single expression of type ``"Money"``. The third argument is a translation
function that takes something of the form ``Quantity[_Real, _String]`` and produces a ``Sequence`` of two values: Real and String.
In the C++ code we used ``mngr.get<Money>``, which means we have to specialize ``MArgumentManager::get`` template to work with ``Money`` type:

.. code-block:: cpp

    template<>
    Money LLU::MArgumentManager::get<Money>(size_type index) const {
        return std::make_from_tuple<Money>(get<double, std::string>({index, index + 1}));
    }

In this code we say that to read an object of type Money from input arguments starting at position ``index``, MArgumentManager first needs to read two
consecutive input arguments as ``double`` and ``std::string``, respectively, and construct a ``Money`` object from those 2 values.

At this point, LLU knows how to change WL expressions of the form ``Quantity[_Real, _String]`` into ``Money`` objects in C++. The only thing left is to teach
LLU how to work in the other direction, i.e. how to return ``Money`` objects via "DataStore" and change them into Quantity. First, let us specialize
``MArgumentManager::set`` template:

.. code-block:: cpp

    template<>
    void LLU::MArgumentManager::set<Money>(const Money& m) const {
        DataList<MArgumentType::MArgument> moneyDS;
        moneyDS.push_back<MArgumentType::Real>(m.amount);
        moneyDS.push_back<MArgumentType::UTF8String>(const_cast<char*>(m.currency.c_str()));
        set(moneyDS);
    }

You can read more about :cpp:class:`DataList <template\<MArgumentType T, class PassingMode = Passing::Manual> LLU::DataList>` in the section
about :doc:`containers`. The last part is to tell LLU how to turn incoming DataStores into Quantities in library functions that declare "Money" as return type:

.. code-block:: mma
   :force:

   `LLU`MResultType["Money", "DataStore", (Quantity @@ #)&];

Here we say that if a library function has return type "Money", then the corresponding LibraryLink type is "DataStore" and when we get such a DataStore
we need to apply a function ``(Quantity @@ #)&`` to turn it into the form that we use to represent Money expressions.

Registering user-defined types in LLU may seem like a lot of extra work, but actually it is no extra work at all. It is merely a way to organize the code
that you would previously have written anyway in the form of small utility functions scattered all over your library and possibly even duplicated,
if you are not careful enough.

API reference
================

.. doxygenclass:: LLU::MArgumentManager
   :members:
