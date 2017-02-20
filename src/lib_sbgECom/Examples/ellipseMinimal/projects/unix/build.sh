#!/bin/sh
# This script is used to build the ellipseMinimal example on unix systems.
# Example: ./build.sh

CC="/media/secure/hg/sda14_soft/sda14_tools/buildroot/bin/arm-buildroot-linux-gnueabi-gcc"
$CC -Wall ../../src/ellipseMinimal.c  -I../../../../src/ ../../../../libSbgECom.a -o ../../ellipseMinimal

