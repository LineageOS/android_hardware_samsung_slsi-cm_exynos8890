# Copyright (C) 2012 The Android Open Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

ifeq ($(TARGET_BOOTLOADER_BOARD_NAME), xyref5433)

LOCAL_PATH:= $(call my-dir)

#################
# libexynoscamera

include $(CLEAR_VARS)

LOCAL_PRELINK_MODULE := false

LOCAL_SHARED_LIBRARIES:= libutils libcutils libbinder liblog libcamera_client libhardware
LOCAL_SHARED_LIBRARIES += libexynosutils libhwjpeg libexynosv4l2 libexynosgscaler libion_exynos libcsc
LOCAL_SHARED_LIBRARIES += libexpat libstlport
LOCAL_SHARED_LIBRARIES += libpower

LOCAL_CFLAGS += -DGAIA_FW_BETA
LOCAL_CFLAGS += -DMAIN_CAMERA_SENSOR_NAME=$(BOARD_BACK_CAMERA_SENSOR)
LOCAL_CFLAGS += -DFRONT_CAMERA_SENSOR_NAME=$(BOARD_FRONT_CAMERA_SENSOR)
ifeq ($(BOARD_CAMERA_DISPLAY_WQHD), true)
	LOCAL_CFLAGS += -DCAMERA_DISPLAY_WQHD
endif
LOCAL_CFLAGS += -DUSE_CAMERA_ESD_RESET
LOCAL_CFLAGS += -DBACK_ROTATION=$(BOARD_BACK_CAMERA_ROTATION)
LOCAL_CFLAGS += -DFRONT_ROTATION=$(BOARD_FRONT_CAMERA_ROTATION)

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../libcamera \
	$(TOP)/hardware/samsung_slsi-cm/exynos/libcamera/54xx \
	$(TOP)/hardware/samsung_slsi-cm/exynos/libcamera/54xx/JpegEncoderForCamera \
	$(TOP)/hardware/samsung_slsi-cm/exynos/libcamera/common \
	$(TOP)/hardware/samsung_slsi-cm/exynos/libcamera/common/Pipes \
	$(TOP)/hardware/samsung_slsi-cm/exynos/libcamera/common/Activities \
	$(TOP)/hardware/samsung_slsi-cm/exynos/libcamera/common/Buffers \
	$(LOCAL_PATH)/../libcamera/Vendor \
	$(TOP)/hardware/samsung_slsi-cm/exynos/include \
	$(TOP)/hardware/samsung_slsi-cm/$(TARGET_SOC)/include \
	$(TOP)/hardware/samsung_slsi-cm/$(TARGET_BOARD_PLATFORM)/include \
	$(TOP)/hardware/samsung_slsi-cm/$(TARGET_BOARD_PLATFORM)/libcamera \
	$(TOP)/hardware/libhardware_legacy/include/hardware_legacy \
	$(TOP)/vendor/samsung/feature/CscFeature/libsecnativefeature \
	$(TOP)/bionic \
	$(TOP)/external/expat/lib \
	$(TOP)/external/stlport/stlport

LOCAL_SRC_FILES:= \
	ExynosCameraSensorInfo.cpp \
	../../exynos/libcamera/common/ExynosCameraFrame.cpp \
	../../exynos/libcamera/common/ExynosCameraMemory.cpp \
	../../exynos/libcamera/common/ExynosCameraUtils.cpp \
	../../exynos/libcamera/common/ExynosCameraNode.cpp \
	../../exynos/libcamera/common/ExynosCameraFrameSelector.cpp \
	../../exynos/libcamera/common/Pipes/ExynosCameraPipe.cpp \
	../../exynos/libcamera/common/Pipes/ExynosCameraPipeFlite.cpp \
	../../exynos/libcamera/common/Pipes/ExynosCameraPipe3AA_ISP.cpp \
	../../exynos/libcamera/common/Pipes/ExynosCameraPipeSCC.cpp \
	../../exynos/libcamera/common/Pipes/ExynosCameraPipeSCP.cpp \
	../../exynos/libcamera/common/Pipes/ExynosCameraPipeGSC.cpp \
	../../exynos/libcamera/common/Pipes/ExynosCameraPipeJpeg.cpp \
	../../exynos/libcamera/common/Pipes/ExynosCameraPipe3AA.cpp \
	../../exynos/libcamera/common/Pipes/ExynosCameraPipe3AC.cpp \
	../../exynos/libcamera/common/Pipes/ExynosCameraPipeISP.cpp \
	../../exynos/libcamera/common/Buffers/ExynosCameraBufferManager.cpp \
	../../exynos/libcamera/common/Buffers/ExynosCameraBufferLocker.cpp \
	../../exynos/libcamera/common/Activities/ExynosCameraActivityBase.cpp \
	../../exynos/libcamera/common/Activities/ExynosCameraActivityAutofocus.cpp \
	../../exynos/libcamera/common/Activities/ExynosCameraActivityFlash.cpp \
	../../exynos/libcamera/common/Activities/ExynosCameraActivitySpecialCapture.cpp \
	../../exynos/libcamera/common/Activities/ExynosCameraActivityUCTL.cpp \
	../../exynos/libcamera/54xx/JpegEncoderForCamera/ExynosJpegEncoderForCamera.cpp \
	../../exynos/libcamera/54xx/ExynosCamera.cpp \
	../../exynos/libcamera/54xx/ExynosCameraParameters.cpp \
	../../exynos/libcamera/54xx/ExynosCameraFrameFactory.cpp \
	../../exynos/libcamera/54xx/ExynosCameraFrameReprocessingFactory.cpp \
	../../exynos/libcamera/54xx/ExynosCameraFrameFactoryFront.cpp \
	../../exynos/libcamera/54xx/ExynosCameraActivityControl.cpp\
	../../exynos/libcamera/54xx/ExynosCameraUtilsModule.cpp \
	../../exynos/libcamera/54xx/ExynosCameraScalableSensor.cpp

LOCAL_MODULE_TAGS := optional
LOCAL_MODULE := libexynoscamera

include $(BUILD_SHARED_LIBRARY)

#################
# libcamera

include $(CLEAR_VARS)

# HAL module implemenation stored in
# hw/<COPYPIX_HARDWARE_MODULE_ID>.<ro.product.board>.so
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw

LOCAL_C_INCLUDES += \
	$(LOCAL_PATH)/../include \
	$(LOCAL_PATH)/../libcamera \
	$(LOCAL_PATH)/../libcamera/Vendor \
	$(TOP)/hardware/samsung_slsi-cm/exynos/libcamera/54xx \
	$(TOP)/hardware/samsung_slsi-cm/exynos/libcamera/54xx/JpegEncoderForCamera \
	$(TOP)/hardware/samsung_slsi-cm/exynos/libcamera/common \
	$(TOP)/hardware/samsung_slsi-cm/exynos/libcamera/common/Pipes \
	$(TOP)/hardware/samsung_slsi-cm/exynos/libcamera/common/Activities \
	$(TOP)/hardware/samsung_slsi-cm/exynos/libcamera/common/Buffers \
	$(TOP)/hardware/samsung_slsi-cm/exynos/include \
	$(TOP)/hardware/samsung_slsi-cm/$(TARGET_SOC)/include \
	$(TOP)/hardware/samsung_slsi-cm/$(TARGET_BOARD_PLATFORM)/include \
	$(TOP)/hardware/samsung_slsi-cm/$(TARGET_BOARD_PLATFORM)/libcamera \
	frameworks/native/include \
	system/media/camera/include

LOCAL_SRC_FILES:= \
	../../exynos/libcamera/common/ExynosCameraInterface.cpp

LOCAL_CFLAGS += -DGAIA_FW_BETA
LOCAL_CFLAGS += -DBACK_ROTATION=$(BOARD_BACK_CAMERA_ROTATION)
LOCAL_CFLAGS += -DFRONT_ROTATION=$(BOARD_FRONT_CAMERA_ROTATION)

LOCAL_SHARED_LIBRARIES:= libutils libcutils libbinder liblog libcamera_client libhardware
LOCAL_SHARED_LIBRARIES += libexynosutils libhwjpeg libexynosv4l2 libcsc libion_exynos libexynoscamera

LOCAL_MODULE := camera.$(TARGET_BOOTLOADER_BOARD_NAME)

LOCAL_MODULE_TAGS := optional

include $(BUILD_SHARED_LIBRARY)

endif
