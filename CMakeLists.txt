cmake_minimum_required(VERSION 3.21)
project(MythicalDangers LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# Build SFML 2 with the same compiler used for the game.
# Do not link against the repository's old 32-bit SFML 2.5.1 binaries.
include(FetchContent)
set(BUILD_SHARED_LIBS ON CACHE BOOL "Build shared SFML libraries" FORCE)
set(SFML_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_TEST_SUITE OFF CACHE BOOL "" FORCE)
set(SFML_BUILD_NETWORK OFF CACHE BOOL "" FORCE)

FetchContent_Declare(sfml
    GIT_REPOSITORY https://github.com/SFML/SFML.git
    GIT_TAG 2.6.2
    GIT_SHALLOW TRUE
)
FetchContent_MakeAvailable(sfml)

add_executable(MythicalDangers src/main.cpp)

# Place the finished game directly next to gfx/, audio/, saves/ and icon.png.
# Keep SFML compilation and other intermediate files inside build/.
set_target_properties(MythicalDangers PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}"
)

# The repository has an old SFML 2.5.1 header tree inside include/.
# Give the newly-built SFML 2.6.2 headers precedence. The other include
# directory is still needed for the bundled nlohmann/json.hpp header.
target_include_directories(MythicalDangers BEFORE PRIVATE
    "${sfml_SOURCE_DIR}/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
target_link_libraries(MythicalDangers PRIVATE
    sfml-graphics
    sfml-window
    sfml-system
    sfml-audio
)

# Copy the matching SFML 2 DLLs next to the executable for easier launching.
if(WIN32)
    foreach(sfml_target IN ITEMS sfml-graphics sfml-window sfml-system sfml-audio)
        add_custom_command(TARGET MythicalDangers POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "$<TARGET_FILE:${sfml_target}>"
                "$<TARGET_FILE_DIR:MythicalDangers>"
            VERBATIM
        )
    endforeach()

    # SFML Audio on Windows uses OpenAL at runtime.
    if(CMAKE_SIZEOF_VOID_P EQUAL 8)
        add_custom_command(TARGET MythicalDangers POST_BUILD
            COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                "${sfml_SOURCE_DIR}/extlibs/bin/x64/openal32.dll"
                "$<TARGET_FILE_DIR:MythicalDangers>"
            VERBATIM
        )
    endif()
endif()

# Copy the MinGW compiler runtime DLLs alongside the game so it can also be
# started by double-clicking in Explorer (without an MSYS2 terminal on PATH).
# This is intended for a normal MSYS2 UCRT64 installation, where g++ and the
# runtime DLLs are together in the UCRT64/bin folder.
if(WIN32 AND MINGW)
    get_filename_component(_compiler_bin_dir "${CMAKE_CXX_COMPILER}" DIRECTORY)
    foreach(_runtime_dll IN ITEMS
        libgcc_s_seh-1.dll
        libstdc++-6.dll
        libwinpthread-1.dll
    )
        if(EXISTS "${_compiler_bin_dir}/${_runtime_dll}")
            add_custom_command(TARGET MythicalDangers POST_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                    "${_compiler_bin_dir}/${_runtime_dll}"
                    "$<TARGET_FILE_DIR:MythicalDangers>"
                VERBATIM
            )
        else()
            message(WARNING "MinGW runtime not found beside compiler: ${_runtime_dll}. The executable may require this DLL on PATH.")
        endif()
    endforeach()
endif()
