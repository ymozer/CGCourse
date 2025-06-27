function(add_chapter CHAPTER_NAME)

    file(GLOB CHAPTER_SOURCES CONFIGURE_DEPENDS 
        "${CMAKE_CURRENT_SOURCE_DIR}/${CHAPTER_NAME}/*.cpp"
    )

    if(PLATFORM_IS_ANDROID)
        # For Android, we must build a SHARED library named "main".
        # The SDL Java activity will load libmain.so.
        add_library(main SHARED ${CHAPTER_SOURCES})
        set_target_properties(main PROPERTIES DEBUG_POSTFIX "")
        target_link_libraries(main PRIVATE base)
        set_target_properties(main PROPERTIES
            FOLDER "Course Projects/Chapters (Android)"
        )
    else()
        add_executable(${CHAPTER_NAME} ${CHAPTER_SOURCES})
        target_link_libraries(${CHAPTER_NAME} PRIVATE base)

        set_target_properties(${CHAPTER_NAME} PROPERTIES
            FOLDER "Course Projects/Chapters"
            VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        )

        add_custom_command(
            TARGET ${CHAPTER_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_directory_if_different
                    ${CMAKE_SOURCE_DIR}/assets
                    $<TARGET_FILE_DIR:${CHAPTER_NAME}>/assets
            COMMENT "Copying assets to build directory for ${CHAPTER_NAME}"
        )
    endif()
endfunction()


if(PLATFORM_IS_ANDROID)
    message(STATUS "Configuring Android build for chapter: ${CHAPTER_TO_BUILD_FOR_ANDROID}")
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${CHAPTER_TO_BUILD_FOR_ANDROID}")
        add_chapter(${CHAPTER_TO_BUILD_FOR_ANDROID})
    else()
        message(FATAL_ERROR "Chapter directory not found for Android target: ${CHAPTER_TO_BUILD_FOR_ANDROID}")
    endif()
else()
    file(GLOB CHAPTER_SUBDIRS LIST_DIRECTORIES true "${CMAKE_CURRENT_SOURCE_DIR}/*")

    foreach(CHAPTER_DIR ${CHAPTER_SUBDIRS})
        if(IS_DIRECTORY ${CHAPTER_DIR})
            get_filename_component(CHAPTER_NAME ${CHAPTER_DIR} NAME)
            if(EXISTS "${CHAPTER_DIR}/main.cpp")
                add_chapter(${CHAPTER_NAME})
            endif()
        endif()
    endforeach()
endif()