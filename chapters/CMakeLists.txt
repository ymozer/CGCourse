set(DEVELOPMENT_TEAM_ID "8MRFDR3542" CACHE STRING "Apple Developer Team ID for code signing")
set(BUNDLE_IDENTIFIER_PREFIX "com.adu.muh" CACHE STRING "Base bundle identifier for Apple targets")

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
        add_dependencies(${CHAPTER_NAME} CopyAssets)

        target_link_libraries(${CHAPTER_NAME} PRIVATE base)
        
        if(PLATFORM_IS_APPLE)
            set_target_properties(${CHAPTER_NAME} PROPERTIES
                MACOSX_BUNDLE_GUI_IDENTIFIER "${BUNDLE_IDENTIFIER_PREFIX}.${CHAPTER_NAME}"
                XCODE_ATTRIBUTE_DEVELOPMENT_TEAM ${DEVELOPMENT_TEAM_ID}
                XCODE_ATTRIBUTE_CODE_SIGN_STYLE "Automatic"

            )
            target_compile_definitions(${CHAPTER_NAME} PRIVATE GLES_SILENCE_DEPRECATION=1)
        endif()

        if (PLATFORM_IS_EMSCRIPTEN)
            set_target_properties(${CHAPTER_NAME} PROPERTIES SUFFIX ".html")            
            set(ASSETS_SOURCE_PATH "${CMAKE_SOURCE_DIR}/assets")
            message(STATUS "For target ${CHAPTER_NAME}: Adding linker flag --embed-file ${ASSETS_SOURCE_PATH}")
            target_link_options(${CHAPTER_NAME} PRIVATE 
                "SHELL:-s FORCE_FILESYSTEM=1"
                "SHELL:-s FULL_ES3"
                "SHELL:-s MIN_WEBGL_VERSION=2"
                "SHELL:-s ALLOW_MEMORY_GROWTH=1"
                "SHELL:-s MAX_WEBGL_VERSION=2"
                "SHELL:-s USE_SDL=3"
                "SHELL:--use-preload-plugins"
                "SHELL:--preload-file ${ASSETS_SOURCE_PATH}@/assets"
            )      
        endif()

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

        install(TARGETS ${CHAPTER_NAME}
            BUNDLE DESTINATION .
            RUNTIME DESTINATION bin 
            COMPONENT "Applications"
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
        )

        install(DIRECTORY ${CMAKE_SOURCE_DIR}/assets/
            DESTINATION bin/assets   # Installs the assets to 'bin/assets'
            COMPONENT "Applications"
        )
    endif()
endfunction()



if(PLATFORM_IS_ANDROID)
    # --- Android Mode ---
    message(STATUS "Configuring Android build for chapter: ${CHAPTER_TO_BUILD_FOR_ANDROID}")
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${CHAPTER_TO_BUILD_FOR_ANDROID}")
        add_chapter(${CHAPTER_TO_BUILD_FOR_ANDROID})
    else()
        message(FATAL_ERROR "Chapter directory not found for Android target: ${CHAPTER_TO_BUILD_FOR_ANDROID}")
    endif()

elseif(BUILD_STANDALONE_CHAPTER)
    # --- Standalone Desktop/Web Mode ---
    # Build only the one chapter specified by STANDALONE_CHAPTER_NAME.
    if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${STANDALONE_CHAPTER_NAME}")
        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${STANDALONE_CHAPTER_NAME}/main.cpp")
            add_chapter(${STANDALONE_CHAPTER_NAME})
        else()
            message(FATAL_ERROR "Standalone chapter '${STANDALONE_CHAPTER_NAME}' does not contain a main.cpp file.")
        endif()
    else()
        message(FATAL_ERROR "Standalone chapter directory not found: ${STANDALONE_CHAPTER_NAME}")
    endif()

else()
    # --- Host Application Desktop/Web Mode (Default) ---

    # NOTE: This assumes your refactored C++ files are named like "ChapterXX.cpp"
    # and live in their respective subfolders.
    file(GLOB_RECURSE CHAPTER_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*/Chapter*.cpp"
    )

    set(ALL_CHAPTER_SOURCES ${CHAPTER_SOURCES} PARENT_SCOPE)

    if(PLATFORM_IS_EMSCRIPTEN)
        message(STATUS "Emscripten detected. Building all chapters as separate targets.")
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

endif()