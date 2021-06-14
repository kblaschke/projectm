# Generic resource builder to package any idle preset including additional textures.
#
# The macro takes two arguments:
# - idlePresetDir: the dir with preset and textures, will include the "idle_preset.cmake" file in there.
# - templateFile: The file with the function templates, will be prepended to the generated code.
# - resourceFile: The target header file to write the resources to.
#
# idle_preset.cmake should define two variables:
# - IDLE_PRESET_FILE: The filename of the preset .milk file
# - IDLE_PRESET_TEXTURES: Additional texture file to package.
#
# The header is only generated if it's missing or any source file has a newer modification date.

macro(generate_idle_preset idlePresetDir templateFile resourceFile)
    include(${idlePresetDir}/idle_preset.cmake)

    set(_isOneFileNewer YES)
    if(EXISTS ${resourceFile})
        set(_isOneFileNewer NO)

        if("${templateFile}" IS_NEWER_THAN ${resourceFile})
            set(_isOneFileNewer YES)
        endif()

        if("${idlePresetDir}/${IDLE_PRESET_FILE}" IS_NEWER_THAN ${resourceFile})
            set(_isOneFileNewer YES)
        endif()

        foreach(_texture ${IDLE_PRESET_TEXTURES})
            if("${idlePresetDir}/${_texture}" IS_NEWER_THAN ${resourceFile})
                set(_isOneFileNewer YES)
            endif()
        endforeach()
    endif()

    if(_isOneFileNewer)
        if(NOT EXISTS "${idlePresetDir}/${IDLE_PRESET_FILE}")
            message(FATAL_ERROR "Idle preset file not found: ${idlePresetDir}/${IDLE_PRESET_FILE}")
        endif()

        include(FileToCharArray)
        file_to_char_array("${idlePresetDir}/${IDLE_PRESET_FILE}" PRESET_DATA PRESET_SIZE)

        configure_file("${templateFile}" "${resourceFile}")
        file(APPEND "${resourceFile}" "using IdlePresetTextArray = std::array<char,${PRESET_SIZE}>;\n")
        file(APPEND "${resourceFile}" "static constexpr IdlePresetTextArray idlePresetText{\n${PRESET_DATA}\n};\n\n")

        list(LENGTH IDLE_PRESET_TEXTURES _textureCount)
        set(_resourceList "")
        foreach(_texture ${IDLE_PRESET_TEXTURES})
            if(NOT EXISTS "${idlePresetDir}/${_texture}")
                message(FATAL_ERROR "Idle preset texture file not found: ${idlePresetDir}/${_texture}")
            endif()

            get_filename_component(_fileBasename "${_texture}" NAME_WLE)
            string(TOUPPER "${_fileBasename}" _fileBasename)
            string(TOLOWER "${_fileBasename}" _textureName)
            string(MAKE_C_IDENTIFIER "${_fileBasename}" _fileBasename)
            file_to_char_array("${idlePresetDir}/${_texture}" ${_fileBasename}_DATA ${_fileBasename}_SIZE)

            file(APPEND "${resourceFile}" "using idlePresetTexture_${_fileBasename}Array = std::array<char,${${_fileBasename}_SIZE}>;\n")
            file(APPEND "${resourceFile}" "static constexpr idlePresetTexture_${_fileBasename}Array idlePresetTexture_${_fileBasename}{\n${PRESET_DATA}\n};\n\n")
            list(APPEND _resourceList "${_textureName}\\;idlePresetTexture_${_fileBasename}")
        endforeach()

        # Write loader function
        file(APPEND "${resourceFile}" "void LoadIdlePresetTextures(TextureManager& textureManager)\n{\n")
        foreach(_resource ${_resourceList})
            list(GET _resource 0 _textureName)
            list(GET _resource 1 _textureTypeName)

            message(STATUS "_textureName = ${_textureName}")
            message(STATUS "_textureTypeName = ${_textureTypeName}")
            file(APPEND "${resourceFile}" "    CreateIdlePresetTexture<${_textureTypeName}Array>(textureManager, ${_textureTypeName}, \"${_textureName}\");\n")
        endforeach()
        file(APPEND "${resourceFile}" "}\n\n")

    endif()
    unset(_isOneFileNewer)
endmacro()
