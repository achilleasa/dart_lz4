cmake_minimum_required(VERSION 2.8.4)
project(dart_lz4)

IF(GCC)
    set(CMAKE_CXX_FLAGS "-fPIC -m64 -O3")
ENDIF(GCC)

IF(APPLE)
    set(CMAKE_CXX_FLAGS "-fPIC -m64 -O3 -dynamic -undefined suppress -flat_namespace")
ENDIF(APPLE)

IF(WIN32)
    SET(CMAKE_BUILD_TYPE Release)

    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_DEBUG lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG lib)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE lib)

    add_definitions(-DDART_SHARED_LIB)
    add_definitions(-D_USRDLL)
    add_definitions(-D_WINDLL)
    set(EXTRA_SOURCES "lib/src/dart_lz4_dllmain_win.cc")
    set(CMAKE_CXX_FLAGS "/DLL")
ENDIF(WIN32)


IF(DEFINED ENV{DART_SDK})
    include_directories( AFTER SYSTEM $ENV{DART_SDK}/include )
ENDIF(DEFINED ENV{DART_SDK})

add_library(dart_lz4 SHARED ${EXTRA_SOURCES} lib/src/dart_lz4.cc lib/src/lz4/lib/lz4.c lib/src/lz4/lib/lz4hc.c)
set_target_properties(dart_lz4 PROPERTIES LIBRARY_OUTPUT_DIRECTORY lib)
IF(WIN32)
    set_target_properties(dart_lz4 PROPERTIES LINK_FLAGS "/machine:X64")
    FIND_LIBRARY(DART_LIBRARY dart $ENV{DART_SDK}/bin)
    TARGET_LINK_LIBRARIES(dart_lz4 ${DART_LIBRARY})
ENDIF(WIN32)
