cmake_minimum_required(VERSION 3.20)
project(minesweeper LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ===== SDL3 配置 =====

# 用户可以在 cmake 时通过 -DSDL3_DIR=xxx 指定路径
set(SDL3_DIR "" CACHE PATH "Path to SDL3 install (for Windows manual build)")
set(SDL3_IMAGE_DIR "" CACHE PATH "Path to SDL3_image install (for Windows manual build)")

# 设置依赖的 DLL 子目录（仅 Windows 下使用）
if(WIN32)
    set(SDL3_DLL_DIR "${SDL3_DIR}/x86_64-w64-mingw32/bin" CACHE PATH "Path to SDL3.dll")
    set(SDL3_IMAGE_DLL_DIR "${SDL3_IMAGE_DIR}/x86_64-w64-mingw32/bin" CACHE PATH "Path to SDL3_image.dll")
    include(${SDL3_DIR}"/cmake/SDL3Config.cmake")
    include(${SDL3_IMAGE_DIR}"/cmake/SDL3_imageConfig.cmake")
else()
  find_package(SDL3 CONFIG)
  if (NOT SDL3_FOUND)
      message(FATAL_ERROR "SDL3 not found. Please set CMAKE_PREFIX_PATH or install SDL3 properly.")
  endif()
  find_package(SDL3_image CONFIG)
  if (NOT SDL3_image_FOUND)
      message(FATAL_ERROR "SDL3_image not found. Please set CMAKE_PREFIX_PATH or install SDL3_image properly.")
  endif()
endif()
# ===== 源码结构 =====
aux_source_directory(src SRC)
add_executable(minesweeper ${SRC})

target_include_directories(minesweeper PRIVATE include)
target_precompile_headers(minesweeper PRIVATE include/pch.hpp)

target_link_libraries(minesweeper PRIVATE SDL3::SDL3 SDL3_image::SDL3_image)

# ===== Windows: 自动复制 DLL =====
function(copy_dll TARGET DLL_PATH)
    if(WIN32)
        add_custom_command(
            TARGET ${TARGET} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${DLL_PATH}" $<TARGET_FILE_DIR:${TARGET}>
            COMMENT "Copying ${DLL_PATH} to build directory"
        )
    endif()
endfunction()

if(WIN32)
    copy_dll(minesweeper "${SDL3_DLL_DIR}/SDL3.dll")
    copy_dll(minesweeper "${SDL3_IMAGE_DLL_DIR}/SDL3_image.dll")
endif()

# ===== compile_commands.json 同步到源码根目录 =====
if(EXISTS "${CMAKE_BINARY_DIR}/compile_commands.json")
    file(COPY "${CMAKE_BINARY_DIR}/compile_commands.json"
         DESTINATION "${CMAKE_SOURCE_DIR}"
         FILES_MATCHING PATTERN "compile_commands.json")
endif()

# 每次构建后，把生成的可执行文件复制到项目根目录
add_custom_command(
    TARGET minesweeper POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:minesweeper>
            "${CMAKE_SOURCE_DIR}/minesweeper"
    COMMENT "Copying executable to project root"
)

