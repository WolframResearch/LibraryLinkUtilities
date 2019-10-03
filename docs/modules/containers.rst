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

* DataStore
* MImage
* MNumericArray
* MTensor

These are just raw _LibraryLink_ containers. If someone wants to use them directly, they probably don't need LLU at all, but it's possible to use them within LLU as well.

Level 1A
----------------------------------

* :cpp:type:`template\<class PassingMode> LLU::GenericDataList`
* :cpp:type:`template\<class PassingMode> LLU::GenericImage`
* :cpp:type:`template\<class PassingMode> LLU::GenericNumericArray`
* :cpp:type:`template\<class PassingMode> LLU::GenericTensor`

These are type-unaware wrappers, offer automatic memory management and basic interface like access to metadata (dimensions, rank, etc). No direct access to underlying data.

Level 1B
----------------------------------

- :cpp:class:`TypedImage\<T> <template\<typename T> LLU::TypedImage>` - type-aware interface to :cpp:class:`Image <template\<typename T, class PassingMode = Passing::Manual> LLU::Image>`
- :cpp:class:`TypedNumericArray\<T> <template\<typename T> LLU::TypedNumericArray>` - type-aware interface to :cpp:class:`NumericArray <template\<typename T, class PassingMode = Passing::Manual> LLU::NumericArray>`
- :cpp:class:`TypedTensor\<T> <template\<typename T> LLU::TypedTensor>` - type-aware interface to :cpp:class:`Tensor <template\<typename T, class PassingMode = Passing::Manual> LLU::Tensor>`

These template classes offer iterators and data access functions for each container. They shouldn't be used directly,
as they don't hold any data. Instead, use containers from level 2 which inherits from level 1B containers.

Level 2
----------------------------------

- :cpp:class:`DataList\<T, PassingMode> <template\<MArgumentType T, class PassingMode = Passing::Manual> LLU::DataList>`
- :cpp:class:`Image\<T, PassingMode> <template\<typename T, class PassingMode = Passing::Manual> LLU::Image>`
- :cpp:class:`NumericArray\<T, PassingMode> <template\<typename T, class PassingMode = Passing::Manual> LLU::NumericArray>`
- :cpp:class:`Tensor\<T, PassingMode> <template\<typename T, class PassingMode = Passing::Manual> LLU::Tensor>`

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

All of the above are described in the `LibraryLink documentation <https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#97446640>`_.

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


Examples
========================
<to be added>

API Reference
========================

Typed Containers
------------------------

DataList
~~~~~~~~~~~~~~~~~~~~~~~~
.. doxygenclass:: LLU::DataList
   :members:

Image
~~~~~~~~~~~~~~~~~~~~~~~~
.. doxygenclass:: LLU::Image
   :members:

NumericArray
~~~~~~~~~~~~~~~~~~~~~~~~
.. doxygenclass:: LLU::NumericArray
   :members:

Tensor
~~~~~~~~~~~~~~~~~~~~~~~~
.. doxygenclass:: LLU::Tensor
   :members:

Generic Containers
------------------------

GenericDataList
~~~~~~~~~~~~~~~~~~~~~~~~
.. doxygentypedef:: LLU::GenericDataList

.. doxygenclass:: LLU::MContainer< MArgumentType::DataStore, PassingMode >
   :members:

GenericImage
~~~~~~~~~~~~~~~~~~~~~~~~
.. doxygentypedef:: LLU::GenericImage

.. doxygenclass:: LLU::MContainer< MArgumentType::Image, PassingMode >
   :members:

GenericNumericArray
~~~~~~~~~~~~~~~~~~~~~~~~
.. doxygentypedef:: LLU::GenericNumericArray

.. doxygenclass:: LLU::MContainer< MArgumentType::NumericArray, PassingMode >
   :members:

GenericTensor
~~~~~~~~~~~~~~~~~~~~~~~~
.. doxygentypedef:: LLU::GenericTensor

.. doxygenclass:: LLU::MContainer< MArgumentType::Tensor, PassingMode >
   :members: