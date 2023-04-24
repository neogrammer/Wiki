# Copyright (c) Microsoft Corporation.
# Licensed under the MIT License.

cmake_minimum_required (VERSION 3.20)

project (wikitest
  DESCRIPTION "DirectX Tool Kit for DirectX 11 Wiki Source Tester"
  HOMEPAGE_URL "http://go.microsoft.com/fwlink/?LinkID=615561"
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/CMake")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/CMake")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin/CMake")

if (DEFINED VCPKG_TARGET_ARCHITECTURE)
    set(DIRECTX_ARCH ${VCPKG_TARGET_ARCHITECTURE})
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Ww][Ii][Nn]32$")
    set(DIRECTX_ARCH x86)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Xx]64$")
    set(DIRECTX_ARCH x64)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Aa][Rr][Mm]$")
    set(DIRECTX_ARCH arm)
elseif(CMAKE_GENERATOR_PLATFORM MATCHES "^[Aa][Rr][Mm]64$")
    set(DIRECTX_ARCH arm64)
endif()

set(TEST_TARGETS ${PROJECT_NAME} spritefontdump wavdump xwbdump)
add_executable(${PROJECT_NAME}
    wikitest.cpp
    ../Animation.cpp
    ../DebugDraw.cpp
    ../MSAAHelper.cpp
    ../RenderTexture.cpp
    ../SkyboxEffect.cpp
    ../TextConsole.cpp
    pch.h)

add_executable(spritefontdump ../spritefontdump.cpp)
add_executable(wavdump ../wavdump.cpp)
add_executable(xwbdump ../xwbdump.cpp)

target_link_libraries(wavdump PRIVATE winmm.lib)

set(BUILD_XAUDIO_WIN10 OFF CACHE BOOL "Build for XAudio 2.9" FORCE)
set(BUILD_XAUDIO_WIN8 OFF CACHE BOOL "Build for XAudio 2.8" FORCE)
set(BUILD_XAUDIO_WIN7 OFF CACHE BOOL "Build for XAudio2Redist" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/../../../DirectXTK ${CMAKE_BINARY_DIR}/bin/CMake/DirectXTK)
target_link_libraries(${PROJECT_NAME} PRIVATE DirectXTK)

target_include_directories(${PROJECT_NAME} PUBLIC ./ ../ ../../inc)
target_include_directories(spritefontdump PUBLIC ../../../DirectXTex/DirectXTex)

if (MINGW OR VCPKG_TOOLCHAIN)
    message("INFO: Using VCPKG for DirectXMath and XAudio2Redist.")
    find_package(directxmath CONFIG REQUIRED)
    find_package(xaudio2redist CONFIG REQUIRED)
    target_link_libraries(${PROJECT_NAME} PRIVATE Microsoft::DirectXMath)
    target_link_libraries(wavdump PRIVATE Microsoft::XAudio2Redist)
endif()

if(MSVC)
    foreach(t IN LISTS TEST_TARGETS)
      target_compile_options(${t} PRIVATE /Wall /GR- /fp:fast "$<$<NOT:$<CONFIG:DEBUG>>:/guard:cf>")
      target_link_options(${t} PRIVATE /DYNAMICBASE /NXCOMPAT)
    endforeach()

    if((CMAKE_SIZEOF_VOID_P EQUAL 4) AND (NOT ${DIRECTX_ARCH} MATCHES "^arm"))
        foreach(t IN LISTS TEST_TARGETS)
          target_link_options(${t} PRIVATE /SAFESEH)
        endforeach()
    endif()
endif()

if(NOT ${DIRECTX_ARCH} MATCHES "^arm")
    if (CMAKE_SIZEOF_VOID_P EQUAL 4)
        set(ARCH_SSE2 $<$<CXX_COMPILER_ID:MSVC>:/arch:SSE2> $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-msse2>)
    else()
        set(ARCH_SSE2 $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-msse2>)
    endif()

    foreach(t IN LISTS TEST_TARGETS)
      target_compile_options(${t} PRIVATE ${ARCH_SSE2})
    endforeach()
endif()

if ( CMAKE_CXX_COMPILER_ID MATCHES "Clang" )
    set(WarningsEXE "-Wall" "-Wpedantic" "-Wextra" "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic" "-Wno-gnu-anonymous-struct" "-Wno-language-extension-token" "-Wno-missing-prototypes" "-Wno-missing-variable-declarations" "-Wno-nested-anon-types")
    foreach(t IN LISTS TEST_TARGETS)
      target_compile_options(${t} PRIVATE ${WarningsEXE})
    endforeach()
endif()
if(MINGW)
    set(MINGW_TARGETS ${TEST_TARGETS})
    list(REMOVE_ITEM MINGW_TARGETS ${PROJECT_NAME})
    foreach(t IN LISTS MINGW_TARGETS)
      target_link_options(${t} PRIVATE -municode)
    endforeach()
endif()
if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
    foreach(t IN LISTS TEST_TARGETS)
      target_compile_options(${t} PRIVATE /sdl /permissive- /Zc:__cplusplus)
    endforeach()

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.24)
        foreach(t IN LISTS TEST_TARGETS)
          target_compile_options(${t} PRIVATE /ZH:SHA_256)
        endforeach()
    endif()

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.26)
        foreach(t IN LISTS TEST_TARGETS)
          target_compile_options(${t} PRIVATE /Zc:preprocessor /wd5105)
        endforeach()
    endif()

    if ((CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.27) AND (NOT (${DIRECTX_ARCH} MATCHES "^arm")))
        foreach(t IN LISTS TEST_TARGETS)
          target_link_options(${t} PRIVATE /CETCOMPAT)
        endforeach()
    endif()

    set(WarningsEXE "/wd4061" "/wd4365" "/wd4514" "/wd4571" "/wd4668" "/wd4710" "/wd4820" "/wd5031" "/wd5032" "/wd5039" "/wd5045")

    if (CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 19.34)
        list(APPEND WarningsEXE "/wd5262" "/wd5264")
    endif()

    foreach(t IN LISTS TEST_TARGETS)
      target_compile_options(${t} PRIVATE ${WarningsEXE})
    endforeach()
endif()

if(WIN32)
    foreach(t IN LISTS TEST_TARGETS)
      target_compile_definitions(${t} PRIVATE _UNICODE UNICODE _WIN32_WINNT=0x0602)
    endforeach()
endif()
