#!/bin/bash

cd ./projects/unix
./build.sh SBG_PLATFORM_LITTLE_ENDIAN
cd ../../

#ln -fs /media/secure/hg/sda14_soft/comet_moos/src/lib_sbgECom/include/sbgEComLib /media/secure/hg/sda14_soft/sda14_tools/buildroot/arm-buildroot-linux-gnueabi/sysroot/usr/include/sbgEComLib
#ln -fs /media/secure/hg/sda14_soft/comet_moos/src/lib_sbgECom/libSbgECom.a /media/secure/hg/sda14_soft/sda14_tools/buildroot/arm-buildroot-linux-gnueabi/sysroot/usr/lib/libsbgECom.a

# Attention a la majuscule dans libsbgECom.a

