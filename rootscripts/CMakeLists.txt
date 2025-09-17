#----------------------------------------------------------------------------
# Setup the project
#
cmake_minimum_required(VERSION 3.16...3.27)
project(rootscripts LANGUAGES CXX)

#----------------------------------------------------------------------------
# Create compile_commands.json for C++ LSP
#
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

#----------------------------------------------------------------------------
# Find ROOT package
#
find_package(ROOT 6.34 CONFIG REQUIRED)

#----------------------------------------------------------------------------
# Sets up global settings
#
# include("${ROOT_USE_FILE}")

#----------------------------------------------------------------------------
# Add the executable, and link it to the root libraries
#
# To see available libs:
# root-config --libs
#
add_executable(rootscripts read_ascii.cc)
target_link_libraries(rootscripts PRIVATE 
    ROOT::Core 
    ROOT::RIO # IO ?
    ROOT::Physics 
    ROOT::Hist # Histograms
    ROOT::Tree # NTuples
    ROOT::Graf # Graphs?
    ROOT::Gpad # TCanvas (Graphics?)
    )



## In build: cmake -DROOT_DIR=$ROOTSYS/cmake ..
