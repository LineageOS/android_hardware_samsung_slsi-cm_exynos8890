/*
 * Copyright 2013, Samsung Electronics Co. LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed toggle an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/*!
 * \file      ExynosCameraConfig.h
 * \brief     hearder file for ExynosCameraConfig
 * \author    Pilsun Jang(pilsun.jang@samsung.com)
 * \date      2013/7/15
 *
 */

#ifndef EXYNOS_CAMERA_CONFIG_H__
#define EXYNOS_CAMERA_CONFIG_H__

#include <math.h>

#define BUILD_DATE()   ALOGE("Build Date is (%s) (%s) ", __DATE__, __TIME__)
#define WHERE_AM_I()   ALOGE("[(%s)%d] ", __FUNCTION__, __LINE__)
#define LOG_DELAY()    usleep(100000)

#define TARGET_ANDROID_VER_MAJ 4
#define TARGET_ANDROID_VER_MIN 4

#define XPaste(s) s
#define Paste2(a, b) XPaste(a)b
#define ID "[%s]-"
#define ID_PARM m_name

#define CLOGD(fmt, ...) \
    ALOGD(Paste2(ID, fmt), ID_PARM, ##__VA_ARGS__)

#define CLOGV(fmt, ...) \
    ALOGV(Paste2(ID, fmt), ID_PARM, ##__VA_ARGS__)

#define CLOGW(fmt, ...) \
    ALOGW(Paste2(ID, fmt), ID_PARM, ##__VA_ARGS__)

#define CLOGE(fmt, ...) \
    ALOGE(Paste2(ID, fmt), ID_PARM, ##__VA_ARGS__)

#define CLOGI(fmt, ...) \
    ALOGI(Paste2(ID, fmt), ID_PARM, ##__VA_ARGS__)

#define CLOGT(cnt, fmt, ...) \
    if (cnt != 0) CLOGI(Paste2("#TRACE#", fmt), ##__VA_ARGS__) \

#define CLOG_ASSERT(fmt, ...) \
    android_printAssert(NULL, LOG_TAG, Paste2(ID, fmt), ID_PARM, ##__VA_ARGS__);

#define ROUND_UP(x, a)              (((x) + ((a)-1)) / (a) * (a))
#define ROUND_OFF_HALF(x, dig)      ((float)(floor((x) * pow(10.0f, dig) + 0.5) / pow(10.0f, dig)))

#define CAMERA_ISP_ALIGN                (8)
#define CAMERA_MAGIC_ALIGN              (16)
/* This value for GSC alignment refer to "csc.h" */
#define GSCALER_IMG_ALIGN               (16)

#define INITIAL_SKIP_FRAME              (8)
#define EFFECT_SKIP_FRAME               (1)
#define SMART_STAY_SKIP_COMPENSATION    (1)

/* Functions tobe enable */
/* #define START_PICTURE_THREAD */
/* #define SET_SETFILE_BY_SHOT */

#define SET_SETFILE_BY_SET_CTRL
#ifdef SET_SETFILE_BY_SET_CTRL
#define SET_SETFILE_BY_SET_CTRL_3AA_ISP (true)
#define SET_SETFILE_BY_SET_CTRL_ISP     (true)
#define SET_SETFILE_BY_SET_CTRL_SCP     (true)
#endif

/* #define USE_DVFS_LOCK */
/* #define SENSOR_NAME_GET_FROM_FILE */
/* #define RESERVED_MEMORY_ENABLE */
#define RESERVED_BUFFER_COUNT_MAX       (5)
/* #define USE_BNS_RECORDING */
/* #define FPS_CHECK */
#define USE_FD_AE
#define FD_ROTATION                     (true)

#define SHOT_RECOVERY                   (true)
#ifdef SHOT_RECOVERY
#define SHOT_RECOVERY_COUNT             (0)
#else
#define SHOT_RECOVERY_COUNT             (0)
#endif

#define USE_ADAPTIVE_CSC_RECORDING      (false)
#define USE_HIGHSPEED_RECORDING         (false)

#define USE_FASTEN_AE_STABLE            (false)
#define FASTEN_AE_WIDTH                 (1312)
#define FASTEN_AE_HEIGHT                (738)
#define FASTEN_AE_FPS                   (120)
#define DEFAULT_BNS_RATIO               (2)
#define USE_CAMERA_SIZE_TABLE           (true)

#define USE_PURE_BAYER_REPROCESSING     (true)

/* This USE_DYNAMIC_BAYER define is for default scenario.
 * See <ExynosCameraParameter.cpp> for details of dynamic bayer setting
 */
#define USE_DYNAMIC_BAYER               (false)

enum REPROCESSING_BAYER_MODE {
    REPROCESSING_BAYER_MODE_NONE            = 0, /* This means capture do not use reprocessing */
    REPROCESSING_BAYER_MODE_PURE_ALWAYS_ON,
    REPROCESSING_BAYER_MODE_DIRTY_ALWAYS_ON,
    REPROCESSING_BAYER_MODE_PURE_DYNAMIC,
    REPROCESSING_BAYER_MODE_DIRTY_DYNAMIC,
    REPROCESSING_BAYER_MODE_MAX,
};

#define USE_DYNAMIC_SCC_REAR            (false)
#define USE_DYNAMIC_SCC_FRONT           (false)

#define USE_GSC_FOR_CAPTURE_BACK        (false)
#define USE_GSC_FOR_CAPTURE_FRONT       (true)

#define MAX_SERIES_SHOT_COUNT           (21)

#ifdef SENSOR_NAME_GET_FROM_FILE
#define SENSOR_NAME_PATH_BACK "vendor specifics"
#define SSENSOR_NAME_PATH_BACK "vendor specifics"
#endif

#define EXYNOS_CAMERA_NAME_STR_SIZE (256)
#define CAMERA_PACKED_BAYER_ENABLE
#define USE_BUFFER_WITH_STRIDE

#define MAIN_CAMERA_FLITE_NUM       FIMC_IS_VIDEO_SS0_NUM
#define MAIN_CAMERA_3AA_NUM         FIMC_IS_VIDEO_3A0_NUM
#define MAIN_CAMERA_3AP_NUM         FIMC_IS_VIDEO_3A0P_NUM
#define MAIN_CAMERA_3AC_NUM         FIMC_IS_VIDEO_3A0C_NUM
#define MAIN_CAMERA_3AA_OTF         (1)
#define MAIN_CAMERA_REPROCESSING    (true)
#define MAIN_CAMERA_SCC_CAPTURE     (false)

#define FRONT_CAMERA_FLITE_NUM      FIMC_IS_VIDEO_SS1_NUM
#define FRONT_CAMERA_3AA_NUM        FIMC_IS_VIDEO_3A1_NUM
#define FRONT_CAMERA_3AP_NUM        FIMC_IS_VIDEO_3A1P_NUM
#define FRONT_CAMERA_3AA_OTF        (0)
#define FRONT_CAMERA_REPROCESSING   (false)
#define FRONT_CAMERA_SCC_CAPTURE    (true)

#define REPROCESSING_3AA_NUM        FIMC_IS_VIDEO_3A1_NUM

#define REPROCESSING_SHIFT          (28)
#define OTF_3AA_SHIFT               (24)
#define SSX_VINDEX_SHIFT            (16)
#define TAX_VINDEX_SHIFT            (8)

#define SENSOR_NOBLOCK_SHIFT        (28)
#define SENSOR_INSTANT_SHIFT        (16)
#define SENSOR_SSTREAM_SHIFT        (0)

#define NODE_PREFIX "/dev/video"

#define PREVIEW_GSC_NODE_NUM            (4)  /* 4 = MSC from Exynos5420 */
#define PICTURE_GSC_NODE_NUM            (5)  /* 0,1,2 = GSC */
#define VIDEO_GSC_NODE_NUM              (4)

#define MAX_BUFFERS                     (32)
#define NUM_BAYER_BUFFERS               (6 + REPROCESSING_BAYER_HOLD_COUNT + SHOT_RECOVERY_COUNT)
#define INIT_BAYER_BUFFERS              (5 + SHOT_RECOVERY_COUNT)
#define NUM_PREVIEW_BUFFERS             (9 + SHOT_RECOVERY_COUNT)
#define NUM_PREVIEW_SPARE_BUFFERS       (3)
#define NUM_PICTURE_BUFFERS             NUM_BAYER_BUFFERS
#define NUM_REPROCESSING_BUFFERS        (1)
#define NUM_RECORDING_BUFFERS           (8)
#define NUM_FASTAESTABLE_BUFFER         (10)
#define NUM_PREVIEW_BUFFERS_MARGIN      (2)
#define NUM_FRAME_PREPARE_COUNT         (6)
#define NUM_BURST_GSC_JPEG_INIT_BUFFER  (4) /* Number of pre-allicated buffer for burst shot
                                               Increasing this number will increase takePicture()'s
                                               response time. Currently it is defined as
                                               (# of JPEG save thread) + 1 */

/* TO DO : will remove */
#define REPROCESSING_BAYER_HOLD_COUNT   (1)
#define FRONT_NUM_BAYER_BUFFERS         (6)
#define FRONT_NUM_PICTURE_BUFFERS       FRONT_NUM_BAYER_BUFFERS

#define MFC_7X_BUFFER_OFFSET            (256)

#define V4L2_CAMERA_MEMORY_TYPE         (V4L2_MEMORY_DMABUF) /* (V4L2_MEMORY_USERPTR) */
#define JPEG_INPUT_COLOR_FMT            (V4L2_PIX_FMT_YUYV)
#define CAMERA_BAYER_FORMAT             (V4L2_PIX_FMT_SBGGR12)

#define ERROR_POLLING_DETECTED          (-1001)
#define ERROR_DQ_BLOCKED_DETECTED       (-1002)
#define ERROR_DQ_BLOCKED_COUNT          (20)
#define WARNING_3AA_THREAD_INTERVAL     (100000)
#define WARNING_SCP_THREAD_INTERVAL     (100000)
#define MONITOR_THREAD_INTERVAL         (200000)

#define FRAME_MIN_NUM (3)

#define EXYNOS_CAMERA_BUFFER_MAX_PLANES (4)     /* img buffer 3 + metadata 1 */
#define EXYNOS_CAMERA_META_PLANE_SIZE   (32 * 1024)
#define GRALLOC_LOCK_FOR_CAMERA         (GRALLOC_SET_USAGE_FOR_CAMERA)

#define EXYNOS_CAMERA_PREVIEW_FPS_REFERENCE  (60)

#define  NUM_OF_DETECTED_FACES           (16)
#define  NUM_OF_DETECTED_FACES_THRESHOLD (0)

#define PERFRAME_NODE_GROUP_MAX          (3)
#define PERFRAME_NODE_GROUP_0            (0)
#define PERFRAME_NODE_GROUP_1            (1)
#define PERFRAME_NODE_GROUP_2            (2)

#define PERFRAME_INFO_INDEX_MAX          (3)
#define PERFRAME_INFO_INDEX_0            (0)
#define PERFRAME_INFO_INDEX_1            (1)
#define PERFRAME_INFO_INDEX_2            (2)

#define PERFRAME_CONTROL_NODE_3AA
/* #define PERFRAME_CONTROL_NODE_ISP */
#define PERFRAME_INFO_3AA                       PERFRAME_INFO_INDEX_0
#define PERFRAME_INFO_ISP                       PERFRAME_INFO_INDEX_1
#define PERFRAME_INFO_DIRTY_REPROCESSING_ISP    PERFRAME_INFO_INDEX_0

#define PERFRAME_INFO_PURE_REPROCESSING_3AA     PERFRAME_INFO_INDEX_0
#define PERFRAME_INFO_PURE_REPROCESSING_ISP     PERFRAME_INFO_INDEX_1

#define PERFRAME_3AA_POS                        PER_FRAME_GROUP_0
#define PERFRAME_ISP_POS                        PER_FRAME_GROUP_1

#define PERFRAME_BACK_3AC_POS           (0)
#define PERFRAME_BACK_3AP_POS           (1)
#define PERFRAME_BACK_SCC_POS           (0)
#define PERFRAME_BACK_SCP_POS           (0)

#define PERFRAME_REPROCESSING_3AP_POS   (0)
#define PERFRAME_REPROCESSING_SCC_POS   (0)
#define PERFRAME_FRONT_3AP_POS          (0)
#define PERFRAME_FRONT_SCC_POS          (0)
#define PERFRAME_FRONT_SCP_POS          (1)

#define WAITING_TIME                     (5000)   /* 5msec */
#define TOTAL_WAITING_TIME               (3000 * 1000)  /* 3000msec */
#define TOTAL_WAITING_COUNT              (3)
#define TOTAL_FLASH_WATING_COUNT         (10)

#define DM_WAITING_TIME                  (30 * 1000) /* 30msec */
#define DM_WAITING_COUNT                 (10)

/* callback state */
#define CALLBACK_STATE_PREVIEW_META     (1)
#define CALLBACK_STATE_PREVIEW_FRAME    (2)
#define CALLBACK_STATE_RAW_IMAGE        (3)
#define CALLBACK_STATE_POSTVIEW_FRAME   (4)
#define CALLBACK_STATE_COMPRESSED_IMAGE (5)

#define GRALLOC_SET_USAGE_FOR_CAMERA \
    (GRALLOC_USAGE_SW_READ_OFTEN | \
     GRALLOC_USAGE_SW_WRITE_OFTEN | \
     GRALLOC_USAGE_HW_TEXTURE | \
     GRALLOC_USAGE_HW_COMPOSER | \
     GRALLOC_USAGE_EXTERNAL_DISP)

enum YUV_RANGE {
    YUV_FULL_RANGE = 0,
    YUV_LIMITED_RANGE = 1,
};

enum pipeline {
    PIPE_FLITE                  = 0,
    PIPE_3AC,
    PIPE_ISP,
    PIPE_3AA_ISP,
    PIPE_SCC,
    PIPE_SCP,
    PIPE_GSC,
    PIPE_GSC_VIDEO,
    PIPE_GSC_PICTURE,
    PIPE_JPEG,        
    MAX_PIPE_NUM,

    PIPE_FLITE_FRONT            = 100,
    PIPE_3AA_FRONT,
    PIPE_3AC_FRONT,
    PIPE_ISP_FRONT,
    PIPE_SCC_FRONT,
    PIPE_SCP_FRONT,
    PIPE_GSC_FRONT,
    PIPE_GSC_PICTURE_FRONT,
    PIPE_GSC_VIDEO_FRONT,
    PIPE_JPEG_FRONT,
    MAX_PIPE_NUM_FRONT,

    PIPE_3AA_REPROCESSING       = 200,
    PIPE_3AC_REPROCESSING,
    PIPE_ISP_REPROCESSING,
    PIPE_SCC_REPROCESSING,
    PIPE_SCP_REPROCESSING,
    PIPE_GSC_REPROCESSING,
    PIPE_JPEG_REPROCESSING,
    MAX_PIPE_NUM_REPROCESSING
};

enum fimc_is_video_dev_num {
    FIMC_IS_VIDEO_SS0_NUM               = 100,
    FIMC_IS_VIDEO_SS1_NUM               = 101,
    FIMC_IS_VIDEO_SS2_NUM               = 102,
    FIMC_IS_VIDEO_SS3_NUM               = 103,
    FIMC_IS_VIDEO_3A0_NUM               = 110,
    FIMC_IS_VIDEO_3A0C_NUM              = 111,
    FIMC_IS_VIDEO_3A0P_NUM              = 112,
    FIMC_IS_VIDEO_3A1_NUM               = 114,
    FIMC_IS_VIDEO_3A1C_NUM              = 115,
    FIMC_IS_VIDEO_3A1P_NUM              = 116,
    FIMC_IS_VIDEO_ISP_NUM               = 130,
    FIMC_IS_VIDEO_SCC_NUM               = 134,
    FIMC_IS_VIDEO_SCP_NUM               = 137,
    FIMC_IS_VIDEO_VDC_NUM               = 140,
    FIMC_IS_VIDEO_VDO_NUM               = 141,
    FIMC_IS_VIDEO_FD_NUM                = 146,
    FIMC_IS_VIDEO_MAX_NUM               = 149,
};

typedef enum
{
    SENSOR_NAME_NOTHING             = 0,
    SENSOR_NAME_S5K3H2              = 1, // 1 ~ 100, SLSI sensors
    SENSOR_NAME_S5K6A3              = 2,
    SENSOR_NAME_S5K3H5              = 3,
    SENSOR_NAME_S5K3H7              = 4,
    SENSOR_NAME_S5K3H7_SUNNY        = 5,
    SENSOR_NAME_S5K3H7_SUNNY_2M     = 6,
    SENSOR_NAME_S5K6B2              = 7,
    SENSOR_NAME_S5K3L2              = 8,
    SENSOR_NAME_S5K4E5              = 9,
    SENSOR_NAME_S5K2P2              = 10,
    SENSOR_NAME_S5K8B1              = 11,
    SENSOR_NAME_S5K1P2              = 12,

    SENSOR_NAME_IMX135              = 101, // 101 ~ 200 Sony sensors

    SENSOR_NAME_SR261               = 201, // 201 ~ 300 Other vendor sensors

    SENSOR_NAME_END,
    SENSOR_NAME_CUSTOM              = 301,
/* HACK: Remove this define after Driver commonized */
    SENSOR_NAME_IMX134,
    SENSOR_NAME_S5K4H5,
    SENSOR_NAME_IMX175,
/* End of HACK */
}IS_SensorNameEnum;


/* This struct used in recording callback */
/* This struct passes to OMX */
struct addrs {
    uint32_t        type;       /* make sure that this is 4 byte. */
    unsigned int    fdPlaneY;
    unsigned int    fdPlaneCbcr;
    unsigned int    bufIndex;
    unsigned int    reserved;
};

/* for test */
/* #define SCALABLE_ON */
/* #define TEST_GED_HIGH_SPEED_RECORDING */
/* #define TEST_APP_HIGH_SPEED_RECORDING */ /* for ArtCamera */

#endif /* EXYNOS_CAMERA_CONFIG_H__ */
