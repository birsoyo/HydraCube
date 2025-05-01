cmake_minimum_required(VERSION 3.30)
set(CMAKE_POLICY_DEFAULT_CMP0077 NEW) # option() honors normal variables. (For third party libraries).
set(CMAKE_NINJA_FORCE_RESPONSE_FILE 1 CACHE INTERNAL "")

project(HydraCube
    VERSION 0.1.0
)

set(CMAKE_C_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
add_compile_definitions(
    NOMINMAX                                                           # std::max used in pxr headers
    _SILENCE_CXX20_OLD_SHARED_PTR_ATOMIC_SUPPORT_DEPRECATION_WARNING   # pxr is using deprecated std::atomic_* overloads std::shared_ptr (C++20)
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # Generate compile_commands.json to make it easier to work with clang based tools
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")
set(CMAKE_OBJECT_PATH_MAX 1024) # Maximum object file full-path length allowed by native build tools.
set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(GLOBAL PROPERTY PREDEFINED_TARGETS_FOLDER "_")

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/artifacts/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/artifacts/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/artifacts/bin)
set(CMAKE_COMPILE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/artifacts/lib)
set(CMAKE_PDB_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/artifacts/bin)

find_package(pxr REQUIRED)
find_package(OpenGL REQUIRED)

add_subdirectory(Code)
