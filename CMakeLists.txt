cmake_minimum_required(VERSION 3.20)

project(
    PSPFunkin
    VERSION      "1.0.0"
    DESCRIPTION  "PSPFunkin Rewrite"
    LANGUAGES    C CXX
    HOMEPAGE_URL "https://github.com/UNSTOP4BLE/PSPFunkin"
)

# Manually list source files
set(SRC_FILES
    src/main.cpp
    src/trans.cpp
    src/object.cpp
    src/engine/audio_file_readers.cpp
    src/engine/audio_mixer.cpp
    src/engine/audio_streamed_file.cpp
    src/engine/file.cpp
    src/engine/scene.cpp
    src/engine/animation.cpp
    src/engine/timer.cpp
    src/engine/input.cpp
    src/engine/assets.cpp
    #game scenes
    src/scenes/playstate.cpp
    src/scenes/title.cpp
    src/scenes/mainmenu.cpp
    src/scenes/freeplay.cpp
    src/scenes/test.cpp
)

include(FetchContent)

find_package(PkgConfig REQUIRED)

# Detect Homebrew prefix on macOS
if(APPLE)
    execute_process(
        COMMAND brew --prefix
        OUTPUT_VARIABLE HOMEBREW_PREFIX
        OUTPUT_STRIP_TRAILING_WHITESPACE
    )
    message(STATUS "Homebrew prefix detected: ${HOMEBREW_PREFIX}")

    include_directories("${HOMEBREW_PREFIX}/include")
    link_directories("${HOMEBREW_PREFIX}/lib")
endif()

pkg_check_modules(VORBIS REQUIRED vorbis vorbisfile ogg)
find_package(SDL2 REQUIRED)
if (!PSP)
    find_package(JsonCpp REQUIRED)
    pkg_check_modules(SDL2IMAGE REQUIRED sdl2_image)
endif()

# Manually list source files
if (PSP)
    set(PSP_SRCS
        src/engine/psp/glib2d.cpp
        src/engine/psp/rendererpsp.cpp
    )

    set(PSPLIBS
        pspvfpu
        m
        z
        jpeg
        png
    )
else()

    set(PC_SRCS
        src/engine/pc/renderersdl.cpp
    )

    set(PCLIBS
    )
endif()

add_executable(main ${SRC_FILES} ${PSP_SRCS} ${PC_SRCS})

target_compile_features(main PRIVATE c_std_11 cxx_std_17)

target_include_directories(main PRIVATE
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_IMAGE_INCLUDE_DIRS}
    ${VORBIS_INCLUDE_DIRS}
#    ${JsonCpp_INCLUDE_DIRS}
)

target_link_libraries(main PRIVATE
    ${SDL2_LIBRARIES}
    -lSDL2_image
    ${VORBIS_LIBRARIES}
    jsoncpp
 #   JsonCpp::JsonCpp
    ${PCLIBS}
    ${PSPLIBS}
)

if(PSP)
    create_pbp_file(
        TARGET          main
        ICON_PATH       "${PROJECT_SOURCE_DIR}/ICON0.PNG"
        BACKGROUND_PATH "${PROJECT_SOURCE_DIR}/PIC1.PNG"
        PREVIEW_PATH    NULL
        TITLE           PSPFunkin
    )
endif()