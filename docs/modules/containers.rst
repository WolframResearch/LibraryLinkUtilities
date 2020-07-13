=================================================
Containers
=================================================


Raw LibraryLink containers like MTensor or MNumericArray store their element type as a regular field in the structure.
This means that the type cannot be used at compile-time, which makes writing generic code that does something with
the underlying data very difficult (lots of switches on the element type and code repetition).

On the other hand, having the element type as template parameter, like STL containers, is often inconvenient and requires
some template magic for simple things like passing forward the container or reading metadata when the data type is not
known a priori.

To get the best of both worlds and to make the library suitable for different needs, LLU provides two categories of container wrappers -
generic, datatype-agnostic wrappers and full-fledged wrappers templated with the datatype. This is illustrated in the table below:

+----------------------------------------------+----------------------------------------------+----------------------------------------+
| LibraryLink element                          |    Generic wrapper                           |   Typed wrapper                        |
+==============================================+==============================================+========================================+
|    :ref:`MTensor <mtensor-label>`            | :ref:`GenericTensor <generictensor-label>`   | :ref:`Tensor\<T> <tensor-label>`       |
+----------------------------------------------+----------------------------------------------+----------------------------------------+
| :ref:`MNumericArray <mnumericarray-label>`   | :ref:`GenericNumericArray <genericna-label>` | :ref:`NumericArray\<T> <numarr-label>` |
+----------------------------------------------+----------------------------------------------+----------------------------------------+
|    :ref:`MImage <mimage-label>`              | :ref:`GenericImage <genericimg-label>`       | :ref:`Image\<T> <image-label>`         |
+----------------------------------------------+----------------------------------------------+----------------------------------------+
|    :ref:`DataStore <datastore-label>`        | :ref:`GenericDataList <genericdl-label>`     | :ref:`DataList\<T> <datalist-label>`   |
+----------------------------------------------+----------------------------------------------+----------------------------------------+
|    :ref:`MSparseArray <msparsearray-label>`  |                      ⊝                       |          ⊝                             |
+----------------------------------------------+----------------------------------------------+----------------------------------------+

Memory management
============================

When passing a container from Wolfram Language to a C++ library, one of 4 passing modes must be chosen:

* Automatic
* Constant
* Manual
* Shared

With the exception of DataStore, which cannot be Shared.

All of the above is described in the `LibraryLink documentation <https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#97446640>`_.

In plain LibraryLink, the choice you make is reflected only in the Wolfram Language code where :wlref:`LibraryFunctionLoad` specifies
the list of parameters for the library function. There is no way to query the WolframLibraryData or MArgument about
the passing modes of function arguments from within C++ code. Therefore, the programmer must remember the passing mode
for each argument and then ensure the correct action is taken (releasing/not releasing memory depending
on the combination of passing mode and whether the container has been returned from the library function to the Wolfram Language).

LLU defines a notion of *container ownership*:

.. doxygenenum:: LLU::Ownership

LLU ensures that at any point of time every container has a well-defined owner. The ownership is mostly static and may change only on a few occasions e.g.
when passing a container to DataList or setting it as a result of a library function.

When a container is received from the Wolfram Language as an argument to a library function, the developer must inform the :cpp:class:`MArgumentManager<LLU::MArgumentManager>`
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

RawContainers
============================

These are just raw LibraryLink containers.

.. _datastore-label:

DataStore
----------------------------

``DataStore`` is C structure (technically, a pointer to structure) defined in the WolframLibrary. It is a unidirectional linked list of non-mutable nodes.
Each node consists of a *name* (``char*``) and *value* (``MArgument``). DataStore itself is a member of the MArgument union, which means that DataStores
can be nested. DataStores can be passed to and from library functions. Existing nodes cannot be removed but adding new nodes is supported.

The complete DataStore API can be found in :file:`SystemFiles/IncludeFiles/C/WolframIOLibraryFunctions.h` inside Mathematica installation (12.0+).

On the Wolfram Language side a ``DataStore`` is represented as an expression with head ``Developer`DataStore`` that takes a list of expressions, where each
expressions is either:

 - a value of type supported by LibraryLink (String, Integer, NumericArray, etc.)
 - a :wlref:`Rule` with the LHS being a String and RHS of the form described in the previous point

For example:

.. code-block:: wolfram-language

   Developer`DataStore["node_name1" -> 42, NumericArray[{1,2,3,4}, "Integer8"], "node_name3" -> "node_value3"]


.. _mimage-label:

MImage
----------------------------

A structure corresponding to Wolfram Language expressions :wlref:`Image` and :wlref:`Image3D`.
Documented in `LibraryLink » MImage <https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#441025439>`_.

.. _mnumericarray-label:

MNumericArray
----------------------------

A structure corresponding to Wolfram Language expressions :wlref:`NumericArray`.
Documented in `LibraryLink » MNumericArray <https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#106266186>`_.

.. _mtensor-label:

MTensor
----------------------------

A structure corresponding to packed arrays in the Wolfram Language.
Documented in `LibraryLink » MTensor <https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#280210622>`_.

.. _msparsearray-label:

MSparseArray
----------------------------

A structure corresponding to Wolfram Language expressions :wlref:`SparseArray`.
Documented in `LibraryLink » MSparseArray <https://reference.wolfram.com/language/LibraryLink/tutorial/InteractionWithWolframLanguage.html#1324196729>`_.

Generic Wrappers
======================================

These are datatype-unaware wrappers that offer automatic memory management and basic interface like access to metadata (dimensions, rank, etc).
They do not provide direct access to the underlying data except via a :cpp:expr:`void*` (or via a generic node type :cpp:any:`LLU::NodeType::Any` in case of a
GenericDataList).

.. note::

   All generic and strongly-typed wrappers are movable but non-copyable, instead they provide a :cpp:expr:`clone()` method for performing deep copies.
   This is in accordance with rule `C.67 <http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#c67-a-polymorphic-class-should-suppress-copying>`_
   from the C++ Core Guidelines but most of all preventing accidental deep copies of containers is beneficial in terms of performance.

.. _genericdl-label:

:cpp:type:`LLU::GenericDataList`
------------------------------------

GenericDataList is a light-weight wrapper over :ref:`datastore-label`. It offers access to the underlying nodes via iterators and a
:cpp:func:`push_back <LLU::MContainer\< MArgumentType::DataStore >::push_back>` method for appending new nodes. You can also get the length of the list.



Here is an example of GenericDataList in action:

.. code-block:: cpp
   :linenos:

   /* Reverse each string in a list of strings using GenericDataList */
   LIBRARY_LINK_FUNCTION(ReverseStrings) {
      LLU::MArgumentManager mngr {libData, Argc, Args, Res};

      // read the input GenericDataList
      auto dsIn = mngr.get<LLU::GenericDataList>(0);

      // create new GenericDataList to store reversed strings
      LLU::GenericDataList dsOut;

      for (auto node : dsIn) {
         // GenericDataList may store nodes of arbitrary type, so we need to explicitly ask to get the string value from the node
         std::string_view s = node.as<LLU::NodeType::UTF8String>();

         std::string reversed {s.rbegin(), s.rend()};	// create reversed copy

         // we push back the reversed string via a string_view, this is safe because GenericDataList will immediately copy the string
         dsOut.push_back(std::string_view(reversed));
      }

      // set the GenericDataList as the result of the library function
      mngr.set(dsOut);
   }

Technically, GenericDataList is an alias:

.. doxygentypedef:: LLU::GenericDataList

.. doxygenclass:: LLU::MContainer< MArgumentType::DataStore >
   :members:

.. _genericimg-label:

:cpp:type:`LLU::GenericImage`
------------------------------------

Here is an example of GenericImage in action:

.. code-block:: cpp
   :linenos:

   /* Get the number of columns in the input Image */
   LIBRARY_LINK_FUNCTION(GetColumnCount) {
      LLU::MArgumentManager mngr {libData, Argc, Args, Res};
      const auto image = mngr.getGenericImage<LLU::Passing::Constant>(0);
      mngr.setInteger(image.columns());
   }

.. doxygentypedef:: LLU::GenericImage

.. doxygenclass:: LLU::MContainer< MArgumentType::Image >
   :members:

.. _genericna-label:

:cpp:type:`LLU::GenericNumericArray`
------------------------------------

Here is an example of GenericNumericArray in action:

.. code-block:: cpp
   :linenos:

   /* Return the largest dimension of the input NumericArray */
   LIBRARY_LINK_FUNCTION(GetLargestDimension) {
      LLU::MArgumentManager mngr {libData, Argc, Args, Res};
      const auto numericArray = mngr.getGenericNumericArray<LLU::Passing::Constant>(0);

      // The list of dimensions of the NumericArray will never be empty because scalar NumericArrays are forbidden
      auto maxDim = *std::max_element(numericArray.getDimensions(), std::next(numericArray.getDimensions(), numericArray.getRank()));
      mngr.setInteger(maxDim);
   }

.. doxygentypedef:: LLU::GenericNumericArray

.. doxygenclass:: LLU::MContainer< MArgumentType::NumericArray >
   :members:

.. _generictensor-label:

:cpp:type:`LLU::GenericTensor`
------------------------------------

Here is an example of GenericTensor in action:

.. code-block:: cpp
   :linenos:

.. doxygentypedef:: LLU::GenericTensor

.. doxygenclass:: LLU::MContainer< MArgumentType::Tensor >
   :members:

Typed Wrappers
============================

Full-fledged wrappers with automatic memory management (see section below), type-safe data access, iterators, etc.
All typed wrappers are movable but non-copyable, instead they provide a :cpp:expr:`clone()` method for performing deep copies.

.. _datalist-label:

:cpp:class:`LLU::DataList\<T> <template\<MArgumentType T> LLU::DataList>`
-------------------------------------------------------------------------------

Here is an example of DataList class in action:

.. code-block:: cpp
   :linenos:

   /* Take a list of named nodes with complex numbers and create two new lists: a list of node names and a list of node values */
   LIBRARY_LINK_FUNCTION(SeparateKeysAndValues) {
      LLU::MArgumentManager mngr {libData, Argc, Args, Res};

      auto dsIn = mngr.getDataList<LLU::NodeType::Complex>(0);
      LLU::DataList<LLU::NodeType::UTF8String> keys;
      LLU::DataList<LLU::NodeType::Complex> values;

      // For each node in the input DataList push its name to "keys" and its value to "values"
      for (auto [name, value] : dsIn) {
        keys.push_back(name);
        values.push_back(value);
      }

      LLU::DataList<LLU::GenericDataList> dsOut;
      dsOut.push_back("Keys", std::move(keys));
      dsOut.push_back("Values", std::move(values));

      mngr.set(dsOut);
   }

On the Wolfram Language side, we can load and use this function as follows:

.. code-block:: wolfram-language

   `LLU`PacletFunctionSet[SeparateKeysAndValues, "SeparateKeysAndValues", {"DataStore"}, "DataStore"];

   SeparateKeysAndValues[Developer`DataStore["a" -> 1 + 2.5 * I, "b" -> -3. - 6.I, 2I]]

   (* Out[] = Developer`DataStore["Keys" -> Developer`DataStore["a", "b", ""], "Values" -> Developer`DataStore[1. + 2.5 * I, -3. - 6.I, 2.I]] *)

.. doxygenclass:: LLU::DataList
   :members:

.. _image-label:

:cpp:class:`LLU::Image\<T> <template\<typename T> LLU::Image>`
-------------------------------------------------------------------------------

Here is an example of the Image class in action:

.. code-block:: cpp
   :linenos:

   /* Take a constant "Byte" image and return a copy with negated pixel values */
   LIBRARY_LINK_FUNCTION(NegateImage) {
      LLU::MArgumentManager mngr {libData, Argc, Args, Res};

      const auto image = mngr.getImage<uint8_t, LLU::Passing::Constant>(0);

      LLU::Image<uint8_t> outImage {image.clone()};
      constexpr uint8_t negator = (std::numeric_limits<uint8_t>::max)();
      std::transform(std::cbegin(in), std::cend(in), std::begin(outImage), [](T inElem) { return negator - inElem; });

      mngr.setImage(outImage);
   }

On the Wolfram Language side, we can load and use this function as follows:

.. code-block:: wolfram-language

   `LLU`PacletFunctionSet[NegateImage, "NegateImage", {{Image, "Constant"}}, Image];

   NegateImage[Image[RandomImage[ColorSpace -> "RGB"], "Byte"]]

   (* Out[] = [--Image--] *)

This is naturally only a toy example, Wolfram Language has a built-in function for negating images: :wlref:`ImageNegate`.

.. doxygenclass:: LLU::Image
   :members:

.. _numarr-label:

:cpp:class:`LLU::NumericArray\<T> <template\<typename T> LLU::NumericArray>`
-------------------------------------------------------------------------------

.. doxygenclass:: LLU::NumericArray
   :members:

.. _tensor-label:

:cpp:class:`LLU::Tensor\<T> <template\<typename T> LLU::Tensor>`
-------------------------------------------------------------------------------

.. doxygenclass:: LLU::Tensor
   :members:


Iterators
========================


