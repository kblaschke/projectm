# Converts any file to a string that can be inserted into a C char array to store binary data as a resource.
# The function sets the byte representation and the length of the original file in the variables named in
# the second and third parameters.
# If the file could not be read, the size variable will be zero and the byte string empty.

function(file_to_char_array _filename _data_var _size_var)
    if(NOT EXISTS "${_filename}")
        set(${_data_var} "" PARENT_SCOPE)
        set(${_size_var} 0 PARENT_SCOPE)
        return()
    endif()

    file(READ "${_filename}" _file_data HEX)
    file(SIZE "${_filename}" _file_size)

    if(NOT _file_data OR NOT _file_size)
        set(${_data_var} "" PARENT_SCOPE)
        set(${_size_var} 0 PARENT_SCOPE)
        return()
    endif()

    set(_output_list "")
    foreach(_index RANGE 1 ${_file_size})
        math(EXPR _char_pos "(${_index} - 1) * 2")
        math(EXPR _line_pos "(${_index} - 1) % 16")
        string(SUBSTRING "${_file_data}" ${_char_pos} 2 _one_byte)

        # Out will contain 16 bytes per line for readability.
        set(_newline "")
        if(_index GREATER 1 AND _line_pos EQUAL 0)
            set(_newline "\n")
        endif()

        # Using a list is slightly faster than appending bytes to a string thousands of times.
        list(APPEND _output_list "${_newline}0x${_one_byte}")
    endforeach()

    list(JOIN _output_list ", " _output)

    set(${_data_var} "${_output}" PARENT_SCOPE)
    set(${_size_var} ${_file_size} PARENT_SCOPE)
endfunction()
