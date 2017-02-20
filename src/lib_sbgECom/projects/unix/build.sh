#!/bin/sh
# This script is used to build the sbgCom library on unix systems.
# To compile the library, you have to specify the byte ordering.
# Example: ./build.sh SBG_PLATFORM_LITTLE_ENDIAN

#CC="/media/secure/hg/sda14_soft/sda14_tools/buildroot/bin/arm-buildroot-linux-gnueabi-gcc"
#AR="/media/secure/hg/sda14_soft/sda14_tools/buildroot/bin/arm-buildroot-linux-gnueabi-ar"

CC="gcc"
AR="ar"

# Test that we have the endianness argument
if [ $# -ne 1 ]; then
    echo "You have to specify the platform endianness using either SBG_PLATFORM_BIG_ENDIAN or SBG_PLATFORM_LITTLE_ENDIAN"
    exit 1
fi

# Test the first argument and define the $CC options according to the selected endianness
if [ "$1" = "SBG_PLATFORM_BIG_ENDIAN" ]; then
    # The platform is in big endian
    gccOptions="-c -Wall -D SBG_PLATFORM_BIG_ENDIAN"
elif [ "$1" = "SBG_PLATFORM_LITTLE_ENDIAN" ]; then
    # The platform is in little endian
    gccOptions="-c -Wall -D SBG_PLATFORM_LITTLE_ENDIAN"
else
    echo "You have entered an invalid argument"
    exit 1
fi

# Create the intermediate directory		
mkdir obj

# Create all objects for binary logs directory
$CC $gccOptions ../../src/binaryLogs/binaryLogDebug.c -o obj/binaryLogDebug.o
$CC $gccOptions ../../src/binaryLogs/binaryLogDvl.c -o obj/binaryLogDvl.o
$CC $gccOptions ../../src/binaryLogs/binaryLogEkf.c -o obj/binaryLogEkf.o
$CC $gccOptions ../../src/binaryLogs/binaryLogEvent.c -o obj/binaryLogEvent.o
$CC $gccOptions ../../src/binaryLogs/binaryLogGps.c -o obj/binaryLogGps.o
$CC $gccOptions ../../src/binaryLogs/binaryLogImu.c -o obj/binaryLogImu.o
$CC $gccOptions ../../src/binaryLogs/binaryLogMag.c -o obj/binaryLogMag.o
$CC $gccOptions ../../src/binaryLogs/binaryLogOdometer.c -o obj/binaryLogOdometer.o
$CC $gccOptions ../../src/binaryLogs/binaryLogPressure.c -o obj/binaryLogPressure.o
$CC $gccOptions ../../src/binaryLogs/binaryLogs.c -o obj/binaryLogs.o
$CC $gccOptions ../../src/binaryLogs/binaryLogShipMotion.c -o obj/binaryLogShipMotion.o
$CC $gccOptions ../../src/binaryLogs/binaryLogStatus.c -o obj/binaryLogStatus.o
$CC $gccOptions ../../src/binaryLogs/binaryLogUsbl.c -o obj/binaryLogUsbl.o
$CC $gccOptions ../../src/binaryLogs/binaryLogUtc.c -o obj/binaryLogUtc.o

# Create all objects for commands directory
$CC $gccOptions ../../src/commands/commandsAdvanced.c -o obj/commandsAdvanced.o
$CC $gccOptions ../../src/commands/commandsCommon.c -o obj/commandsCommon.o
$CC $gccOptions ../../src/commands/commandsEvent.c -o obj/commandsEvent.o
$CC $gccOptions ../../src/commands/commandsGnss.c -o obj/commandsGnss.o
$CC $gccOptions ../../src/commands/commandsInfo.c -o obj/commandsInfo.o
$CC $gccOptions ../../src/commands/commandsInterface.c -o obj/commandsInterface.o
$CC $gccOptions ../../src/commands/commandsMag.c -o obj/commandsMag.o
$CC $gccOptions ../../src/commands/commandsOdo.c -o obj/commandsOdo.o
$CC $gccOptions ../../src/commands/commandsOutput.c -o obj/commandsOutput.o
$CC $gccOptions ../../src/commands/commandsSensor.c -o obj/commandsSensor.o
$CC $gccOptions ../../src/commands/commandsSettings.c -o obj/commandsSettings.o


# Create all objects for interfaces directory
$CC $gccOptions ../../src/interfaces/interfaceFile.c -o obj/interfaceFile.o
$CC $gccOptions ../../src/interfaces/interfaceUdp.c -o obj/interfaceUdp.o
$CC $gccOptions ../../src/interfaces/interfaceSerialUnix.c -o obj/interfaceSerialUnix.o

# Create all objects for misc directory
$CC $gccOptions ../../src/misc/sbgCrc.c -o obj/sbgCrc.o
$CC $gccOptions ../../src/misc/transfer.c -o obj/transfer.o

# Create all objects for protocol directory
$CC $gccOptions ../../src/protocol/protocol.c -o obj/protocol.o

# Create all objects for time directory
$CC $gccOptions ../../src/time/sbgTime.c -o obj/sbgTime.o

# Create all objets for the root directory
$CC $gccOptions ../../src/sbgECom.c -o obj/sbgECom.o

# Create the library
mkdir -p ../../lib
$AR cr ../../lib/libsbgECom.a obj/binaryLogDebug.o obj/binaryLogDvl.o obj/binaryLogEkf.o obj/binaryLogEvent.o obj/binaryLogGps.o obj/binaryLogImu.o obj/binaryLogMag.o obj/binaryLogOdometer.o obj/binaryLogPressure.o obj/binaryLogs.o obj/binaryLogShipMotion.o obj/binaryLogStatus.o obj/binaryLogUsbl.o obj/binaryLogUtc.o obj/commandsAdvanced.o obj/commandsCommon.o obj/commandsEvent.o obj/commandsGnss.o obj/commandsInfo.o obj/commandsInterface.o obj/commandsMag.o obj/commandsOdo.o obj/commandsOutput.o obj/commandsSensor.o obj/commandsSettings.o obj/interfaceFile.o obj/interfaceUdp.o obj/interfaceSerialUnix.o obj/sbgCrc.o obj/transfer.o obj/protocol.o obj/sbgTime.o obj/sbgECom.o
