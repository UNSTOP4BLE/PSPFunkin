cmake_minimum_required(VERSION 3.20)

set(CMAKE_C_STANDARD           11)
set(CMAKE_CXX_STANDARD         17)

project(PSPFunkin LANGUAGES C CXX)

file(GLOB MY_SRCS src/*.cpp src/*/*.cpp)

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

#copy over the assets folder
file(COPY ${PROJECT_SOURCE_DIR}/assets/ DESTINATION ${PROJECT_BINARY_DIR}/assets)