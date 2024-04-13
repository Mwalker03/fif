# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

project ("fif")

# Set the path to libsmb2 includes and library
include_directories("../../libsmb2/include")
link_directories("../../libsmb2/build")

if(UNIX)
    set(CMAKE_BUILD_TYPE Debug)
    
    add_executable (fif "src/main.cc"
    "src/argsparser/argsparser.h"	 
    "src/argsparser/argsparser.cc"
    "src/strings.h"			
    "src/os/os.h"		    "src/os/os.cc"
    "src/colors/colors.h"	"src/colors/colors.cc"	
    "src/configuration.h"	"src/configuration.cc"
    "src/utilities.h"		"src/utilities.cc"
    "src/moduls/fif.h"      "src/moduls/fif.cc"
    "src/moduls/smb.h"      "src/moduls/smb.cc")
    
    target_link_libraries(fif smb2)
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
    "src/moduls/fif.h"      "src/moduls/fif.cc")

    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif(WIN32)
