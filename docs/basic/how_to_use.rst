==============================================
How to use
==============================================

:abbr:`LLU (LibraryLink Utilities)` is designed to be built from sources as a static library and included in other projects.
We do not provide prebuilt binaries for any platform.

0. Prerequisites
==============================================

Since the source code uses C++14 features, you have to make sure your compiler supports C++14. This means:

 * **Visual Studio** >= 15.0
 * **gcc** >= 5.0
 * **clang** >= 3.4

Plus:
 * **CMake** >= 3.8.0
 * **MathLink** interface version 4 or later
 * **WolframLibrary** >= 5

MathLink library and WolframLibrary header files can be found in any Mathematica installation. For testing, ``wolframscript`` must be available on the system.

1. Get source code
=========================================

You can clone LLU from GitHub:

ssh://git@stash.wolfram.com:7999/imex/librarylinkutilities.git **TODO: replace with GitHub clone link**

Alternatively, a zip package can be downloaded from GitHub containing a snapshot from any chosen branch.

2. Configure
=========================================

Dependencies
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LLU depends on MathLink and WolframLibrary so both must be installed on your system.
Below is a quick overview of CMake variables which you can use to customize build process. Let's consider a number of possible scenarios:

1. Use MathLink and WolframLibrary from a standard Mathematica installation:

   If you have Mathematica **12.1** installed in a default location, the build configuration step should succeed out of the box without setting any variables.
   If you have other versions of Mathematica installed in a default location you only need to specify ``MATHEMATICA_VERSION``, for example:

   .. code-block:: bash

      cmake -DMATHEMATICA_VERSION=12.0 ..

2. Use MathLink and WolframLibrary from a non-standard Mathematica installation:

   .. code-block:: bash

      cmake -DMATHEMATICA_INSTALL_DIR=/home/jerome/path/to/Mathematica ..

3. Use MathLink and WolframLibrary from arbitrary locations

   If MathLink and WolframLibrary are not located in a Mathematica installation, two paths must be passed to CMake:

   .. code-block:: bash

      cmake -DWOLFRAM_LIBRARY_PATH=/path/to/WolframLibrary -DWOLFRAM_MATHLINK_PATH=/my/own/MathLink/installation ..

Other Options
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Other useful cmake variables used by LLU include:

 - ``BUILD_SHARED_LIBS`` - Whether to build LLU as shared library. A static library is created by default.
 - ``CMAKE_BUILD_TYPE`` - Choose the type of build. This should match the type of build of your project.
 - ``CMAKE_INSTALL_PREFIX`` - Where to install LLU. The default location is the :file:`install/` directory in the source tree.
 - ``CMAKE_VERBOSE_MAKEFILE`` - Useful for debugging.

3. Build, Install and Test
=========================================

After successful configuration you are just one :code:`make && make install` away from the end.

When you have the library installed you may want to run unit tests to confirm that everything went well. Currently there are 10 test modules defined:

- DataList
- ErrorReporting
- GenericContainers
- Image
- MathLink
- NumericArray
- ProgressMonitor
- Scalar
- String
- Tensor

You can run all of them with :code:`make test` or :code:`ctest`. It is possible to run a specific test module, for example

.. code-block:: bash

	ctest -R MathLink

Test targets actually call :code:`wolframscript` under the hood, it must be installed in your system. This means that :code:`make test` will not show individual test failures.

To improve the test feedback, another CMake target called :code:`TestWithOutputOnFailure` is defined. Running:

.. code-block:: bash

	make TestWithOutputOnFailure

will show the whole output produced by ctest and wolframscript. There is still room for improvement in this area and suggestions are welcome.

4. Link from your project
=========================================

LLU defines CMake export target and hides the build details. Dependencies, compiler flags, include paths, etc do not need to be set.
After LLU is installed, in your project's CMakeLists.txt call:

.. code-block:: cmake

   find_package(LLU)

and later

.. code-block:: cmake

   target_link_libraries(MyTarget PRIVATE LLU::LLU)

The last step is to copy the file with Wolfram Language code to use the top-level features of LLU, for example:

.. code-block:: cmake

   install(FILES "${LLU_LOCATION}/share/LibraryLinkUtilities.wl"
     DESTINATION "${PACLET_NAME}/LibraryResources"
   )
