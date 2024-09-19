cmake_minimum_required(VERSION 3.8)
project("fif")

# Set the path to libsmb2 includes and library
include_directories("libsmb2/include")
link_directories("libsmb2/lib")
link_directories("libsmb2/build")

set(COMMON_SOURCES
    "src/main.cc"
    "src/argsparser/argsparser.h"    
    "src/argsparser/argsparser.cc"
    "src/strings.h"            
    "src/os/os.h"              "src/os/os.cc"
    "src/colors/colors.h"      "src/colors/colors.cc"    
    "src/configuration.h"      "src/configuration.cc"
    "src/utilities.h"          "src/utilities.cc"
    "src/moduls/fif.h"         "src/moduls/fif.cc"
    "src/moduls/smb.h"         "src/moduls/smb.cc"
)

if(UNIX AND NOT APPLE)
    set(CMAKE_BUILD_TYPE Debug)
    add_executable(fif ${COMMON_SOURCES})
    target_link_libraries(fif smb2)
elseif(WIN32)
    add_executable(fif ${COMMON_SOURCES} "src/os/dirent.h")
    target_link_libraries(fif ws2_32)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
endif()