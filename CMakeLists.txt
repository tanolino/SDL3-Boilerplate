cmake_minimum_required(VERSION 3.16)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


if(MSVC)
set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo")
else()
set(CMAKE_CONFIGURATION_TYPES "Release;Debug")
endif()

# ASAN or not ASAN
if (ON)

if(MSVC)
add_compile_options(/fsanitize=address)
add_link_options(/fsanitize=address)
elseif(XCode)
set(CMAKE_XCODE_GENERATE_SCHEME ON)
set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER ON)
set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER_USE_AFTER_RETURN ON)
else()
add_compile_options(-g -fsanitize=address -fsanitize=pointer-subtract -fsanitize=leak -O0 -fno-omit-frame-pointer)
add_link_options(-g -fsanitize=address -fsanitize=pointer-subtract -fsanitize=leak -O0 -fno-omit-frame-pointer)
endif()

endif()

project(SDL3_Project
    LANGUAGES C
    # CXX
    VERSION "0.0.1")

add_subdirectory(3rd)
add_subdirectory(src)
