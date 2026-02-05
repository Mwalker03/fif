cmake_minimum_required (VERSION 3.8)
project ("fif" LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ---- Common sources
set(COMMON_SOURCES
    src/main.cc
    src/argsparser/argsparser.cc
    src/os/os.cc
    src/colors/colors.cc
    src/configuration.cc
    src/utilities.cc
    src/moduls/fif.cc
)

# ---- Linux / Unix build (with SMB)
if(UNIX AND NOT APPLE)
    set(CMAKE_BUILD_TYPE Debug)

    include_directories("../../libsmb2/include")
    link_directories("../../libsmb2/build")

    add_executable(fif
        ${COMMON_SOURCES}
        src/moduls/smb.cc
    )

    target_compile_definitions(fif PRIVATE FIF_HAVE_SMB=1)
    target_link_libraries(fif smb2)
endif()

# ---- Windows build (no SMB)
if(WIN32)
    add_executable(fif
        ${COMMON_SOURCES}
    )

    target_compile_definitions(fif PRIVATE FIF_HAVE_SMB=0)
    target_link_libraries(fif ws2_32)

    # MinGW static runtime (avoids missing libgcc/libstdc++ dlls)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static -static-libgcc -static-libstdc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()
