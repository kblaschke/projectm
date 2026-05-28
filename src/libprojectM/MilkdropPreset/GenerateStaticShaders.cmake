# Run as custom command in script mode if any shader file has changed.
# Recreates the MilkdropBuiltinShaders.hpp/.cpp files accordingly.

set(SHADER_CONTENTS "")
set(SHADER_ACCESSOR_DECLARATIONS "")
set(SHADER_ACCESSOR_DEFINITIONS "")
foreach(shader_file IN LISTS SHADER_FILES)
    cmake_path(GET shader_file FILENAME _shader_name)
    cmake_path(GET shader_file EXTENSION _shader_type)
    cmake_path(REMOVE_EXTENSION _shader_name)
    file(READ "${shader_file}" _shader_contents)

    string(APPEND SHADER_CONTENTS "static std::string k${_shader_name} = R\"(\n${_shader_contents})\";\n\n")
    string(APPEND SHADER_ACCESSOR_DECLARATIONS "    std::string Get${_shader_name}() const;\n")

    if(_shader_type STREQUAL ".inc")
        # Includes don't get the header prepended.
        string(APPEND SHADER_ACCESSOR_DEFINITIONS "IMPLEMENT_SHADER_ACCESSOR_NO_HEADER(${_shader_name});\n")
    else()
        string(APPEND SHADER_ACCESSOR_DEFINITIONS "IMPLEMENT_SHADER_ACCESSOR(${_shader_name});\n")
    endif()
endforeach()

configure_file(MilkdropBuiltinShaders.hpp.in ${OUTPUT_DIR}/MilkdropBuiltinShaders.hpp @ONLY)
configure_file(MilkdropBuiltinShaders.cpp.in ${OUTPUT_DIR}/MilkdropBuiltinShaders.cpp @ONLY)
