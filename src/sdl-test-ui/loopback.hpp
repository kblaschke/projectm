#pragma once

#include "pmSDL.hpp"

class ProjectMSDL;

auto InitLoopback() -> bool;
void ConfigureLoopback(ProjectMSDL* app);
auto ProcessLoopbackFrame(ProjectMSDL* app) -> bool;
