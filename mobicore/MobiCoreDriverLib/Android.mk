# =============================================================================
#
# MobiCore Android build components
#
# =============================================================================

LOCAL_PATH := $(call my-dir)

# Only compile proxy support for Android M (use stlport absence as indicator)
use_proxy =
ifeq ($(wildcard external/stlport/libstlport.mk),)
	use_proxy = yes
endif

# Client Library
# =============================================================================
include $(CLEAR_VARS)
LOCAL_MODULE := libMcClient
LOCAL_MODULE_TAGS := debug eng optional
LOCAL_C_INCLUDES += $(GLOBAL_INCLUDES)
LOCAL_SHARED_LIBRARIES += $(GLOBAL_LIBRARIES)

LOCAL_CFLAGS := -fvisibility=hidden -fvisibility-inlines-hidden
LOCAL_CFLAGS += -include buildTag.h
LOCAL_CFLAGS += -DLOG_TAG=\"McClient\"
LOCAL_CFLAGS += -DTBASE_API_LEVEL=5
LOCAL_CFLAGS += -std=c++11
ifdef use_proxy
LOCAL_CFLAGS += -DGOOGLE_PROTOBUF_NO_RTTI
else # use_proxy
LOCAL_CFLAGS += -DWITHOUT_PROXY
endif # !use_proxy
LOCAL_CFLAGS += -DLOG_D=LOG_I

LOCAL_SRC_FILES := \
	ClientLib/Device.cpp \
	ClientLib/ClientLib.cpp \
	ClientLib/Session.cpp \
	ClientLib/driver_client.cpp \
	Common/CMutex.cpp \
	Common/Connection.cpp \
	ClientLib/Contrib/common_client.cpp \
	ClientLib/Contrib/mc_client_api.cpp \
	ClientLib/Contrib/tee_client_api.cpp

ifdef use_proxy
LOCAL_SRC_FILES += \
	ClientLib/Contrib/proxy_client.cpp \
	ClientLib/mc.pb.cpp
endif # use_proxy

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/Common \
	$(LOCAL_PATH)/ClientLib \
	$(LOCAL_PATH)/ClientLib/Contrib \
	$(LOCAL_PATH)/ClientLib/public \
	$(LOCAL_PATH)/ClientLib/public/GP \
	$(MOBICORE_PROJECT_PATH)/include/GPD_TEE_Internal_API \
	$(MOBICORE_PROJECT_PATH)/include/public \
	$(COMP_PATH_MobiCore)/inc \
	$(COMP_PATH_MobiCore)/inc/McLib

ifdef use_proxy
LOCAL_C_INCLUDES += \
	external/protobuf/src

LOCAL_SHARED_LIBRARIES += \
	libprotobuf-cpp-lite
endif # use_proxy

LOCAL_EXPORT_C_INCLUDE_DIRS += \
	$(COMP_PATH_MobiCore)/inc \
	$(LOCAL_PATH)/ClientLib/public

include $(LOCAL_PATH)/Kernel/Android.mk
# Import logwrapper
include $(COMP_PATH_Logwrapper)/Android.mk

include $(BUILD_SHARED_LIBRARY)

ifdef use_proxy

# Proxy server lib
# =============================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := libMcProxy
LOCAL_MODULE_TAGS := eng

LOCAL_CFLAGS := -fvisibility=hidden
LOCAL_CFLAGS += -DLOG_TAG=\"McProxy\"
LOCAL_CFLAGS += -DTBASE_API_LEVEL=5
LOCAL_CFLAGS += -Wall -Wextra
LOCAL_CFLAGS += -std=c++11
LOCAL_CFLAGS += -DLOG_ANDROID
LOCAL_CFLAGS += -DGOOGLE_PROTOBUF_NO_RTTI
LOCAL_CFLAGS += -DLOG_D=LOG_I

LOCAL_C_INCLUDES := $(GLOBAL_INCLUDES) \
	$(LOCAL_PATH)/Common \
	$(LOCAL_PATH)/ClientLib \
	$(LOCAL_PATH)/ClientLib/Contrib \
	$(LOCAL_PATH)/ClientLib/public \
	$(LOCAL_PATH)/ClientLib/public/GP \
	$(COMP_PATH_MobiCore_Src_Code)/McLib/public \
	external/protobuf/src

LOCAL_SHARED_LIBRARIES += \
	libprotobuf-cpp-lite

LOCAL_SRC_FILES := \
	ClientLib/driver_client.cpp \
	ClientLib/mc.pb.cpp \
	ClientLib/Contrib/proxy_server.cpp

LOCAL_EXPORT_CFLAGS := -DLOG_ANDROID
LOCAL_EXPORT_C_INCLUDES := $(LOCAL_C_INCLUDES)
LOCAL_EXPORT_C_INCLUDE_DIRS := $(LOCAL_EXPORT_C_INCLUDES)

include $(LOCAL_PATH)/Kernel/Android.mk
# Import logwrapper
include $(COMP_PATH_Logwrapper)/Android.mk

include $(BUILD_STATIC_LIBRARY)

endif # use_proxy

# Daemon Application
# =============================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := mcDriverDaemon
LOCAL_MODULE_TAGS := debug eng optional
LOCAL_CFLAGS += -include buildTag.h
LOCAL_CFLAGS += -DLOG_TAG=\"McDaemon\"
LOCAL_CFLAGS += -DTBASE_API_LEVEL=3
ifndef use_proxy
LOCAL_CFLAGS += -DWITHOUT_PROXY
endif

LOCAL_C_INCLUDES += $(GLOBAL_INCLUDES)
LOCAL_SHARED_LIBRARIES += $(GLOBAL_LIBRARIES) libMcClient libMcRegistry

ifdef use_proxy
LOCAL_STATIC_LIBRARIES := \
	libMcProxy

LOCAL_SHARED_LIBRARIES += \
	libprotobuf-cpp-lite
endif # use_proxy

include $(LOCAL_PATH)/Daemon/Android.mk

# Common Source files required for building the daemon
LOCAL_SRC_FILES += \
	Common/CMutex.cpp \
	Common/Connection.cpp \
	Common/NetlinkConnection.cpp \
	Common/CSemaphore.cpp \
	Common/CThread.cpp \
	Registry/PrivateRegistry.cpp

# Includes required for the Daemon
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/Common \
	$(LOCAL_PATH)/common/MobiCore/inc \
	$(LOCAL_PATH)/ClientLib/public \
	$(LOCAL_PATH)/ClientLib/public/GP \
	$(MOBICORE_PROJECT_PATH)/include/public \
	$(COMP_PATH_MobiCore)/inc \
	$(COMP_PATH_MobiCore)/inc/McLib

# Private Registry components
LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/Registry/Public \
	$(LOCAL_PATH)/Registry

# Common components
include $(LOCAL_PATH)/Kernel/Android.mk
# Logwrapper
include $(COMP_PATH_Logwrapper)/Android.mk

include $(BUILD_EXECUTABLE)

# Registry Shared Library
# =============================================================================
include $(CLEAR_VARS)

LOCAL_MODULE := libMcRegistry
LOCAL_MODULE_TAGS := debug eng optional
LOCAL_CFLAGS := -DLOG_TAG=\"McRegistry\"
LOCAL_C_INCLUDES += $(GLOBAL_INCLUDES)
LOCAL_SHARED_LIBRARIES += $(GLOBAL_LIBRARIES)

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/Common \
	$(LOCAL_PATH)/Daemon/public \
	$(LOCAL_PATH)/ClientLib/public

# Common Source files required for building the daemon
LOCAL_SRC_FILES += \
	Common/CMutex.cpp \
	Common/Connection.cpp \
	Common/CSemaphore.cpp

include $(LOCAL_PATH)/Registry/Android.mk

# Import logwrapper
include $(COMP_PATH_Logwrapper)/Android.mk

include $(BUILD_SHARED_LIBRARY)
