set(SOURCES "")

# Build default glad objects, always include GL Core/GLES 3/EGL
add_library(glad_obj
        OBJECT
        include/glad/egl.h
        include/glad/gl.h
        src/egl.c
        src/gl.c
)

# Add OS-specific APIs
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    target_sources(glad_obj
            PUBLIC
            include/glad/wgl.h
            src/wgl.c
    )
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    target_sources(glad_obj
            PUBLIC
            include/glad/glx.h
            src/glx.c
    )
endif()

target_include_directories(glad_obj
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

# wrap the objects in a static library
add_library(glad STATIC $<TARGET_OBJECTS:glad_obj>)

# propagate include dirs via library target
target_include_directories(glad
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        )

target_compile_definitions(glad_obj
        PUBLIC
        GLAD_GLES2
        PRIVATE
        GLAD_GLX_NO_X11
        )

# platform link dependencies
if (UNIX AND NOT APPLE)
    target_link_libraries(glad PUBLIC dl)
endif()
