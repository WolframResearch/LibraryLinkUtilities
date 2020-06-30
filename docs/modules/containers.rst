=================================================
Containers
=================================================

Overview
============================

Raw LibraryLink containers like MTensor or MNumericArray store their element type as a regular field in the stucture.
This means that the type cannot be used at compile-time, which makes writing generic code that does something with
the underlying data very difficult (lots of switches on the element type and code repetition).

On the other hand, having the element type as template parameter, like STL containers, is often inconvenient and requires
some template magic for simple things like passing forward the container or reading metadata when the data type is not
known a priori.

To get the best of both worlds at cost of a bit more complicated interface, LLU provides the following hierarchy of container
classes:

Level 0
----------------------------------

* DataStore
* MImage
* MNumericArray
* MTensor

These are just raw LibraryLink containers. LLU is likely unnecessary when working with raw LibraryLink containers. However, they are still supported.

Level 1
----------------------------------

* :cpp:type:`LLU::GenericDataList`
* :cpp:type:`LLU::GenericImage`
* :cpp:type:`LLU::GenericNumericArray`
* :cpp:type:`LLU::GenericTensor`

These are datatype-unaware wrappers that offer automatic memory management and basic interface like access to metadata (dimensions, rank, etc).
They do not provide direct access to the underlying data except via a ``void*``, which should be used with caution.

Level 2
----------------------------------

- :cpp:class:`DataList\<T> <template\<MArgumentType T> LLU::DataList>`
- :cpp:class:`Image\<T> <template\<typename T> LLU::Image>`
- :cpp:class:`NumericArray\<T> <template\<typename T> LLU::NumericArray>`
- :cpp:class:`Tensor\<T> <template\<typename T> LLU::Tensor>`

Full-fledged wrappers with automatic memory management (see section below), type-safe data access, iterators, etc.


Memory management
============================

When passing a container from Wolfram Language to a C++ library, one of 4 passing modes must be chosen:

* Automatic
* Constant
* Manual
* Shared

With the exception of DataStore, which cannot be Shared.

All of the above are described in the `LibraryLink documentation <https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#97446640>`_.

In plain LibraryLink, the choice you make is reflected only in the Wolfram Language code where LibraryFunctionLoad specifies
the list of parameters for the library function. There is no way to query the WolframLibraryData or MArgument about
the passing modes of function arguments from within C++ code. Therefore, the programmer must remember the passing mode
for each argument and then ensure the correct action is taken (releasing/not releasing memory depending
on the combination of passing mode and whether the container has been returned from the library function to the Wolfram Language).
This design is far from perfect because manual resource management often leads to bugs and leaks.

LLU defines a notion of *container ownership*:

.. doxygenenum:: LLU::Ownership

LLU ensures that at any point of time every container has a well-defined owner. The ownership is mostly static and may change only on a few occasions e.g.
when passing a container to DataList or setting it as a result of a library function.

When a container is receiced from the Wolfram Language as an argument to a library function, the develop must inform the :cpp:class:`MArgumentManager<LLU::MArgumentManager>`
about the passing mode used for that container. There is a separate enumeration for this purpose:

.. doxygenenum:: LLU::Passing

The ``Passing`` value is used by the :cpp:class:`MArgumentManager<LLU::MArgumentManager>` to determine the initial owner of the container.

Here are some examples:

.. code-block:: cpp
   :dedent: 1

    LLU::Tensor<mint> t { 1, 2, 3, 4, 5 };    // this Tensor is created (and therefore owned) by the library (LLU)

    LLU::MArgumentManager manager {...};
    auto tensor = manager.getTensor<double>(0);  // tensors acquired via MArgumentManager are by default owned by the LibraryLink

    auto image = manager.getGenericImage<LLU::Passing::Shared>(0);    // the image is shared between LLU and the Kernel, so LLU knows not to deallocate
                                                                      // the underlying MImage when image goes out of scope

    auto newImage = image.clone();    // the newImage has the same contents as image but it is not shared, it is owned by LLU


More examples can be found in unit tests.

Views
========================




The following table summarizes current status of LibraryLink containers and their LLU wrappers:

+---------------------+--------------------------+--------------------+--------------------+-----------------------+
|    Raw Container    |    Generic wrapper       |   Typed wrapper    |        View        |        TypedView      |
+=====================+==========================+====================+====================+=======================+
|       MTensor       |    GenericTensor         |      Tensor<T>     |     TensorView     |     TensorTypedView   |
+---------------------+--------------------------+--------------------+--------------------+-----------------------+
|    MNumericArray    | GenericNumericArray      |   NumericArray<T>  |  NumericArrayView  | NumericArrayTypedView |
+---------------------+--------------------------+--------------------+--------------------+-----------------------+
|       MImage        |    GenericImage          |       Image<T>     |      ImageView     |     ImageTypedView    |
+---------------------+--------------------------+--------------------+--------------------+-----------------------+
|      DataStore      |   GenericDataList        |      DataList<T>   |                    |                       |
+---------------------+--------------------------+--------------------+--------------------+-----------------------+

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

.. doxygenclass:: LLU::MContainer< MArgumentType::DataStore >
   :members:

GenericImage
~~~~~~~~~~~~~~~~~~~~~~~~
.. doxygentypedef:: LLU::GenericImage

.. doxygenclass:: LLU::MContainer< MArgumentType::Image >
   :members:

GenericNumericArray
~~~~~~~~~~~~~~~~~~~~~~~~
.. doxygentypedef:: LLU::GenericNumericArray

.. doxygenclass:: LLU::MContainer< MArgumentType::NumericArray >
   :members:

GenericTensor
~~~~~~~~~~~~~~~~~~~~~~~~
.. doxygentypedef:: LLU::GenericTensor

.. doxygenclass:: LLU::MContainer< MArgumentType::Tensor >
   :members:
