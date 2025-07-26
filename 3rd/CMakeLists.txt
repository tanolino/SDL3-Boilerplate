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

message(INFO " Fetching SDL3 (this might take some time)")
FetchContent_Declare(
    sdl
    GIT_REPOSITORY https://github.com/libsdl-org/SDL
    GIT_TAG        68bfcb6c5419f51104e74e72ea0f8d405a4615b0 # release 3.2.18
    GIT_PROGRESS TRUE
    EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(sdl)

### STB

set(STB_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/STB" CACHE PATH "" FORCE)

### NATE

add_subdirectory(NATE)

### Font

set(FONT_INCLUDE "${CMAKE_CURRENT_SOURCE_DIR}/Font" CACHE PATH "" FORCE)

