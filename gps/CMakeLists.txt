#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 3.16...3.27)
project(gps)

#----------------------------------------------------------------------------
# Create compile_commands.json for C++ LSP
#
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#----------------------------------------------------------------------------
# Find Geant4 package, activating all available UI and Vis drivers by default
# See the documentation for a guide on how to enable/disable specific components
#
find_package(Geant4 REQUIRED ui_all vis_all)

#----------------------------------------------------------------------------
# Locate sources and headers for this project
# NB: headers are included so they will show up in IDEs
#
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)

#----------------------------------------------------------------------------
# Add the executable, use our local headers, and link it to the Geant4 libraries
#
add_executable(gps gps.cc ${sources} ${headers})
target_include_directories(gps PRIVATE include)
target_link_libraries(gps PRIVATE ${Geant4_LIBRARIES})

#----------------------------------------------------------------------------
# Copy all scripts to the build directory, i.e. the directory in which we
# build the executable. This is so that we can run the executable directly 
# because it relies on these scripts being in the current working directory.
#
set(GPS_SCRIPTS
  run.mac
  vis.mac
  test.mac
  137Cs.mac
  60Co.mac
  241Am.mac
  22Na.mac
  )

foreach(_script ${GPS_SCRIPTS})
  configure_file(
    # ${PROJECT_SOURCE_DIR}/${_script}
    ${PROJECT_SOURCE_DIR}/macros/${_script}
    ${PROJECT_BINARY_DIR}/${_script}
    COPYONLY
    )
endforeach()


## Alternate approach if using dedicated macros dir
# file(GLOB MACRO_FILES "macros/*")
# file(COPY ${MACRO_FILES} DESTINATION ${CMAKE_BINARY_DIR}/)
