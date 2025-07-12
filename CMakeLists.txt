cmake_minimum_required(VERSION 3.16)

if (OFF)

if(MSVC)
add_compile_options(/fsanitize=address)
add_link_options(/fsanitize=address)
elseif(XCode)
set(CMAKE_XCODE_GENERATE_SCHEME ON)
set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER ON)
set(CMAKE_XCODE_SCHEME_ADDRESS_SANITIZER_USE_AFTER_RETURN ON)
else()
add_compile_options(-fsanitize=address)
add_link_options(-fsanitize=address)
endif()

endif()

set(CMAKE_CONFIGURATION_TYPES "Release;RelWithDebInfo")
project(SDL3_in_3D
    LANGUAGES C
    # CXX
    VERSION "0.0.1")

add_subdirectory(3rd)
add_subdirectory(src)