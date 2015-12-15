#
# Generated Makefile - do not edit!
#
# Edit the Makefile in the project folder instead (../Makefile). Each target
# has a -pre and a -post target defined where you can add customized code.
#
# This makefile implements configuration specific macros and targets.


# Environment
MKDIR=mkdir
CP=cp
GREP=grep
NM=nm
CCADMIN=CCadmin
RANLIB=ranlib
CC=clang
CCC=clang++
CXX=clang++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=CLang-Linux-x86
CND_DLIB_EXT=so
CND_CONF=Debug
CND_DISTDIR=dist
CND_BUILDDIR=build

# Include project Makefile
include Makefile

# Object Directory
OBJECTDIR=${CND_BUILDDIR}/${CND_CONF}/${CND_PLATFORM}

# Object Files
OBJECTFILES= \
	${OBJECTDIR}/src/vistek_logger.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-m64 -pthread
CXXFLAGS=-m64 -pthread

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../__output/${CND_CONF}/${CND_PLATFORM} -lboost_filesystem -lboost_system

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ../__output/${CND_CONF}/${CND_PLATFORM}/libvistek_logger.${CND_DLIB_EXT}

../__output/${CND_CONF}/${CND_PLATFORM}/libvistek_logger.${CND_DLIB_EXT}: ${OBJECTFILES}
	${MKDIR} -p ../__output/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ../__output/${CND_CONF}/${CND_PLATFORM}/libvistek_logger.${CND_DLIB_EXT} ${OBJECTFILES} ${LDLIBSOPTIONS} -shared -fPIC

${OBJECTDIR}/src/vistek_logger.o: nbproject/Makefile-${CND_CONF}.mk src/vistek_logger.cpp 
	${MKDIR} -p ${OBJECTDIR}/src
	${RM} "$@.d"
	$(COMPILE.cc) -g -I. -Iinclude -I../common/include -I~/sdk/boost_1_56_0 -std=c++11 -fPIC  -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/src/vistek_logger.o src/vistek_logger.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ../__output/${CND_CONF}/${CND_PLATFORM}/libvistek_logger.${CND_DLIB_EXT}

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
