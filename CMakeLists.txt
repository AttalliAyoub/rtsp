cmake_minimum_required(VERSION 3.14)
project(rtsp C CXX)
set (PROJECT_NAME rtsp)
set ( ${PROJECT_NAME}_HOME ${CMAKE_CURRENT_SOURCE_DIR} )
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})
if ( NOT DEFINED BUILD_SHARED_LIBS )
    set ( BUILD_SHARED_LIBS FALSE )
endif ()
if ( CMAKE_COMPILER_IS_GNUCXX )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive" )
endif ()

option(USE_CXX14_STD "Product should be build with C++11 compiler option enabled" ON)

if(USE_CXX11_STD)
    set(CMAKE_CXX_STANDARD 14)
    # set(CMAKE_CXX_STANDARD 11)
endif()

if(MSVC)
	add_definitions(-D_CONSOLE)
else()
    # GCC or Clang
endif()

# cmake .. -DCMAKE_TOOLCHAIN_FILE=vcpkg/scripts/buildsystems/vcpkg.cmake
# cmake .. -DCMAKE_TOOLCHAIN_FILE=C:/cpp/vcpkg/scripts/buildsystems/vcpkg.cmake

find_package(PkgConfig REQUIRED)
add_definitions(-DGL_SILENCE_DEPRECATION)

find_package(OpenGL REQUIRED)
find_package(glfw3  REQUIRED)
find_package(FFmpeg REQUIRED)

find_path(AVCODEC_INCLUDE_DIR libavcodec/avcodec.h)
find_library(AVCODEC_LIBRARY avcodec)

find_path(AVFORMAT_INCLUDE_DIR libavformat/avformat.h)
find_library(AVFORMAT_LIBRARY avformat)

set(CXX_SRC_EXT_PATTERN ".cpp")
set(C_SRC_EXT_PATTERN ".c")
set(C_HDR_EXT_PATTERN ".h")
set(C_XX_HDR_EXT_PATTERN ".hpp")

file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*${CXX_SRC_EXT_PATTERN} ${PROJECT_SOURCE_DIR}/src/*${C_SRC_EXT_PATTERN})
file(GLOB HDR_FILES ${PROJECT_SOURCE_DIR}/src/*${C_HDR_EXT_PATTERN} ${PROJECT_SOURCE_DIR}/src/*${C_XX_HDR_EXT_PATTERN})

source_group("Source Files" FILES ${SRC_FILES})
source_group("Header Files" FILES ${HDR_FILES})

add_executable(rtsp ${SRC_FILES} ${HDR_FILES})

# include_directories(${SYSTEM_INCLUDE_DIR})

target_include_directories(rtsp 
	PRIVATE
        ${OPENGL_INCLUDE_DIRS} ${GLFW3_INCLUDE_PATH}
        ${FFMPEG_INCLUDE_DIRS}
            ${AVCODEC_INCLUDE_DIR}
            ${AVFORMAT_INCLUDE_DIR}
)


if(APPLE)
    list(APPEND EXTRA_LIBS
        "-framework OpenGL"
    )

    configure_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/assets/MacOSXBundleInfo.plist.in
        ${CMAKE_CURRENT_BINARY_DIR}/assets/MacOSXBundleInfo.plist
    )

    set_target_properties(rtsp PROPERTIES
        MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_BINARY_DIR}/assets/MacOSXBundleInfo.plist
    )

elseif(WIN32)
    list(APPEND EXTRA_LIBS
        "-lglu32 -lopengl32"
    )
    set(CMAKE_EXE_LINKER_FLAGS "-std=gnu99 -static -static-libgcc -static-libstdc++ -mwindows")
else()
    list(APPEND EXTRA_LIBS
        "-lGL -lGLU -lX11"
    )
endif()

target_link_libraries(rtsp 
    PRIVATE
        ${OPENGL_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${FFMPEG_LIBRARIES}
            ${AVCODEC_LIBRARY}
            ${AVFORMAT_LIBRARY}
        # ${SWSCALE_LIBRARIES}
        # ${AVDEVICE_LIBRARIES}
        # ${AVFILTER_LIBRARIES}
        ${EXTRA_LIBS}
)

