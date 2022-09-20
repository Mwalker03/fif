# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("fif")

# Include sub-projects.
#add_subdirectory ("src")
#add_dependencies (fif "src\argsparser")

if(UNIX)
    add_executable (fif2 "src/main.cc"
    "src/argsparser/argsparser.h"	 
    "src/argsparser/argsparser.cc"
    "src/strings.h"			
    "src/os/os.h"		    "src/os/os.cc"
    "src/colors/colors.h"	"src/colors/colors.cc"	
    "src/configuration.h"	"src/configuration.cc"
    "src/utilities.h"		"src/utilities.cc"
    "src/fif.h"             "src/fif.cc")
endif(UNIX)

if(WIN32)
    add_executable(fif	"src/main.cc"
    "src/argsparser/argsparser.h"	 
    "src/argsparser/argsparser.cc"
    "src/strings.h"			
    "src/os/os.h"		"src/os/os.cc"		"src/os/dirent.h"
    "src/colors/colors.h"	"src/colors/colors.cc"	
    "src/configuration.h"	"src/configuration.cc"
    "src/utilities.h"		"src/utilities.cc"
    "src/fif.h"             "src/fif.cc")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif(WIN32)
