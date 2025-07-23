cmake_minimum_required(VERSION 3.16)

include(FetchContent)
set(BUILD_SHARED_LIBS OFF)

### SDL 3

# Deactivate Subsystems
set(SDL_DIALOG OFF) # We don't need a File Open Dialog yet
#set(SDL_GPU OFF) # Not yet
set(SDL_HAPTIC OFF) # Not yet
set(SDL_POWER OFF)
set(SDL_SENSOR OFF)
set(SDL_CAMERA OFF)

# Deactivate Details
set(SDL_AVX512F OFF) # No one has that
set(SDL_VULKAN OFF) # OpenGL / DirectX is sufficient
set(SDL_DIRECTX OFF) # OpenGL / Vulkan is sufficient
#set(SDL_OPENGLES OFF) # We only need that for android or web
set(SDL_TEST_LIBRARY OFF) # Not needed
#set(SDL_LIBC OFF) # now we can't use Asan
set(SDL_DISKAUDIO OFF)
#deactivate Dummy
set(SDL_DUMMYAUDIO OFF)
set(SDL_DUMMYCAMERA OFF)
set(SDL_DUMMYVIDEO OFF)

FetchContent_Declare(
    sdl
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG        c9a6709bd21750f1ad9597be21abace78c6378c9 # release 3.2.16
    GIT_PROGRESS TRUE
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(sdl)

### STB

set(STB_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/STB" CACHE PATH "" FORCE)
