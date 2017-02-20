#!/bin/sh
# This script is used to build the ellipseOnboardMagCalib example on unix systems.
# Example: ./build.sh

CC="/media/secure/hg/sda14_soft/sda14_buildroot/builddir/host/usr/bin/arm-buildroot-linux-gnueabi-gcc"
$CC -Wall ../../src/ellipseOnboardMagCalib.c  -I../../../../src/ ../../../../libSbgECom.a -o ../../ellipseOnboardMagCalib

