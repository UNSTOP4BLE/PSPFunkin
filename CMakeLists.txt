cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD           11)
set(CMAKE_CXX_STANDARD         17)

project(PSPFunkin LANGUAGES C CXX)

if (VITA)
include("${VITASDK}/share/vita.cmake" REQUIRED)
find_package(SDL2 REQUIRED)
set(VITA_APP_NAME "PSPFunkin Vita")
# Unique ID must be exactly 9 characters. Recommended: XXXXYYYYY where X =
# unique string of developer and Y = a unique number for this app
set(VITA_TITLEID  "VSDK00007")
# Optional version string to show in LiveArea's more info screen
set(VITA_VERSION  "01.00")
endif()

#glob all files
file(GLOB MY_SRCS src/*.cpp src/*/*.cpp)

#find the chart building program
find_program(_chtbin
    cht2bin REQUIRED
    HINTS ${PROJECT_SOURCE_DIR}/cht2bin/build
)

#copy over the assets folder
file(COPY ${PROJECT_SOURCE_DIR}/assets/ DESTINATION ${PROJECT_BINARY_DIR}/assets)

#build charts 
file(
    GLOB_RECURSE _charts
    RELATIVE ${PROJECT_SOURCE_DIR}
    assets/songs/*.json  
)
list(
    TRANSFORM _charts REPLACE "\.json$" ".bin"
    OUTPUT_VARIABLE _built_charts
)

foreach(_in _out IN ZIP_LISTS _charts _built_charts)
    if(${_in} MATCHES "config\.json$")
        continue()
    endif()
    message("BUILT CHART ${_out}")
    add_custom_command(
        OUTPUT ${_out}
        COMMAND ${_chtbin} ${PROJECT_SOURCE_DIR}/${_in} ${_out}
        DEPENDS ${PROJECT_SOURCE_DIR}/${_in}
        COMMENT "Building chart ${_out}"
    )
endforeach()
add_custom_target(charts ALL DEPENDS ${_out})

add_executable(${PROJECT_NAME} ${MY_SRCS})
target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)

if(PSP)
    target_link_libraries(${PROJECT_NAME} PRIVATE
        pspdebug
        pspdisplay
        pspge
        pspctrl
        jsoncpp 
        stdc++ 
        pspaudio 
        vorbisfile 
        vorbis 
        ogg 
        SDL2 
        SDL2_image #remove
        jpeg #remove
        png 
        z 
        pspgum 
        pspgu 
        m 
        pspvram 
        GL 
        pspvfpu 
        psphprm 
        xmp 
        pspprof
    )
    target_include_directories(${PROJECT_NAME} PRIVATE include)
    create_pbp_file(
        TARGET ${PROJECT_NAME}
        ICON_PATH ${PROJECT_SOURCE_DIR}/ICON0.PNG
        BACKGROUND_PATH ${PROJECT_SOURCE_DIR}/PIC1.PNG  
        PREVIEW_PATH NULL
        TITLE ${PROJECT_NAME}
    )
else()

    if (VITA)
        set(JSON_INCLUDE_DIR "$ENV{VITASDK}/arm-vita-eabi/include/jsoncpp")

        # Add any additional include paths here
        include_directories(
          ${JSON_INCLUDE_DIR}
        )
        target_link_libraries(${PROJECT_NAME} PRIVATE
            SDL2::SDL2
        )
    endif()

    find_package(SDL2) 
    find_package(jsoncpp) 
    
    target_link_libraries(${PROJECT_NAME} PRIVATE
        jsoncpp 
        stdc++ 
        vorbisfile 
        vorbis 
        ogg 
        SDL2 
        SDL2_image
        png 
        z 
        m  
    )
endif()

if (VITA)
## Create Vita files
vita_create_self(${PROJECT_NAME}.self ${PROJECT_NAME})
# The FILE directive lets you add additional files to the VPK, the syntax is
# FILE src_path dst_path_in_vpk. In this case, we add the LiveArea paths.
vita_create_vpk(${PROJECT_NAME}.vpk ${VITA_TITLEID} ${PROJECT_NAME}.self
  VERSION ${VITA_VERSION}
  NAME ${VITA_APP_NAME}
  FILE assets assets
)
endif()