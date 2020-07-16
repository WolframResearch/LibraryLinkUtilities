.. include:: ../globals.rst

==============================================
How to use
==============================================

:abbr:`LLU (LibraryLink Utilities)` is designed to be built from sources as a static library and included in other projects.
We do not provide prebuilt binaries for any platform.

0. Prerequisites
==============================================

Since the source code uses C++17 features, you have to make sure your compiler supports C++17. For the three most popular compilers this roughly means:

 * **Visual Studio** >= 15.7
 * **gcc** >= 7.0
 * **clang** >= 5

Plus:

 * **CMake** >= 3.14
 * **Mathematica** >= 12.0, or more specifically

   - **WSTP** interface version 4 or later
   - **WolframLibrary** >= 5

WSTP library and WolframLibrary header files can be found in any Mathematica installation.
Optionally, for running unit tests, **wolframscript** must be available on the system.

1. Get source code
=========================================

You can clone LLU from GitHub:

**TODO: here goes the GitHub clone link**

Alternatively, a zip package can be downloaded from GitHub containing a snapshot from any branch.

2. Configure
=========================================

LLU depends on WSTP and WolframLibrary so both must be installed on your system.
Below is a quick overview of CMake variables which you can use to customize build process. Let's consider a number of possible scenarios:

1. Use WSTP and WolframLibrary from a standard Mathematica installation:

   If you have Mathematica **12.0** or later installed in a default location or on the system PATH, the build configuration step should succeed out of the box
   without setting any variables.
   Otherwise, set ``Mathematica_INSTALL_DIR`` to an absolute path to Mathematica installation directory, for instance

   .. code-block:: console

      cmake -DMathematica_INSTALL_DIR=/home/jerome/Mathematica/12.1 ..

3. Use WSTP and WolframLibrary from arbitrary locations (rare case)

   If WSTP and WolframLibrary are not located in a Mathematica installation, two paths must be passed to CMake:

   .. code-block:: console

      cmake -DWOLFRAM_LIBRARY_PATH=/path/to/WolframLibrary -DWOLFRAM_WSTP_PATH=/my/own/WSTP/installation ..


Other useful cmake variables used by LLU include:

 - ``BUILD_SHARED_LIBS`` - Whether to build LLU as shared library. A static library is created by default and it is the recommended choice.
 - ``CMAKE_BUILD_TYPE`` - Choose the type of build. This should match the type of build of your project.
 - ``CMAKE_INSTALL_PREFIX`` - Where to install LLU. The default location is the :file:`install/` directory in the source tree.
 - ``CMAKE_VERBOSE_MAKEFILE`` - Useful for debugging.

3. Build, Install and Test
=========================================

After successful configuration you are just one :code:`make && make install` away from the end.

When you have the library installed you may want to run unit tests to confirm that everything went well. Currently there are 14 test modules defined:

- Async
- DataList
- ErrorReporting
- GenericContainers
- Image
- ManagedExpressions
- MArgumentManager
- NumericArray
- ProgressMonitor
- Scalar
- String
- Tensor
- Utilities
- WSTP

You can run all of them (except for ProgressMonitor tests which are contained in a Mathematica notebook and excluded from batch testing) with
a :program:`ctest` command or by running the ``test`` CMake target. It is possible to run a specific test module, for example

.. code-block:: console

	ctest -R WSTP

``test`` target actually calls :code:`wolframscript` under the hood, so it must be installed in your system. This also means that the ``test`` target will not show
individual test failures.

To improve unit test feedback, another CMake target called :code:`TestWithOutputOnFailure` is defined. Running this target (the exact command depends on the
generator used):

.. code-block:: console

	make TestWithOutputOnFailure

will show the whole output produced by ctest and wolframscript. There is still room for improvement in this area and suggestions are welcome.

.. warning::
	Tests will only work after the LLU library has been installed.

4. Add to your project
=========================================

CMake configuration
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

LLU defines CMake export target and hides the build details. Dependencies, compiler flags, include paths, etc do not need to be set.
After LLU is installed, in your project's CMakeLists.txt call:

.. code-block:: cmake

   set(LLU_ROOT /path/to/LLU/installation/dir)

   find_package(LLU NO_MODULE PATH_SUFFIXES LLU)

and later

.. code-block:: cmake

   target_link_libraries(MyTarget PRIVATE LLU::LLU)

The last step is to copy the file with Wolfram Language code to use the top-level features of LLU, for example:

.. code-block:: cmake

   install(FILES "${LLU_ROOT}/share/LibraryLinkUtilities.wl"
     DESTINATION "${PACLET_NAME}/LibraryResources"
   )

Initialization
~~~~~~~~~~~~~~~~~~~~~~~~~~~

LLU, being a wrapper over LibraryLink, lives at the edge of two worlds: the C++ domain and the Wolfram Language. Both parts of LLU need to be initialized.
On the C++ side the initialization looks like this:

.. code-block:: cpp

	#include <LLU/LLU.h>

	EXTERN_C DLLEXPORT int WolframLibrary_initialize(WolframLibraryData libData) {
	       LibraryData::setLibraryData(libData);
	}

``WolframLibrary_initialize`` will be automatically called when you load your LibraryLink paclet into the Wolfram Language, for instance with ``Needs["MyPaclet`"]``.
:cpp:func:`setLibraryData<LibraryData::setLibraryData>` is called to initialize the globally accessible instance of ``WolframLibraryData``, that LLU uses, with
the instance that was passed to the initialization function. Later on, you can call ``LibraryData::API()`` to access this instance from anywhere in the code.
See the documentation of :cpp:class:`LLU::LibraryData` for details.

Initialization of the WL part is equally simple - imagine that we have paclet called *MyPaclet* and its shared library is named :file:`MyPacletLib.so` (the extension
is platform-dependent):

.. code-block:: wolfram-language

   Get["/path/to/LibraryLinkUtilities.wl"];

   `LLU`InitializePacletLibrary["MyPacletLib"];

:file:`LibraryLinkUtilities.wl` is part of LLU sources and it should be copied to every paclet that uses LLU. As soon as you call :wlref:`Get` on it, it will inject
LLU symbols to the current context, which should typically be ``MyPaclet`Private``.

```LLU`InitializePacletLibrary`` takes the path to the paclet library (or just the name, if the library can be located with :wlref:`FindLibrary`) and loads it
into the WolframKernel process. It also loads WSTP library (if not already loaded) and initializes internal structures. LLU will store the path to the paclet
library ("MyPacletLib" in the example above) so that later when you load library functions you do not need to pass the library path every time.

Another task commonly done right after initializing LLU is error registering. See the :doc:`../modules/error_handling` chapter for detailed description
of this process.

5. Example - demo project
=========================================

All of the above can be seen in action in the demo project that is shipped with LLU in the :file:`tests/Demo` directory. The Demo project is a complete
Wolfram Language :term:`paclet` and it can be built and used as follows:

1. Install LLU as described above. Let's say you chose :file:`/my/workspace/LLU` as the install directory.
2. Navigate to :file:`tests/Demo` in the LLU source directory.
3. Run the following commands (or equivalent for your system):

.. code-block:: console

	cmake -DLLU_ROOT=/my/workspace/LLU -DMathematica_ROOT=/path/to/Mathematica/ -B build
	cd build/
	cmake --build . --target install

This will put a complete paclet directory structure under :file:`tests/Demo/build/Demo`. You can copy this directory into :file:`SystemFiles/Links` subdirectory
of Mathematica installation and then load the paclet by calling ``Needs["Demo`"]`` in a notebook.

Optionally, you can build another target called *paclet*"

.. code-block:: console

	cmake --build . --target paclet

When built, *paclet* target will take the directory structure created by the *install* target and turn it into a proper **.paclet** file.
It can optionally validate paclet contents, run a test file or install paclet to a directory where Mathematica can automatically find it.
Investigate the :file:`tests/Demo/CMakeLists.txt` file for details on how to create and use this target.

Finally, after building the *paclet* target or manually copying the Demo paclet into :file:`SystemFiles/Links`, you should be able to run the following code
in a notebook:

.. code-block:: none

	In[1]:= Needs["Demo`"]

	In[2]:= Demo`CaesarCipherEncode["HelloWorld", 5]

	Out[2]= "Mjqqtbtwqi"

	In[3]:= Demo`CaesarCipherDecode[%, 5]

	Out[3]= "HelloWorld"