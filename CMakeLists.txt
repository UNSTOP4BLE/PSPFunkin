cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD           11)
set(CMAKE_CXX_STANDARD         17)

project(PSPFunkin LANGUAGES C CXX)

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
    add_custom_command(
        OUTPUT ${_out}
        COMMAND ${_chtbin} ${_out} ${PROJECT_SOURCE_DIR}/${_in}
        DEPENDS ${PROJECT_SOURCE_DIR}/${_in}
        COMMENT "Building chart ${_out}"
    )
endforeach()

#file(COPY ${_built_charts} DESTINATION ${PROJECT_BINARY_DIR}/assets)

add_executable(${PROJECT_NAME} ${MY_SRCS})

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
    create_pbp_file(
        TARGET ${PROJECT_NAME}
        ICON_PATH ${PROJECT_SOURCE_DIR}/ICON0.PNG
        BACKGROUND_PATH ${PROJECT_SOURCE_DIR}/PIC1.PNG  
        PREVIEW_PATH NULL
        TITLE ${PROJECT_NAME}
    )
else()
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
