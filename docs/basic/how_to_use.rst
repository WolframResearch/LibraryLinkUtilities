==============================================
How to use
==============================================

:abbr:`LLU (LibraryLink Utilities)` is designed to be built from sources as a static library and included in other projects.
We do not provide prebuilt binaries for any platform.

0. Prerequisites
==============================================

Since the source code uses C++17 features, you have to make sure your compiler supports C++17. This roughly means:

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

ssh://git@stash.wolfram.com:7999/imex/librarylinkutilities.git **TODO: replace with GitHub clone link**

Alternatively, a zip package can be downloaded from GitHub containing a snapshot from any branch.

2. Configure
=========================================

LLU depends on WSTP and WolframLibrary so both must be installed on your system.
Below is a quick overview of CMake variables which you can use to customize build process. Let's consider a number of possible scenarios:

1. Use WSTP and WolframLibrary from a standard Mathematica installation:

   If you have Mathematica **12.0** or later installed in a default location or on the system PATH, the build configuration step should succeed out of the box
   without setting any variables.
   Otherwise, set ``Mathematica_INSTALL_DIR`` to an absolute path to Mathematica installation directory:

   .. code-block:: console

      cmake -DMathematica_INSTALL_DIR=/home/jerome/Mathematica/12.1 ..

3. Use WSTP and WolframLibrary from arbitrary locations

   If WSTP and WolframLibrary are not located in a Mathematica installation, two paths must be passed to CMake:

   .. code-block:: console

      cmake -DWOLFRAM_LIBRARY_PATH=/path/to/WolframLibrary -DWOLFRAM_WSTP_PATH=/my/own/WSTP/installation ..


Other useful cmake variables used by LLU include:

 - ``BUILD_SHARED_LIBS`` - Whether to build LLU as shared library. A static library is created by default.
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

4. Link from your project
=========================================

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

5. Example - demo project
=========================================

All of the above can be seen in action in the demo project that is shipped with LLU in the :file:`tests/Demo` directory. The Demo project is a complete
Wolfram Language paclet (to learn more about WL paclets and their development see `this guide <https://www.wolframcloud.com/obj/tgayley/Published/PacletDevelopment.nb>`_)
and it can be built and used as follows:

1. Install LLU as described above. Let's say you chose /my/workspace/LLU as the install directory.
2. Navigate to :file:`tests/Demo` in the LLU source directory.
3. Run the following commands (or equivalent for your system):

.. code-block:: console

	cmake -B build
	cd build/
