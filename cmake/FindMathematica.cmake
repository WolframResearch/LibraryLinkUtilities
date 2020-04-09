# FindMathematica.cmake
#
# Finds an installation of Mathematica. By "installation of Mathematica" we mean a directory DIR such that:
#
# - DIR/.VersionID is a text file containing a version number of the form X.Y.Z.A
# - DIR/Executables/Mathematica is an executable file
# - DIR/Executables/WolframKernel is an executable file
#
# This module will define the following variables
#
#    Mathematica_FOUND
#    Mathematica_VERSION
#    Mathematica_INSTALL_DIR
#    Mathematica_EXE
#    Mathematica_KERNEL_EXE
#
# You can specify custom location to search for Wolfram Library either by specifying WOLFRAM_LIBRARY_PATH explicitly,
# or if that variable is not set, by providing MATHEMATICA_INSTALL_DIR variable with a path to Mathematica installation.
#
# Author: Rafal Chojna - rafalc@wolfram.com

function(parse_mathematica_version M_DIRECTORY VERSION)
	
endfunction()