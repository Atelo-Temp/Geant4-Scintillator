# About

...

## Building The Outfile

In the project base directory (i.e. ./B1):

```
mkdir ./build
cd ./build
cmake -DGeant4_DIR=~/geant4/geant4-v11.3.2/install/lib64/cmake/Geant4/ ../../B1
make -j N exampleB1
```

Makes directory
Changes to directory
Populates build directory
Compiles the executable in the build directory

Note, in line 13: "N" is the number of processes (i.e. 2)

Note, i think this can also be shortened to: -DGeant4_DIR=~/geant4/geant4-v11.3.2/

## Rebuilding The Outfile

Once the build is configured, code in the source directory can be edited, and only call:

```
make
```

... Inside of the build directory.

This will pick up and compile any changes made in the source directory.

NOTE: If files are added or removed, CMake must be re-run from the beginning,
which is one of the reasons why Kitware recommend listing sources explicity,
instead of relying on globbing:

```
file(GLOB sources ${PROJECT_SOURCE_DIR}/src/*.cc)
file(GLOB headers ${PROJECT_SOURCE_DIR}/include/*.hh)
```

^ From line 17&18 in CMakeLists.txt, convenient, but listing explicity reduces chances of errors.

## Running The Outfile

In the base directory:

./build/exampleB1

## ...

Default particle set in PrimaryGeneratorAction.cc

Default can be overridden via macro (i.e. "gps/particle/gamma") after compilation, and changed between runs

## ...

```
find . -name "*.root" -type f
find . -name "*.root" -type f -delete
```
