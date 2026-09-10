#pragma once

#include "pmSDL.hpp"

#include <string>

class ProjectMSDL;

void DebugGl(GLenum source,
             GLenum type,
             GLuint id,
             GLenum severity,
             GLsizei length,
             const GLchar* message,
             const void* userParam);

auto GetConfigFilePath(const std::string& datadirPath) -> std::string;
void SeedRand();
void InitGl();
void DumpOpenGlInfo();
void EnableGlDebugOutput();
void TestAllPresets(ProjectMSDL* app);
auto SetupSdlApp() -> ProjectMSDL*;
auto StartUnlockedFpsCounter() -> int64_t;
void AdvanceUnlockedFpsCounterFrame(int64_t startFrame);
