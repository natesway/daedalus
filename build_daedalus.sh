#!/bin/bash

## TODO:
# Enable use of nproc for processor detection so builds according to number of processors in machine (Darwin will need sysctl -n hw.logicalcpu)

PLATFORM=$1"build"

function pre_prep(){

    if [ -d $PWD/$PLATFORM ]; then

        echo "Removing previous build attempt"
        rm -r "$PWD/$PLATFORM"
    fi

    if [ -d $PWD/DaedalusX64 ]; then
        rm -r $PWD/DaedalusX64/EBOOT.PBP
    else
        mkdir $PWD/DaedalusX64
        mkdir ../DaedalusX64/SaveStates
        mkdir ../DaedalusX64/SaveGames
        mkdir ../DaedalusX64/Roms
    fi
        mkdir "$PWD/$PLATFORM"
}

function finalPrep() {

    if [ ! -d ../DaedalusX64 ]; then
        mkdir ../DaedalusX64/SaveStates ../DaedalusX64/SaveGames ../DaedalusX64/Roms
    fi

    if [ -f "$PWD/EBOOT.PBP" ]; then
        mv "$PWD/EBOOT.PBP" ../DaedalusX64/
        cp -r ../Data/* ../DaedalusX64/
    else
        cp -r ../Data/* ../DaedalusX64/
        cp ../Source/SysGL/HLEGraphics/n64.psh ../DaedalusX64
    fi
}

function build() {

## Build PSP extensions - Really need to make these cmake files
if [[ $PLATFORM = "PSP"* ]]; then
  make -C "$PWD/../Source/SysPSP/PRX/DveMgr"
  make -C "$PWD/../Source/SysPSP/PRX/ExceptionHandler"
  make -C "$PWD/../Source/SysPSP/PRX/KernelButtons"
  make -C "$PWD/../Source/SysPSP/PRX/MediaEngine"
fi

make -j8 # Should make this use whatever is avaliable but that's fine for now

if [[ $PLATFORM = "PSP"* ]] &&  [[ -f "$PWD/daedalus.elf" ]]; then
  psp-fixup-imports daedalus.elf
  mksfoex -d MEMSIZE=1 DaedalusX64 PARAM.SFO
  psp-prxgen daedalus.elf daedalus.prx
  cp ../Source/SysPSP/Resources/eboot_icons/* "$PWD"
  pack-pbp EBOOT.PBP PARAM.SFO icon0.png NULL NULL pic1.png NULL daedalus.prx NULL
fi

finalPrep
}

#Add Defines for CMake to not mess up the main loop
if [[ $1 = "PSP" ]]; then

    CMAKEDEFINES+="-DCMAKE_TOOLCHAIN_FILE=../Tools/psptoolchain.cmake"
fi

CMAKEDEFINES+=" -D$1=1"
if [[ $2 = "DEBUG" ]]; then
    CMAKEDEFINES+=" -D$2=1"
fi

## Main()
## Do our OS checking here
case "$1" in
    PSP | LINUX | MAC | WIN)
    pre_prep
    cd $PLATFORM
    cmake $CMAKEDEFINES ../Source
    build
    ;;
    **)
    echo "Build Options: PSP / LINUX / MAC / WIN"
    echo "Building debug or profile builds requires DEBUG or PROFILE after build option"
    ;;
    esac

if [[ $1 = "LINUX" ]]; then
    cp ../Source/SysGL/HLEGraphics/n64.psh ../LINUXbuild
fi
