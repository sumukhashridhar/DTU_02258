
ifndef CC
	CC = gcc
endif
CCFLAGS=-std=c99

LIBS = -lOpenCL

CL_DIR = /zhome/bb/5/41921/Public/OpenCL/AMDAPPSDK-3.0/include
LIB_DIR = /zhome/bb/5/41921/Public/OpenCL/AMDAPPSDK-3.0/lib/x86_64

COMMON_DIR = ../../C_common

# Check our platform and make sure we define the APPLE variable
# and set up the right compiler flags and libraries
PLATFORM = $(shell uname -s)
ifeq ($(PLATFORM), Darwin)
	LIBS = -framework OpenCL
endif

DeviceInfo: DeviceInfo.c
	gcc $^ $(CCFLAGS) $(LIBS) -I $(COMMON_DIR) -I $(CL_DIR) -L$(LIB_DIR) -o $@


clean:
	rm -f DeviceInfo
