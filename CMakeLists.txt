cmake_minimum_required(VERSION 3.23)
project(
    my-shmup-01
    VERSION 1.0
    DESCRIPTION "Shmup-01 SDL2"
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/$<CONFIGURATION>")

file(GLOB SOURCES
    src/defs.h src/structs.h
    src/*.c src/*.h
    src/*/*.c src/*/*.h
)

add_executable(${PROJECT_NAME} ${SOURCES})

if(MINGW)
    target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall
        -Wextra
        -Wempty-body
        -Werror
        -Warray-bounds
        -g
        -Wstrict-prototypes
        -Werror=uninitialized
    )
elseif(MSVC)
    target_compile_options(${PROJECT_NAME} PRIVATE
        /W4
        /WX
        /Zi
    )
endif()

# ── Platform linking ──────────────────────────────────────────────────────────
if(MINGW)
    set(SDL2_ROOT       "C:/sdl/SDL2-2.32.2/x86_64-w64-mingw32")
    set(SDL2_MIXER_ROOT "C:/sdl/SDL2_mixer-2.8.0/x86_64-w64-mingw32")
    set(SDL2_IMAGE_ROOT "C:/sdl/SDL2_image-2.8.12/x86_64-w64-mingw32")
    set(SDL2_TTF_ROOT   "C:/sdl/SDL2_ttf-2.24.0/x86_64-w64-mingw32")

    target_include_directories(${PROJECT_NAME} PRIVATE
        "${SDL2_ROOT}/include/SDL2"
        "${SDL2_MIXER_ROOT}/include/SDL2"
        "${SDL2_IMAGE_ROOT}/include/SDL2"
        "${SDL2_TTF_ROOT}/include/SDL2"
    )

    target_link_libraries(${PROJECT_NAME} PRIVATE
        mingw32
        "${SDL2_ROOT}/lib/libSDL2main.a"
        "${SDL2_ROOT}/lib/libSDL2.dll.a"
        "${SDL2_IMAGE_ROOT}/lib/libSDL2_image.dll.a"
        "${SDL2_MIXER_ROOT}/lib/libSDL2_mixer.dll.a"
        "${SDL2_TTF_ROOT}/lib/libSDL2_ttf.dll.a"
    )
    target_link_options(${PROJECT_NAME} PRIVATE -mwindows)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${SDL2_ROOT}/bin/SDL2.dll"
            "${SDL2_IMAGE_ROOT}/bin/SDL2_image.dll"
            "${SDL2_MIXER_ROOT}/bin/SDL2_mixer.dll"
            "${SDL2_TTF_ROOT}/bin/SDL2_ttf.dll"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
elseif(APPLE)
    execute_process(
        COMMAND brew --prefix
        OUTPUT_VARIABLE BREW_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    set(ENV{PKG_CONFIG_PATH} "${BREW_PREFIX}/lib/pkgconfig:$ENV{PKG_CONFIG_PATH}")

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)

    # SDL2_image/mixer/ttf have broken transitive deps on macOS (zlib, libpng, etc.)
    # so we skip pkg-config for them and link dylibs directly
    target_include_directories(${PROJECT_NAME} PRIVATE
        "${BREW_PREFIX}/include/SDL2"
    )
    target_link_libraries(${PROJECT_NAME} PRIVATE
        ${SDL2_LIBRARIES}
        "${BREW_PREFIX}/lib/libSDL2_image.dylib"
        "${BREW_PREFIX}/lib/libSDL2_mixer.dylib"
        "${BREW_PREFIX}/lib/libSDL2_ttf.dylib"
    )
    target_link_directories(${PROJECT_NAME} PRIVATE
        "${BREW_PREFIX}/lib"
    )
    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wempty-body -Werror -Warray-bounds -g
        $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes -Werror=uninitialized>
    )
else()
    # Linux
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)

    target_include_directories(${PROJECT_NAME} PRIVATE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(${PROJECT_NAME} PRIVATE
        SDL2::SDL2
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
    )

    target_compile_options(${PROJECT_NAME} PRIVATE
        -Wall -Wempty-body -Werror -Warray-bounds -g
        $<$<COMPILE_LANGUAGE:C>:-Wstrict-prototypes -Werror=uninitialized>
    )
endif()

# ── Install / RPATH ───────────────────────────────────────────────────────────
if(APPLE)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH "@executable_path;@loader_path"
        BUILD_WITH_INSTALL_RPATH TRUE
        MACOSX_RPATH TRUE
    )
elseif(UNIX)
    set_target_properties(${PROJECT_NAME} PROPERTIES
        INSTALL_RPATH "$ORIGIN"
        BUILD_WITH_INSTALL_RPATH TRUE
    )
endif()

install(TARGETS ${PROJECT_NAME} RUNTIME DESTINATION bin)
