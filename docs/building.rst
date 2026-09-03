Building projectM from source
=============================

libprojectM is built with CMake on all platforms. (The GNU Autotools
build was removed in projectM 4.0.)

The maintained build instructions live in the repository root and are
the single source of truth — this page only summarizes them:

-  `BUILDING.md <https://github.com/projectM-visualizer/projectm/blob/master/BUILDING.md>`__:
   quick-start guide for Linux, macOS and Windows.
-  `BUILDING-cmake.md <https://github.com/projectM-visualizer/projectm/blob/master/BUILDING-cmake.md>`__:
   all CMake build options and generators, plus how to use libprojectM
   in other CMake-based projects.

Quick start (Debian / Ubuntu)
-----------------------------

.. code:: bash

   sudo apt install build-essential cmake libgl1-mesa-dev mesa-common-dev
   git clone https://github.com/projectM-visualizer/projectm.git
   cd projectm
   git submodule update --init
   cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
   cmake --build build
   sudo cmake --build build --target install

projectM requires CMake 3.21 or newer. To additionally build the
SDL-based developer test UI, install ``libsdl2-dev`` and configure with
``-DENABLE_SDL_UI=ON``.
