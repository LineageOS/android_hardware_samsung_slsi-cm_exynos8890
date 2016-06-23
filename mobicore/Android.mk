# =============================================================================
#
# Makefile pointing to all makefiles within the project.
#
# =============================================================================

# Only compile proxy support for Android M (use stlport absence as indicator)
use_proxy =
ifeq ($(wildcard external/stlport/libstlport.mk),)
	use_proxy = yes
endif

MOBICORE_PROJECT_PATH := $(call my-dir)
# Setup common variables
COMP_PATH_Logwrapper := $(MOBICORE_PROJECT_PATH)/common/LogWrapper
COMP_PATH_MobiCore := $(MOBICORE_PROJECT_PATH)/common/MobiCore
COMP_PATH_MobiCoreDriverMod := $(MOBICORE_PROJECT_PATH)/include
COMP_PATH_AndroidOpenSsl := $(MOBICORE_PROJECT_PATH)/../openssl

# Application wide Cflags
GLOBAL_INCLUDES := bionic \
	external/stlport/stlport \
	$(COMP_PATH_MobiCore)/inc \
	$(COMP_PATH_MobiCore)/inc/McLib \
	$(COMP_PATH_MobiCoreDriverMod)/Public \
	$(COMP_PATH_MobiCore)/inc/TlCm

ifndef use_proxy
GLOBAL_LIBRARIES := libstlport
endif

# Include the Daemon
include $(MOBICORE_PROJECT_PATH)/MobiCoreDriverLib/Android.mk

MC_INCLUDE_DIR := $(COMP_PATH_MobiCore)/inc \
    $(COMP_PATH_MobiCore)/inc/TlCm \
    $(COMP_PATH_MobiCore)/inc/TlCm/2.0 \
    $(MOBICORE_PROJECT_PATH)/MobiCoreDriverLib/ClientLib/public \
    $(MOBICORE_PROJECT_PATH)/MobiCoreDriverLib/Registry/Public
MC_DEBUG := _DEBUG
SYSTEM_LIB_DIR=/system/lib
GDM_PROVLIB_SHARED_LIBS=libMcClient


MOBICORE_DIR_INC := $(MC_INCLUDE_DIR) $(MOBICORE_PROJECT_PATH)/common/curl/include
ifeq ($(wildcard external/curl/Android.mk),)
include $(MOBICORE_PROJECT_PATH)/common/curl/Android.mk
endif
include $(MOBICORE_PROJECT_PATH)/tlcm/Android.mk
include $(MOBICORE_PROJECT_PATH)/rootpa/Code/Common/Android.mk
include $(MOBICORE_PROJECT_PATH)/rootpa/Code/Android/app/jni/Android.mk
include $(MOBICORE_PROJECT_PATH)/rootpa/Code/Android/lib/Android.mk
include $(MOBICORE_PROJECT_PATH)/rootpa/Code/Android/app/Android.mk

# Include the TuiService
ifneq ($(wildcard $(MOBICORE_PROJECT_PATH)/TuiService/Android.mk),)
include $(MOBICORE_PROJECT_PATH)/TuiService/Android.mk
include $(MOBICORE_PROJECT_PATH)/TuiService/jni/Android.mk
endif

# Include TlcTeeKeymaster
ifneq ($(wildcard $(MOBICORE_PROJECT_PATH)/TlcTeeKeymaster/Android.mk),)
include $(MOBICORE_PROJECT_PATH)/TlcTeeKeymaster/Android.mk
endif

