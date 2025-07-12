cmake_minimum_required(VERSION 3.16)

set(GAMENAME "MyGame")

set(SRC_FILES
    main.c
)

#if(WIN32)
#    add_executable(TestSDL3 WIN32 main.c)
#else()
    add_executable(${GAMENAME} ${SRC_FILES})
#endif()

set_target_properties(${GAMENAME} PROPERTIES
    C_STANDARD 17
    C_STANDARD_REQUIRED ON
    C_EXTENSIONS OFF
    #CXX_STANDARD 20
    #CXX_STANDARD_REQUIRED ON
    #CXX_EXTENSIONS OFF
)

target_link_libraries(${GAMENAME} PRIVATE
    SDL3::SDL3
    #SDL3_image::SDL3_image
    #SDL3_ttf::SDL3_ttf
    #zlibstatic
    #MINIZIP::minizip
)

#target_include_directories(${GAMENAME} PRIVATE
#    ${STB_INCLUDE}
#)

if(NOT EMSCRIPTEN)
    install(TARGETS ${GAMENAME}
        ARCHIVE DESTINATION .
        RUNTIME DESTINATION .
        LIBRARY DESTINATION .
        RESOURCE DESTINATION .
    )
else()
    install(FILES
            "$<TARGET_FILE_DIR:${GAMENAME}>/${GAMENAME}.js"
            "$<TARGET_FILE_DIR:${GAMENAME}>/${GAMENAME}.wasm"
            DESTINATION .)
    install (FILES "index.html" DESTINATION .)
endif()