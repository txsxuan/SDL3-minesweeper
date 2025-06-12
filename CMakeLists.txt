cmake_minimum_required(VERSION 3.20)
project(minesweeper LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# This assumes the SDL source is available in vendored/SDL
add_subdirectory(3rdlibs/SDL EXCLUDE_FROM_ALL)

# This assumes the SDL_image source is available in vendored/SDL_image
add_subdirectory(3rdlibs/SDL_image EXCLUDE_FROM_ALL)
if(EMSCRIPTEN)
    message(STATUS "Target: WebAssembly")

    set(CMAKE_EXECUTABLE_SUFFIX ".html")

    # 预加载资源目录
    set(PRELOAD "--preload-file ${CMAKE_SOURCE_DIR}/resources@resources")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${PRELOAD} -sUSE_SDL=3 -sALLOW_MEMORY_GROWTH=1")

endif()



# ===== 源码结构 =====
aux_source_directory(src SRC)
add_executable(minesweeper ${SRC})

target_include_directories(minesweeper PRIVATE include)
target_precompile_headers(minesweeper PRIVATE include/pch.hpp)

target_link_libraries(minesweeper PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

# 每次构建后，把生成的可执行文件复制到项目根目录
add_custom_command(
    TARGET minesweeper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:minesweeper>
            "${CMAKE_SOURCE_DIR}/minesweeper"
    COMMENT "Copying executable to project root"
)
