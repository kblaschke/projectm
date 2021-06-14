
include(IdlePresetResourceBuilder.cmake)

set(IDLE_PRESET_RESOURCES "${CMAKE_CURRENT_BINARY_DIR}/include/IdlePresetResources.h")
file(MAKE_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/include")
generate_idle_preset(${CMAKE_SOURCE_DIR}/presets/idle IdlePresetResources.h.in "${IDLE_PRESET_RESOURCES}")

add_library(MilkdropPresetFactory OBJECT
        "${IDLE_PRESET_RESOURCES}"
        BuiltinFuncs.cpp
        BuiltinFuncs.hpp
        BuiltinParams.cpp
        BuiltinParams.hpp
        CustomShape.cpp
        CustomShape.hpp
        CustomWave.cpp
        CustomWave.hpp
        CValue.hpp
        Eval.cpp
        Eval.hpp
        Expr.cpp
        Expr.hpp
        Func.cpp
        Func.hpp
        IdlePreset.cpp
        IdlePreset.hpp
        InitCond.cpp
        InitCond.hpp
        InitCondUtils.hpp
        JitContext.hpp
        MilkdropPreset.cpp
        MilkdropPresetFactory.cpp
        MilkdropPresetFactory.hpp
        MilkdropPreset.hpp
        Param.cpp
        Param.hpp
        ParamUtils.hpp
        Parser.cpp
        Parser.hpp
        PerFrameEqn.cpp
        PerFrameEqn.hpp
        PerPixelEqn.cpp
        PerPixelEqn.hpp
        PerPointEqn.cpp
        PerPointEqn.hpp
        PresetFrameIO.cpp
        PresetFrameIO.hpp
        )

target_include_directories(MilkdropPresetFactory
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        "${CMAKE_CURRENT_BINARY_DIR}/include"
        PUBLIC
        "${CMAKE_CURRENT_SOURCE_DIR}"
        )

target_link_libraries(MilkdropPresetFactory
        PUBLIC
        GLM::GLM
        ${PROJECTM_OPENGL_LIBRARIES}
        )

set_target_properties(MilkdropPresetFactory PROPERTIES
        FOLDER libprojectM
        )

if(ENABLE_LLVM)
    target_link_libraries(MilkdropPresetFactory
            PUBLIC
            LLVM::LLVM
            )
endif()
