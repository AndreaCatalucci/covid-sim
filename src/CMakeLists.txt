# CMakeLists.txt

# CMake setup
cmake_minimum_required (VERSION 3.8)

# Set a default build type if none was specified
set(default_build_type "RelWithDebInfo")

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# Project initialisation
project("CovidSim")

option(USE_OPENMP "Compile with OpenMP parallelism enabled" ON)

# Packages used
if(USE_OPENMP)
  find_package(OpenMP REQUIRED)
endif()

# specify the C++ standard
set(CMAKE_CXX_STANDARD 11)

# CovidSim target
add_executable(CovidSim main.cpp CovidSim.cpp CovidSim.h binio.cpp binio.h Rand.cpp Rand.h Constants.h Country.h MachineDefines.h Error.cpp Error.h Dist.cpp Dist.h Kernels.cpp Kernels.h Bitmap.cpp Bitmap.h Model.h Param.h SetupModel.cpp SetupModel.h SharedFuncs.h ModelMacros.h InfStat.h CalcInfSusc.cpp CalcInfSusc.h Sweep.cpp Sweep.h Update.cpp Update.h)
target_include_directories(CovidSim PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
if(USE_OPENMP)
  target_link_libraries(CovidSim PUBLIC OpenMP::OpenMP_CXX)
endif()
if(WIN32)
  target_link_libraries(CovidSim PUBLIC Gdiplus.lib Vfw32.lib)
  target_compile_definitions(CovidSim PUBLIC  "_CRT_SECURE_NO_WARNINGS")
elseif(UNIX)
  target_compile_definitions(CovidSim PUBLIC UNIX)
else()
  message(FATAL_ERROR "Unknown operating system type: ${CMAKE_SYSTEM_NAME}")
endif()

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
                 ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
                 EXCLUDE_FROM_ALL)

# Link Tests
add_executable(SweepUnitTest Sweep.unittest.cpp CovidSim.h CovidSim.cpp binio.cpp binio.h Rand.cpp Rand.h Constants.h Country.h MachineDefines.h Error.cpp Error.h Dist.cpp Dist.h Kernels.cpp Kernels.h Bitmap.cpp Bitmap.h Model.h Param.h SetupModel.cpp SetupModel.h SharedFuncs.h ModelMacros.h InfStat.h CalcInfSusc.cpp CalcInfSusc.h Sweep.cpp Sweep.h Update.cpp Update.h)
target_link_libraries(SweepUnitTest gtest_main)
target_compile_definitions(SweepUnitTest PUBLIC UNIX)
add_test(NAME SweepUnitTest COMMAND SweepUnitTest)

add_executable(UpdateUnitTest Update.unittest.cpp CovidSim.h CovidSim.cpp binio.cpp binio.h Rand.cpp Rand.h Constants.h Country.h MachineDefines.h Error.cpp Error.h Dist.cpp Dist.h Kernels.cpp Kernels.h Bitmap.cpp Bitmap.h Model.h Param.h SetupModel.cpp SetupModel.h SharedFuncs.h ModelMacros.h InfStat.h CalcInfSusc.cpp CalcInfSusc.h Sweep.cpp Sweep.h Update.cpp Update.h)
target_link_libraries(UpdateUnitTest gtest_main)
target_compile_definitions(UpdateUnitTest PUBLIC UNIX)
add_test(NAME UpdateUnitTest COMMAND UpdateUnitTest)
