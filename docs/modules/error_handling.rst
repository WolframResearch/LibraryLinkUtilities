.. include:: ../globals.rst

================
Error handling
================

Every *LibraryLink* function in C code has a fixed signature [#]_

.. code-block:: cpp

   int f (WolframLibraryData libData, mint Argc, MArgument* Args, MArgument Res);

The actual result of computations should be returned via the "out-parameter" ``Res``. The value of ``Res`` is only considered in the Wolfram Language code if the actual
return value of ``f`` (of type ``int``) was equal to ``LIBRARY_NO_ERROR`` (with LLU use ``ErrorCode::NoError``, see note below).

.. tip::
   *LibraryLink* uses 8 predefined error codes

   .. code-block:: cpp

      enum {
          LIBRARY_NO_ERROR = 0,
          LIBRARY_TYPE_ERROR,
          LIBRARY_RANK_ERROR,
          LIBRARY_DIMENSION_ERROR,
          LIBRARY_NUMERICAL_ERROR,
          LIBRARY_MEMORY_ERROR,
          LIBRARY_FUNCTION_ERROR,
          LIBRARY_VERSION_ERROR
      };

   *LLU* redefines those values as constexpr integers in a dedicated namespace :cpp:any:`LLU::ErrorCode`, so for example instead of
   ``LIBRARY_FUNCTION_ERROR`` one can use :cpp:any:`ErrorCode::FunctionError`.


That means, that the **only information** about an error which occurred in the library that makes it to the Wolfram Language code is a **single integer**.

In C++ exceptions are the preferred way of error handling, so *LLU* offers a special class of exceptions that can be easily translated to error codes,
returned to *LibraryLink* and then translated to descriptive :wl:`Failure` objects in Wolfram Language.

Such exceptions are identified in the C++ code by name - a short string. For example, imagine you have a function that reads data from a source.
If the source does not exist or is empty, you want to throw exceptions, let's call them "NoSourceError" and "EmptySourceError", respectively.
First, you **must** register all your exceptions inside ``WolframLibrary_initialize`` function:

.. code-block:: cpp
   :linenos:

   EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
       try {
           ErrorManager::registerPacletErrors({
               {"NoSourceError", "Requested data source does not exist."},
               {"EmptySourceError", "Requested data source has `1` elements, but required at least `2`."}
           });
       } catch(...) {
           return LLErrorCode::FunctionError;
       }
       return LLErrorCode::NoError;
   }

In the code above, the second element of each pair is a textual description of the error which will be visible in the
`Failure <https://reference.wolfram.com/language/ref/Failure.html>`_ object.
This text may contain "slots" denoted as \`1\`, \`2\`, etc. that work like `TemplateSlots <https://reference.wolfram.com/language/ref/TemplateSlot.html>`_
in the Wolfram Language.

.. note::
   Notice that there is no way to assign specific error codes to your custom exceptions, this is handled internally by *LLU*.

Now, in the function that reads data:

.. code-block:: cpp
   :linenos:

   void readData(std::unique_ptr<DataSource> source) {
       if (!source) {
           ErrorManager::throwException("NoSourceError");
       }
       if (source->elemCount() < 3) {
           ErrorManager::throwException("EmptySourceError", source->elemCount(), 3);
       }
       //...
   }

Each call to :cpp:func:`ErrorManager::throwException<LLU::ErrorManager::throwException>` causes an exception of class :cpp:class:`LibraryLinkError<LLU::LibraryLinkError>`
with predefined name and error code to be thrown.
All parameters to :cpp:func:`throwException<LLU::ErrorManager::throwException>` call after the first one are used to populate consecutive template slots in the error message.
The only thing left do now is to catch the exception.
Usually, you will catch only in the interface functions (the ones with ``EXTERN_C DLLEXPORT``), extract the error code from exception and return it:

.. code-block:: cpp
   :linenos:

   EXTERN_C DLLEXPORT int MyFunction(WolframLibraryData libData, mint Argc, MArgument *Args, MArgument Res) {
       auto err = ErrorCode::NoError;    // no error initially
       try {
           //...
       } catch (const LibraryLinkError& e) {
           err = e.which();    // extract error code from LibraryLinkError
       } catch (...) {
           err = ErrorCode::FunctionError;   // to be safe, handle non-LLU exceptions as well and return generic error code
       }
       return err;
   }

The Wolfram Language part of the error-handling functionality of LLU is responsible for converting error codes returned by library functions
into nice and informative :wl:`Failure` objects.

.. rubric:: Footnotes
.. [#] One more possible signature is ``int f(WolframLibraryData, MLINK)``. For such functions error handling is done in the same way.

Examples
================


API reference
================

.. doxygenclass:: LLU::LibraryLinkError
   :members:

.. doxygennamespace:: LLU::ErrorCode

.. doxygenclass:: LLU::ErrorManager
   :members: