TOP=../..
include $(TOP)/configure/CONFIG
#=======================================

# Build devHP3458A as a library for an IOC:
LIBRARY_IOC += devHP3458A

# Library Source files
devHP3458A_SRCS += devHP3458A.c

# Link with the asyn and base libraries
devHP3458A_LIBS += asyn
devHP3458A_LIBS += $(EPICS_BASE_IOC_LIBS)

# Install .dbd and .db files
DBD += devHP3458A.dbd

#=======================================
include $(TOP)/configure/RULES
