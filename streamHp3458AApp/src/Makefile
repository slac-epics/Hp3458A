TOP=../..

include $(TOP)/configure/CONFIG
#----------------------------------------
#  ADD MACRO DEFINITIONS AFTER THIS LINE
#=============================
# Build an IOC support library
LIBRARY_IOC += Hp3458AStream

# devHp3458AStream.dbd will be created and installed
DBD += Hp3458AStream.dbd

# Hp3458AStream.dbd will be made up from these files:
Hp3458AStream_DBD += base.dbd

# streams:
#Hp3458AStream_DBD += aaiRecord.dbd
#Hp3458AStream_DBD += aaoRecord.dbd
Hp3458AStream_DBD += asyn.dbd
Hp3458AStream_DBD += asynRecord.dbd
Hp3458AStream_DBD += stream.dbd
Hp3458AStream_DBD += drvAsynIPPort.dbd
Hp3458AStream_DBD += drvVxi11.dbd
Hp3458AStream_DBD += drvAsynSerialPort.dbd

# The following are compiled and added to the Support library
Hp3458AStream_SRCS    += Hp3458AStream.c

#add a definition for each support application used by this module
Hp3458AStream_LIBS += stream
Hp3458AStream_LIBS += asyn

#NOTES:
#      1)It is not possible to build sncExample both as a component of streamHp3458A
#        and standalone. You must choose only one.
#      2)To build sncExample SNCSEQ must be defined in <top>/configure/RELEASE

#The following builds sncExample as a component of freqCount
#streamHp3458A_SRCS += sncExample.stt
#streamHp3458A_LIBS += seq pv

Hp3458AStream_LIBS += $(EPICS_BASE_IOC_LIBS)

#===========================
include $(TOP)/configure/RULES
#----------------------------------------
#  ADD RULES AFTER THIS LINE

