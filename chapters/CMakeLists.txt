function(add_chapter CHAPTER_PATH)
    get_filename_component(CHAPTER_NAME ${CHAPTER_PATH} NAME)
    message(STATUS "Adding chapter: ${CHAPTER_NAME}")
    message(STATUS "Chapter sources: ${CHAPTER_SOURCES}")
    message(STATUS "Chapter path: ${CHAPTER_PATH}")

    file(GLOB CHAPTER_SOURCES CONFIGURE_DEPENDS 
        "${CHAPTER_PATH}/*.cpp"
    )
    
    if(PLATFORM_IS_ANDROID)
        add_library(main SHARED ${CHAPTER_SOURCES})
        set_target_properties(main PROPERTIES DEBUG_POSTFIX "")
        target_link_libraries(main PRIVATE base)
        set_target_properties(main PROPERTIES
            FOLDER "Course Projects/Chapters (Android)"
        )
    else()
        # Standalone builds (Desktop, Web) require a main.cpp entry point.
        if(NOT EXISTS "${CHAPTER_PATH}/main.cpp")
            message(WARNING "Skipping standalone build for ${CHAPTER_NAME}: missing main.cpp")
            return()
        endif()
        
        add_executable(${CHAPTER_NAME} ${CHAPTER_SOURCES})
        target_link_libraries(${CHAPTER_NAME} PRIVATE base)
        add_dependencies(${CHAPTER_NAME} CopyAssets)
        
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

        install(TARGETS ${CHAPTER_NAME}
            BUNDLE DESTINATION .
            RUNTIME DESTINATION bin 
            COMPONENT "Applications"
            LIBRARY DESTINATION lib
            ARCHIVE DESTINATION lib
        )
    endif()
endfunction()