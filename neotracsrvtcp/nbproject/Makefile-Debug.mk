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
CC=gcc
CCC=g++
CXX=g++
FC=gfortran
AS=as

# Macros
CND_PLATFORM=GNU-Linux-x86
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
	${OBJECTDIR}/_ext/1270477542/NeoProtocolBase.o \
	${OBJECTDIR}/_ext/1270477542/base91.o \
	${OBJECTDIR}/_ext/1270477542/custlog.o \
	${OBJECTDIR}/_ext/1270477542/database.o \
	${OBJECTDIR}/_ext/1270477542/helpers.o \
	${OBJECTDIR}/neotracsrvtcp.o


# C Compiler Flags
CFLAGS=

# CC Compiler Flags
CCFLAGS=-Wall -Wextra -std=gnu++11
CXXFLAGS=-Wall -Wextra -std=gnu++11

# Fortran Compiler Flags
FFLAGS=

# Assembler Flags
ASFLAGS=

# Link Libraries and Options
LDLIBSOPTIONS=-L../poco-1.6.1/lib/Linux/i686 -Wl,-rpath,./ -dynamic -lPocoNetd -lPocoUtild -lPocoFoundationd -lPocoJSONd -lPocoXMLd

# Build Targets
.build-conf: ${BUILD_SUBPROJECTS}
	"${MAKE}"  -f nbproject/Makefile-${CND_CONF}.mk ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/neotracsrvtcp

${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/neotracsrvtcp: ${OBJECTFILES}
	${MKDIR} -p ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}
	${LINK.cc} -o ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/neotracsrvtcp ${OBJECTFILES} ${LDLIBSOPTIONS}

${OBJECTDIR}/_ext/1270477542/NeoProtocolBase.o: ../common/NeoProtocolBase.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1270477542
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -D_DEBUG -I../poco-1.6.1/Util/include -I../poco-1.6.1/Net/include -I../poco-1.6.1/Foundation/include -I../common -I../boost -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1270477542/NeoProtocolBase.o ../common/NeoProtocolBase.cpp

${OBJECTDIR}/_ext/1270477542/base91.o: ../common/base91.c 
	${MKDIR} -p ${OBJECTDIR}/_ext/1270477542
	${RM} "$@.d"
	$(COMPILE.c) -g -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1270477542/base91.o ../common/base91.c

${OBJECTDIR}/_ext/1270477542/custlog.o: ../common/custlog.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1270477542
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -D_DEBUG -I../poco-1.6.1/Util/include -I../poco-1.6.1/Net/include -I../poco-1.6.1/Foundation/include -I../common -I../boost -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1270477542/custlog.o ../common/custlog.cpp

${OBJECTDIR}/_ext/1270477542/database.o: ../common/database.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1270477542
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -D_DEBUG -I../poco-1.6.1/Util/include -I../poco-1.6.1/Net/include -I../poco-1.6.1/Foundation/include -I../common -I../boost -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1270477542/database.o ../common/database.cpp

${OBJECTDIR}/_ext/1270477542/helpers.o: ../common/helpers.cpp 
	${MKDIR} -p ${OBJECTDIR}/_ext/1270477542
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -D_DEBUG -I../poco-1.6.1/Util/include -I../poco-1.6.1/Net/include -I../poco-1.6.1/Foundation/include -I../common -I../boost -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/_ext/1270477542/helpers.o ../common/helpers.cpp

${OBJECTDIR}/neotracsrvtcp.o: neotracsrvtcp.cpp 
	${MKDIR} -p ${OBJECTDIR}
	${RM} "$@.d"
	$(COMPILE.cc) -g -Werror -D_DEBUG -I../poco-1.6.1/Util/include -I../poco-1.6.1/Net/include -I../poco-1.6.1/Foundation/include -I../common -I../boost -MMD -MP -MF "$@.d" -o ${OBJECTDIR}/neotracsrvtcp.o neotracsrvtcp.cpp

# Subprojects
.build-subprojects:

# Clean Targets
.clean-conf: ${CLEAN_SUBPROJECTS}
	${RM} -r ${CND_BUILDDIR}/${CND_CONF}
	${RM} ${CND_DISTDIR}/${CND_CONF}/${CND_PLATFORM}/neotracsrvtcp

# Subprojects
.clean-subprojects:

# Enable dependency checking
.dep.inc: .depcheck-impl

include .dep.inc
