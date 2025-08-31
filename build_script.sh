# Check if build folder in current dir, if not, cd ../
cd ../

# Will need to check if exists
rm -rf build

mkdir build

cd build

# cd ../ && rm -rf build && mkdir build && cd build

# cmake -DGeant4_DIR=~/geant4/geant4-v11.3.2/install/lib64/cmake/Geant4/ ../../scintillator
cmake -DGeant4_DIR=~/geant4/geant4-v11.3.2/install/lib64/cmake/Geant4/ ../

make -j 4 scintillator

# cd ../ && rm -rf build && mkdir build && cd build && cmake -DGeant4_DIR=~/geant4/geant4-v11.3.2/install/lib64/cmake/Geant4/ ../../scintillator && make -j 4 scintillator

# cd ../ && rm -rf build && mkdir build && cd build && cmake -DGeant4_DIR=~/geant4/geant4-v11.3.2/install/lib64/cmake/Geant4/ ../ && make -j 4 scintillator
