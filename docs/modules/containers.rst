=================================================
Containers
=================================================

Overview
============================

Raw _LibraryLink_ containers like MTensor or MNumericArray store their element type as a regular field in the stucture
meaning that the type cannot be used in compile-time context, which makes writing generic code that does something with
the underlying data very difficult (lots of switches and code repetition).

On the other hand, having element type as template parameter, like in STL containers, is often inconvenient and requires
some template magic for simple things like passing forward the container or reading metadata when the data type is not
known a priori.

To get the best of two worlds at cost of a bit more complicated interface LLU provides the following hierarchy of container
classes:

Level 0
----------------------------------

* MTensor
* MNumericArray
* MImage
* DataStore

These are just raw _LibraryLink_ containers. If someone wants to use them directly, they probably don't need LLU at all, but it's possible to use them within LLU as well.

Level 1A
----------------------------------

* [GenericTensor\<PassingMode\>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1MContainer_3_01MArgumentType_1_1Tensor_00_01PassingMode_01_4.html)
* [GenericNumericArray\<PassingMode\>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1MContainer_3_01MArgumentType_1_1NumericArray_00_01PassingMode_01_4.html)
* [GenericImage\<PassingMode\>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1MContainer_3_01MArgumentType_1_1Image_00_01PassingMode_01_4.html)
* [GenericDataList\<PassingMode\>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1MContainer_3_01MArgumentType_1_1DataStore_00_01PassingMode_01_4.html)

These are type-unaware wrappers, offer automatic memory management and basic interface like access to metadata (dimensions, rank, etc). No direct access to underlying data.

Level 1B
----------------------------------

- Type-aware interface to [Tensor](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1TypedTensor.html)
- Type-aware interface to [NumericArray](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1TypedNumericArray.html)
- Type-aware interface to [Image](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1TypedImage.html)

These template classes offer iterators and data access functions for each container. They shouldn't be used directly,
as they don't hold any data. Instead, use containers from level 2 which inherits from level 1B containers.

Level 2
----------------------------------

- [Tensor<T, PassingMode>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1Tensor.html)
- [NumericArray<T, PassingMode>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1NumericArray.html)
- [Image<T, PassingMode>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1Image.html)
- [DataList<T, PassingMode>](http://algorithms.wolfram.com:8080/documentation/LLU2_0_0/classLLU_1_1DataList.html)

Full-fledged wrappers with automatic memory management (via Passing policies, see section below), type-safe data access, iterators, etc.

The following table summarizes current status of _LibraryLink_ containers and their LLU wrappers:

+---------------------+--------------------------+--------------------+
| LibraryLink element |    Generic wrapper       |   Typed wrapper    |
+=====================+==========================+====================+
|       MTensor       |    GenericTensor\<P\>    |    Tensor<T, P>    |
+---------------------+--------------------------+--------------------+
|    MNumericArray    | GenericNumericArray\<P\> | NumericArray<T, P> |
+---------------------+--------------------------+--------------------+
|       MImage        |    GenericImage\<P\>     |    Image<T, P>     |
+---------------------+--------------------------+--------------------+
|      DataStore      |   GenericDataList\<P\>   |   DataList<T, P>   |
+---------------------+--------------------------+--------------------+

Passing policies
============================

When passing a container from WL to a C++ library you have to choose one of the 4 available passing modes:

* Automatic
* Constant
* Manual
* Shared

With exception of DataStore, which cannot be Shared.

All of the above are described in the [LibraryLink documentation](https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#97446640).

In plain _LibraryLink_, the choice you make is reflected only in the WL code where you call `LibraryFunctionLoad` and specify
the list of parameters for the library function. There is no way to query the WolframLibraryData or MArgument about
the passing modes of function arguments from within C++ code. Therefore, the programmer must remember the passing mode
for each argument and then ensure the correct action is taken (releasing/not releasing memory depending
on the combination of passing mode and whether the container has been returned from the library function as result to WL).
This design is far from perfect because manual resource management often leads to bugs and leaks.

As a remedy for this flaw of _LibraryLink_, LLU encodes the passing mode in a form of template parameter for each
container wrapper. It makes sense because passing mode is known at compile time and cannot be changed throughout
the life of container.

LLU defines 3 classes representing passing policies:

* Passing::Automatic
* Passing::Manual
* Passing::Shared

They serve as base classes to containers and they store and update the information whether the underlying raw container
should be freed when the wrapper ends its life.

There is also `Passing::Constant` which is just an alias for Passing::Automatic because from the memory management
point of view these two policies are equivalent.

Some examples:

.. code-block:: cpp
   :linenos:
   :dedent: 1

	Tensor<mint, Passing::Manual> t { 1, 2,  3, 4, 5 };    // fine, new MTensor is allocated and it will be freed when t goes out of scope

	Tensor<mint, Passing::Automatic> s { 1, 2,  3, 4, 5 };     // compile-time error, you cannot create a container with Automatic mode
	                                                           // because LibraryLink doesn't know about it and will not free it automatically

	auto t = mngr.getGenericImage<Shared>(0);   // OK

	auto copy = t;    // compile-time error, you cannot copy a Shared container because the copy will not be shared

	LLU::GenericImage<Manual> clone {t};   // but this is fine, we make a deep copy which is no longer Shared

More examples can be found in unit tests.
