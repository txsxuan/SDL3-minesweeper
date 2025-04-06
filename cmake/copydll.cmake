macro(CopyDLL target_name)
    if(WIN32)
        add_custom_command(
            TARGET ${target_name} POST_BULD COMMAND $(CMAKE_COMMAND) -E copy $CACHE{SDL3_DLIB_DIR}/SDL3.dll $<TARGET_FILE_DIR:${target_name}>
        )
endmacro()
