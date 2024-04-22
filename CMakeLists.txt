cmake_minimum_required(VERSION 3.20)

project(
    PSPFunkin
    VERSION      "1.0.0"
    DESCRIPTION  "pspfunki"
    LANGUAGES    C CXX
    HOMEPAGE_URL "https://github.com/UNSTOP4BLE/PSPFunkin"
)

if(DEFINED VITA)
    include("${VITASDK}/share/vita.cmake" REQUIRED)
endif()

## Settings

set(
    PSPFUNKIN_DISPLAY_NAME "PSPFunkin"
    CACHE STRING           "Game display name"
)
set(
    PSPFUNKIN_GAME_ID "PSPF69420"
    CACHE STRING      "Game ID in XXXXnnnnn format (currently only used in Vita build)"
)

## Chart and asset conversion

find_program(
    PSPFUNKIN_CHT2BIN "cht2bin" REQUIRED
    DOC   "Path to compiled cht2bin executable (from cht2bin subdirectory)"
    HINTS "${PROJECT_SOURCE_DIR}/cht2bin/build"
)

file(
    COPY        "${PROJECT_SOURCE_DIR}/assets/"
    DESTINATION "${PROJECT_BINARY_DIR}/assets" NO_SOURCE_PERMISSIONS
    PATTERN     "SOURCE_FILES" EXCLUDE
    PATTERN     "pbp" EXCLUDE
    PATTERN     "vpk" EXCLUDE
)
file(
    GLOB_RECURSE charts
    RELATIVE     "${PROJECT_SOURCE_DIR}"
    "assets/songs/*.json"
)

foreach(input IN LISTS charts)
    cmake_path(GET input PARENT_PATH parent)
    cmake_path(GET input STEM LAST_ONLY name)

    if("${name}" STREQUAL "config") # ignore config.json
        continue()
    endif()

    set(output "${PROJECT_BINARY_DIR}/${parent}/${name}.bin")
    list(APPEND builtCharts "${output}")

    add_custom_command(
        OUTPUT  "${output}"
        COMMAND "${PSPFUNKIN_CHT2BIN}" "${PROJECT_SOURCE_DIR}/${input}" "${output}"
        DEPENDS "${PROJECT_SOURCE_DIR}/${input}"
        COMMENT "Building chart ${output}"
    )
endforeach()

add_custom_target(charts ALL DEPENDS ${builtCharts})

## Libraries

set(
    libraries
    jsoncpp
    vorbis
    vorbisfile
    ogg
)
set(
    sdlLibraries
    SDL2
    SDL2_image
)
set(
    libraryDependencies
    jpeg  # SDL2_image dependency
    png   # SDL2_image dependency
    z     # libpng dependency
)

foreach(library IN LISTS sdlLibraries)
    find_package(${library} CONFIG REQUIRED)
endforeach()

# HACK: PSPSDK's pkg-config and SDL2 CMake scripts are broken, but the toolchain
# already has all include paths set up
if(DEFINED PSP)
    list(APPEND libraries ${libraryDependencies})
else()
    find_package(PkgConfig REQUIRED)

    foreach(library IN LISTS libraries)
        pkg_check_modules(${library} REQUIRED IMPORTED_TARGET GLOBAL)
    endforeach()
endif()

## Main executable

file(
    GLOB_RECURSE sources
    RELATIVE     "${PROJECT_SOURCE_DIR}"
    "src/*.cpp"
)

add_executable(main ${sources})
target_include_directories(main PRIVATE "include")
target_compile_features(main PRIVATE c_std_11 cxx_std_17)
target_compile_options(main PRIVATE -Wall -Wextra)
target_link_libraries(main PRIVATE m ${libraries} ${sdlLibraries})

# HACK: make sure charts are built before main executable
# (and thus before PSP/Vita packages)
add_dependencies(main charts)

## PSP build

if(DEFINED PSP)
    target_link_libraries(
        main PRIVATE
        GL
        pspaudio
        pspctrl
        pspdebug
        pspdisplay
        pspge
        pspgu
        pspgum
        psphprm
        pspprof
        pspvfpu
        pspvram
        xmp
    )

    create_pbp_file(
        TARGET          main
        ICON_PATH       "${PROJECT_SOURCE_DIR}/ICON0.PNG"
        BACKGROUND_PATH "${PROJECT_SOURCE_DIR}/PIC1.PNG"
        PREVIEW_PATH    NULL
        TITLE           "${PSPFUNKIN_DISPLAY_NAME}"
    )
endif()

## Vita build

if(DEFINED VITA)
    # is this even needed?
    #if (VITA)
    #   target_include_directories(
    #       main PRIVATE
    #       "$ENV{VITASDK}/arm-vita-eabi/include/jsoncpp"
    #   )
    #endif()

    vita_create_self(${PROJECT_NAME}.self main)

    vita_create_vpk(
        ${PROJECT_NAME}.vpk "${PSPFUNKIN_GAME_ID}" ${PROJECT_NAME}.self
        VERSION "01.00" # dummy
        NAME    "${PSPFUNKIN_DISPLAY_NAME}"
        #FILE    "${PROJECT_SOURCE_DIR}/assets/vpk/icon0.png"    "sce_sys/icon0.png"
        #FILE    "${PROJECT_SOURCE_DIR}/assets/vpk/bg.png"       "sce_sys/livearea/contents/bg.png"
        #FILE    "${PROJECT_SOURCE_DIR}/assets/vpk/startup.png"  "sce_sys/livearea/contents/startup.png"
        #FILE    "${PROJECT_SOURCE_DIR}/assets/vpk/template.xml" "sce_sys/livearea/contents/template.xml"
        FILE    "${PROJECT_BINARY_DIR}/assets"                  "assets"
    )
endif()
