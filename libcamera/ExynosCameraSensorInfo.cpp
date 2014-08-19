/*
**
** Copyright 2013, Samsung Electronics Co. LTD
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

/*#define LOG_NDEBUG 0 */
#define LOG_TAG "ExynosCameraUtils"
#include <cutils/log.h>

#include "ExynosCameraSensorInfo.h"

namespace android {

#ifdef SENSOR_NAME_GET_FROM_FILE
int g_rearSensorId = -1;
int g_frontSensorId = -1;
#endif

struct ExynosSensorInfo *createSensorInfo(int camId)
{
    struct ExynosSensorInfo *sensorInfo = NULL;
    int sensorName = getSensorId(camId);
    if (sensorName < 0) {
        ALOGE("ERR(%s[%d]): Inavalid camId, sensor name is nothing", __FUNCTION__, __LINE__);
        sensorName = SENSOR_NAME_NOTHING;
    }

    switch (sensorName) {
    case SENSOR_NAME_S5K6B2:
        sensorInfo = new ExynosSensorS5K6B2();
        break;
    case SENSOR_NAME_S5K3L2:
        sensorInfo = new ExynosSensorS5K3L2();
        break;
    case SENSOR_NAME_S5K2P2:
        sensorInfo = new ExynosSensorS5K2P2();
        break;
    case SENSOR_NAME_IMX135:
        sensorInfo = new ExynosSensorIMX135();
        break;
    case SENSOR_NAME_IMX134:
        sensorInfo = new ExynosSensorIMX134();
        break;
    case SENSOR_NAME_S5K6A3:
        sensorInfo = new ExynosSensorS5K6A3();
        break;
    case SENSOR_NAME_S5K3H5:
        sensorInfo = new ExynosSensorS5K3H5();
        break;
    case SENSOR_NAME_S5K4H5:
        sensorInfo = new ExynosSensorS5K4H5();
        break;
    case SENSOR_NAME_S5K3H7:
    case SENSOR_NAME_S5K3H7_SUNNY:
        sensorInfo = new ExynosSensorS5K3H7();
        break;
    case SENSOR_NAME_IMX175:
        sensorInfo = new ExynosSensorIMX175();
        break;
    case SENSOR_NAME_S5K8B1:
        sensorInfo = new ExynosSensorS5K8B1();
        break;
    default:
        ALOGW("WRN(%s[%d]): Unknown sensor, create default sensor", __FUNCTION__, __LINE__);
        sensorInfo = new ExynosSensorInfo();
        break;
    }

    return sensorInfo;
}

bool needGSCForCapture(int camId)
{
    return (camId == CAMERA_ID_BACK) ? USE_GSC_FOR_CAPTURE_BACK : USE_GSC_FOR_CAPTURE_FRONT;
}

int getSensorId(int camId)
{
    int sensorId = -1;

#ifdef SENSOR_NAME_GET_FROM_FILE
    int &curSensorId = (camId == CAMERA_ID_BACK) ? g_rearSensorId : g_frontSensorId;

    if (curSensorId < 0) {
        curSensorId = getSensorIdFromFile(camId);
        if (curSensorId < 0) {
            ALOGE("ERR(%s): invalid sensor ID %d", __FUNCTION__, sensorId);
        }
    }

    sensorId = curSensorId;
#else
    if (camId == CAMERA_ID_BACK) {
        sensorId = MAIN_CAMERA_SENSOR_NAME;
    } else if (camId == CAMERA_ID_FRONT) {
        sensorId = FRONT_CAMERA_SENSOR_NAME;
    } else {
        ALOGE("ERR(%s):Unknown camera ID(%d)", __FUNCTION__, camId);
    }
#endif

    return sensorId;
}

ExynosSensorInfo::ExynosSensorInfo()
{
    maxPreviewW = 1920;
    maxPreviewH = 1080;
    maxPictureW = 4128;
    maxPictureH = 3096;
    maxVideoW = 1920;
    maxVideoH = 1080;
    maxSensorW = 4128;
    maxSensorH = 3096;
    maxThumbnailW = 512;
    maxThumbnailH = 384;

    fNumberNum = 22;
    fNumberDen = 10;
    focalLengthNum = 420;
    focalLengthDen = 100;
    focusDistanceNum = 0;
    focusDistanceDen = 0;
    apertureNum = 227;
    apertureDen = 100;
    horizontalViewAngle = 51.2f;
    verticalViewAngle = 39.4f;
    focalLengthIn35mmLength = 31;

    minExposureCompensation = -4;
    maxExposureCompensation = 4;
    exposureCompensationStep = 0.5f;
    maxNumDetectedFaces = 16;
    maxNumFocusAreas = 1;
    maxNumMeteringAreas = 1;
    maxZoomLevel = ZOOM_LEVEL_MAX;
    maxZoomRatio = 400;

    zoomSupport = true;
    smoothZoomSupport = false;
    videoSnapshotSupport = true;
    videoStabilizationSupport = true;
    autoWhiteBalanceLockSupport = true;
    autoExposureLockSupport = true;

    antiBandingList =
          ANTIBANDING_AUTO
        | ANTIBANDING_50HZ
        | ANTIBANDING_60HZ
        | ANTIBANDING_OFF
        ;

    effectList =
          EFFECT_NONE
        | EFFECT_MONO
        | EFFECT_NEGATIVE
        | EFFECT_SOLARIZE
        | EFFECT_SEPIA
        | EFFECT_POSTERIZE
        | EFFECT_WHITEBOARD
        | EFFECT_BLACKBOARD
        | EFFECT_AQUA;

    flashModeList =
          FLASH_MODE_OFF
        | FLASH_MODE_AUTO
        | FLASH_MODE_ON
        | FLASH_MODE_RED_EYE
        | FLASH_MODE_TORCH;

    focusModeList =
          FOCUS_MODE_AUTO
        | FOCUS_MODE_INFINITY
        | FOCUS_MODE_MACRO
        | FOCUS_MODE_FIXED
        | FOCUS_MODE_EDOF
        | FOCUS_MODE_CONTINUOUS_VIDEO
        | FOCUS_MODE_CONTINUOUS_PICTURE
        | FOCUS_MODE_TOUCH;

    sceneModeList =
          SCENE_MODE_AUTO
        | SCENE_MODE_ACTION
        | SCENE_MODE_PORTRAIT
        | SCENE_MODE_LANDSCAPE
        | SCENE_MODE_NIGHT
        | SCENE_MODE_NIGHT_PORTRAIT
        | SCENE_MODE_THEATRE
        | SCENE_MODE_BEACH
        | SCENE_MODE_SNOW
        | SCENE_MODE_SUNSET
        | SCENE_MODE_STEADYPHOTO
        | SCENE_MODE_FIREWORKS
        | SCENE_MODE_SPORTS
        | SCENE_MODE_PARTY
        | SCENE_MODE_CANDLELIGHT;

    whiteBalanceList =
          WHITE_BALANCE_AUTO
        | WHITE_BALANCE_INCANDESCENT
        | WHITE_BALANCE_FLUORESCENT
        | WHITE_BALANCE_WARM_FLUORESCENT
        | WHITE_BALANCE_DAYLIGHT
        | WHITE_BALANCE_CLOUDY_DAYLIGHT
        | WHITE_BALANCE_TWILIGHT
        | WHITE_BALANCE_SHADE;

    previewSizeLutMax     = 0;
    pictureSizeLutMax     = 0;
    videoSizeLutMax       = 0;
    previewSizeLut        = NULL;
    pictureSizeLut        = NULL;
    videoSizeLut          = NULL;
    videoSizeLutHighSpeed = NULL;
    sizeTableSupport      = false;

    /* vendor specifics */
    highResolutionCallbackW = 3264;
    highResolutionCallbackH = 1836;
    highSpeedRecording60WFHD = 1920;
    highSpeedRecording60HFHD = 1080;
    highSpeedRecording60W = 1008;
    highSpeedRecording60H = 566;
    highSpeedRecording120W = 1008;
    highSpeedRecording120H = 566;
    scalableSensorSupport = true;
    bnsSupport = false;
    minFps = 0;
    maxFps = 30;
}

ExynosSensorIMX135::ExynosSensorIMX135()
{
    maxPreviewW = 1920;
    maxPreviewH = 1080;
    maxPictureW = 4128;
    maxPictureH = 3096;
    maxVideoW = 1920;
    maxVideoH = 1080;
    maxSensorW = 4128;
    maxSensorH = 3096;
    maxThumbnailW = 512;
    maxThumbnailH = 384;

    fNumberNum = 22;
    fNumberDen = 10;
    focalLengthNum = 420;
    focalLengthDen = 100;
    focusDistanceNum = 0;
    focusDistanceDen = 0;
    apertureNum = 227;
    apertureDen = 100;
    horizontalViewAngle = 51.2f;
    verticalViewAngle = 39.4f;
    focalLengthIn35mmLength = 31;

    minFps = 1;
    maxFps = 30;

    minExposureCompensation = -4;
    maxExposureCompensation = 4;
    exposureCompensationStep = 0.5f;
    maxNumDetectedFaces = 16;
    maxNumFocusAreas = 2;
    maxNumMeteringAreas = 32;
    maxZoomLevel = ZOOM_LEVEL_MAX;
    maxZoomRatio = 400;

    zoomSupport = true;
    smoothZoomSupport = false;
    videoSnapshotSupport = true;
    videoStabilizationSupport = true;
    autoWhiteBalanceLockSupport = true;
    autoExposureLockSupport = true;

    antiBandingList =
          ANTIBANDING_AUTO
        | ANTIBANDING_50HZ
        | ANTIBANDING_60HZ
        | ANTIBANDING_OFF
        ;

    effectList =
          EFFECT_NONE
        | EFFECT_MONO
        | EFFECT_NEGATIVE
        /*| EFFECT_SOLARIZE*/
        | EFFECT_SEPIA
        | EFFECT_POSTERIZE
        /*| EFFECT_WHITEBOARD*/
        /*| EFFECT_BLACKBOARD*/
        | EFFECT_AQUA
        ;

    flashModeList =
          FLASH_MODE_OFF
        | FLASH_MODE_AUTO
        | FLASH_MODE_ON
        /*| FLASH_MODE_RED_EYE*/
        | FLASH_MODE_TORCH;

    focusModeList =
          FOCUS_MODE_AUTO
        /*| FOCUS_MODE_INFINITY*/
        | FOCUS_MODE_MACRO
        /*| FOCUS_MODE_FIXED*/
        /*| FOCUS_MODE_EDOF*/
        | FOCUS_MODE_CONTINUOUS_VIDEO
        | FOCUS_MODE_CONTINUOUS_PICTURE
        | FOCUS_MODE_TOUCH;

    sceneModeList =
          SCENE_MODE_AUTO
        | SCENE_MODE_ACTION
        | SCENE_MODE_PORTRAIT
        | SCENE_MODE_LANDSCAPE
        | SCENE_MODE_NIGHT
        | SCENE_MODE_NIGHT_PORTRAIT
        | SCENE_MODE_THEATRE
        | SCENE_MODE_BEACH
        | SCENE_MODE_SNOW
        | SCENE_MODE_SUNSET
        | SCENE_MODE_STEADYPHOTO
        | SCENE_MODE_FIREWORKS
        | SCENE_MODE_SPORTS
        | SCENE_MODE_PARTY
        | SCENE_MODE_CANDLELIGHT;

    whiteBalanceList =
          WHITE_BALANCE_AUTO
        | WHITE_BALANCE_INCANDESCENT
        | WHITE_BALANCE_FLUORESCENT
        /*| WHITE_BALANCE_WARM_FLUORESCENT*/
        | WHITE_BALANCE_DAYLIGHT
        | WHITE_BALANCE_CLOUDY_DAYLIGHT
        /*| WHITE_BALANCE_TWILIGHT*/
        /*| WHITE_BALANCE_SHADE*/
        ;

    previewSizeLutMax     = 0;
    pictureSizeLutMax     = 0;
    videoSizeLutMax       = 0;
    previewSizeLut        = NULL;
    pictureSizeLut        = NULL;
    videoSizeLut          = NULL;
    videoSizeLutHighSpeed = NULL;
    sizeTableSupport      = false;

    /* vendor specifics */
    highResolutionCallbackW = 3264;
    highResolutionCallbackH = 1836;
    highSpeedRecording60WFHD = 1920;
    highSpeedRecording60HFHD = 1080;
    highSpeedRecording60W = 1008;
    highSpeedRecording60H = 566;
    highSpeedRecording120W = 1008;
    highSpeedRecording120H = 566;
    scalableSensorSupport = true;
    bnsSupport = false;
};

ExynosSensorIMX134::ExynosSensorIMX134()
{
    maxPreviewW = 1920;
    maxPreviewH = 1080;
    maxPictureW = 3264;
    maxPictureH = 2448;
    maxVideoW = 1920;
    maxVideoH = 1080;
    maxSensorW = 3264;
    maxSensorH = 2448;

    maxThumbnailW = 512;
    maxThumbnailH = 384;

    fNumberNum = 22;
    fNumberDen = 10;
    focalLengthNum = 420;
    focalLengthDen = 100;
    focusDistanceNum = 0;
    focusDistanceDen = 0;
    apertureNum = 227;
    apertureDen = 100;
    horizontalViewAngle = 51.2f;
    verticalViewAngle = 39.4f;
    focalLengthIn35mmLength = 31;

    minFps = 1;
    maxFps = 30;

    minExposureCompensation = -4;
    maxExposureCompensation = 4;
    exposureCompensationStep = 0.5f;
    maxNumDetectedFaces = 16;
    maxNumFocusAreas = 2;
    maxNumMeteringAreas = 32;
    maxZoomLevel = ZOOM_LEVEL_MAX;
    maxZoomRatio = 400;

    zoomSupport = true;
    smoothZoomSupport = false;
    videoSnapshotSupport = true;
    videoStabilizationSupport = false;
    autoWhiteBalanceLockSupport = false;
    autoExposureLockSupport = true;

    antiBandingList =
          ANTIBANDING_AUTO
        | ANTIBANDING_50HZ
        | ANTIBANDING_60HZ
        | ANTIBANDING_OFF
        ;

    effectList =
          EFFECT_NONE
        | EFFECT_MONO
        | EFFECT_NEGATIVE
        /*| EFFECT_SOLARIZE*/
        | EFFECT_SEPIA
        | EFFECT_POSTERIZE
        /*| EFFECT_WHITEBOARD*/
        /*| EFFECT_BLACKBOARD*/
        | EFFECT_AQUA
        ;

    flashModeList =
          FLASH_MODE_OFF
        | FLASH_MODE_AUTO
        | FLASH_MODE_ON
        /*| FLASH_MODE_RED_EYE*/
        | FLASH_MODE_TORCH;

    focusModeList =
          FOCUS_MODE_AUTO
        /*| FOCUS_MODE_INFINITY*/
        | FOCUS_MODE_MACRO
        /*| FOCUS_MODE_FIXED*/
        /*| FOCUS_MODE_EDOF*/
        | FOCUS_MODE_CONTINUOUS_VIDEO
        | FOCUS_MODE_CONTINUOUS_PICTURE
        | FOCUS_MODE_TOUCH;

    sceneModeList =
          SCENE_MODE_AUTO
        | SCENE_MODE_ACTION
        | SCENE_MODE_PORTRAIT
        | SCENE_MODE_LANDSCAPE
        | SCENE_MODE_NIGHT
        | SCENE_MODE_NIGHT_PORTRAIT
        | SCENE_MODE_THEATRE
        | SCENE_MODE_BEACH
        | SCENE_MODE_SNOW
        | SCENE_MODE_SUNSET
        | SCENE_MODE_STEADYPHOTO
        | SCENE_MODE_FIREWORKS
        | SCENE_MODE_SPORTS
        | SCENE_MODE_PARTY
        | SCENE_MODE_CANDLELIGHT;

    whiteBalanceList =
          WHITE_BALANCE_AUTO
        | WHITE_BALANCE_INCANDESCENT
        | WHITE_BALANCE_FLUORESCENT
        /*| WHITE_BALANCE_WARM_FLUORESCENT*/
        | WHITE_BALANCE_DAYLIGHT
        | WHITE_BALANCE_CLOUDY_DAYLIGHT
        /*| WHITE_BALANCE_TWILIGHT*/
        /*| WHITE_BALANCE_SHADE*/
        ;

    previewSizeLutMax     = 0;
    pictureSizeLutMax     = 0;
    videoSizeLutMax       = 0;
    previewSizeLut        = NULL;
    pictureSizeLut        = NULL;
    videoSizeLut          = NULL;
    videoSizeLutHighSpeed = NULL;
    sizeTableSupport      = false;

    /* vendor specifics */
    /*
    burstPanoramaW = 3264;
    burstPanoramaH = 1836;
    highSpeedRecording60WFHD = 1920;
    highSpeedRecording60HFHD = 1080;
    highSpeedRecording60W = 1008;
    highSpeedRecording60H = 566;
    highSpeedRecording120W = 1008;
    highSpeedRecording120H = 566;
    scalableSensorSupport = true;
    */
    bnsSupport = false;
};

ExynosSensorS5K3L2::ExynosSensorS5K3L2()
{
    maxPreviewW = 1920;
    maxPreviewH = 1080;
    maxPictureW = 4128;
    maxPictureH = 3096;
    maxVideoW = 1920;
    maxVideoH = 1080;
    maxSensorW = 4128;
    maxSensorH = 3096;
    maxThumbnailW = 512;
    maxThumbnailH = 384;

    fNumberNum = 22;
    fNumberDen = 10;
    focalLengthNum = 420;
    focalLengthDen = 100;
    focusDistanceNum = 0;
    focusDistanceDen = 0;
    apertureNum = 227;
    apertureDen = 100;
    horizontalViewAngle = 51.2f;
    verticalViewAngle = 39.4f;
    focalLengthIn35mmLength = 31;

    minFps = 1;
    maxFps = 30;

    minExposureCompensation = -4;
    maxExposureCompensation = 4;
    exposureCompensationStep = 0.5f;
    maxNumDetectedFaces = 16;
    maxNumFocusAreas = 2;
    maxNumMeteringAreas = 32;
    maxZoomLevel = ZOOM_LEVEL_MAX;
    maxZoomRatio = 400;

    zoomSupport = true;
    smoothZoomSupport = false;
    videoSnapshotSupport = true;
    videoStabilizationSupport = true;
    autoWhiteBalanceLockSupport = true;
    autoExposureLockSupport = true;

    antiBandingList =
          ANTIBANDING_AUTO
        | ANTIBANDING_50HZ
        | ANTIBANDING_60HZ
        | ANTIBANDING_OFF
        ;

    effectList =
          EFFECT_NONE
        | EFFECT_MONO
        | EFFECT_NEGATIVE
        /*| EFFECT_SOLARIZE*/
        | EFFECT_SEPIA
        | EFFECT_POSTERIZE
        /*| EFFECT_WHITEBOARD*/
        /*| EFFECT_BLACKBOARD*/
        | EFFECT_AQUA
        ;

    flashModeList =
          FLASH_MODE_OFF
        | FLASH_MODE_AUTO
        | FLASH_MODE_ON
        /*| FLASH_MODE_RED_EYE*/
        | FLASH_MODE_TORCH;

    focusModeList =
          FOCUS_MODE_AUTO
        /*| FOCUS_MODE_INFINITY*/
        | FOCUS_MODE_MACRO
        /*| FOCUS_MODE_FIXED*/
        /*| FOCUS_MODE_EDOF*/
        | FOCUS_MODE_CONTINUOUS_VIDEO
        | FOCUS_MODE_CONTINUOUS_PICTURE
        | FOCUS_MODE_TOUCH;

    sceneModeList =
          SCENE_MODE_AUTO
        | SCENE_MODE_ACTION
        | SCENE_MODE_PORTRAIT
        | SCENE_MODE_LANDSCAPE
        | SCENE_MODE_NIGHT
        | SCENE_MODE_NIGHT_PORTRAIT
        | SCENE_MODE_THEATRE
        | SCENE_MODE_BEACH
        | SCENE_MODE_SNOW
        | SCENE_MODE_SUNSET
        | SCENE_MODE_STEADYPHOTO
        | SCENE_MODE_FIREWORKS
        | SCENE_MODE_SPORTS
        | SCENE_MODE_PARTY
        | SCENE_MODE_CANDLELIGHT;

    whiteBalanceList =
          WHITE_BALANCE_AUTO
        | WHITE_BALANCE_INCANDESCENT
        | WHITE_BALANCE_FLUORESCENT
        /*| WHITE_BALANCE_WARM_FLUORESCENT*/
        | WHITE_BALANCE_DAYLIGHT
        | WHITE_BALANCE_CLOUDY_DAYLIGHT
        /*| WHITE_BALANCE_TWILIGHT*/
        /*| WHITE_BALANCE_SHADE*/
        ;

    /* vendor specifics */
    highResolutionCallbackW = 3264;
    highResolutionCallbackH = 1836;
    highSpeedRecording60WFHD = 1920;
    highSpeedRecording60HFHD = 1080;
    highSpeedRecording60W = 2056;
    highSpeedRecording60H = 1152;
    highSpeedRecording120W = 1024;
    highSpeedRecording120H = 574;
    scalableSensorSupport = true;
    bnsSupport = false;

    if (bnsSupport == true) {
        previewSizeLutMax = sizeof(PREVIEW_SIZE_LUT_3L2_BNS) / (sizeof(int) * SIZE_OF_LUT);
        videoSizeLutMax   = sizeof(VIDEO_SIZE_LUT_3L2_BNS)   / (sizeof(int) * SIZE_OF_LUT);
        previewSizeLut    = PREVIEW_SIZE_LUT_3L2_BNS;
        videoSizeLut      = VIDEO_SIZE_LUT_3L2_BNS;
    } else {
        previewSizeLutMax = sizeof(PREVIEW_SIZE_LUT_3L2) / (sizeof(int) * SIZE_OF_LUT);
        videoSizeLutMax   = sizeof(VIDEO_SIZE_LUT_3L2)   / (sizeof(int) * SIZE_OF_LUT);
        previewSizeLut    = PREVIEW_SIZE_LUT_3L2;
        videoSizeLut      = VIDEO_SIZE_LUT_3L2;
    }
    pictureSizeLutMax     = sizeof(PICTURE_SIZE_LUT_3L2) / (sizeof(int) * SIZE_OF_LUT);
    pictureSizeLut        = PICTURE_SIZE_LUT_3L2;
    videoSizeLutHighSpeed = VIDEO_SIZE_LUT_HIGH_SPEED_3L2;
    sizeTableSupport = true;
};

ExynosSensorS5K2P2::ExynosSensorS5K2P2()
{
    maxPreviewW = 3840;
    maxPreviewH = 2160;
    maxPictureW = 5312;
    maxPictureH = 2988;
    maxVideoW = 3840;
    maxVideoH = 2160;
    maxSensorW = 5312;
    maxSensorH = 2990;
    maxThumbnailW = 512;
    maxThumbnailH = 384;

    fNumberNum = 22;
    fNumberDen = 10;
    focalLengthNum = 420;
    focalLengthDen = 100;
    focusDistanceNum = 0;
    focusDistanceDen = 0;
    apertureNum = 227;
    apertureDen = 100;
    horizontalViewAngle = 51.2f;
    verticalViewAngle = 39.4f;
    focalLengthIn35mmLength = 31;

    minFps = 1;
    maxFps = 30;

    minExposureCompensation = -4;
    maxExposureCompensation = 4;
    exposureCompensationStep = 0.5f;
    maxNumDetectedFaces = 16;
    maxNumFocusAreas = 2;
    maxNumMeteringAreas = 32;
    maxZoomLevel = ZOOM_LEVEL_MAX;
    maxZoomRatio = 400;

    zoomSupport = true;
    smoothZoomSupport = false;
    videoSnapshotSupport = true;
    videoStabilizationSupport = true;
    autoWhiteBalanceLockSupport = true;
    autoExposureLockSupport = true;

    antiBandingList =
          ANTIBANDING_AUTO
        | ANTIBANDING_50HZ
        | ANTIBANDING_60HZ
        | ANTIBANDING_OFF
        ;

    effectList =
          EFFECT_NONE
        | EFFECT_MONO
        | EFFECT_NEGATIVE
        /*| EFFECT_SOLARIZE*/
        | EFFECT_SEPIA
        | EFFECT_POSTERIZE
        /*| EFFECT_WHITEBOARD*/
        /*| EFFECT_BLACKBOARD*/
        | EFFECT_AQUA
        ;

    flashModeList =
          FLASH_MODE_OFF
        | FLASH_MODE_AUTO
        | FLASH_MODE_ON
        //| FLASH_MODE_RED_EYE
        | FLASH_MODE_TORCH;

    focusModeList =
          FOCUS_MODE_AUTO
        //| FOCUS_MODE_INFINITY
        | FOCUS_MODE_MACRO
        //| FOCUS_MODE_FIXED
        //| FOCUS_MODE_EDOF
        | FOCUS_MODE_CONTINUOUS_VIDEO
        | FOCUS_MODE_CONTINUOUS_PICTURE
        | FOCUS_MODE_TOUCH;

    sceneModeList =
          SCENE_MODE_AUTO
        | SCENE_MODE_ACTION
        | SCENE_MODE_PORTRAIT
        | SCENE_MODE_LANDSCAPE
        | SCENE_MODE_NIGHT
        | SCENE_MODE_NIGHT_PORTRAIT
        | SCENE_MODE_THEATRE
        | SCENE_MODE_BEACH
        | SCENE_MODE_SNOW
        | SCENE_MODE_SUNSET
        | SCENE_MODE_STEADYPHOTO
        | SCENE_MODE_FIREWORKS
        | SCENE_MODE_SPORTS
        | SCENE_MODE_PARTY
        | SCENE_MODE_CANDLELIGHT;

    whiteBalanceList =
          WHITE_BALANCE_AUTO
        | WHITE_BALANCE_INCANDESCENT
        | WHITE_BALANCE_FLUORESCENT
        //| WHITE_BALANCE_WARM_FLUORESCENT
        | WHITE_BALANCE_DAYLIGHT
        | WHITE_BALANCE_CLOUDY_DAYLIGHT
        //| WHITE_BALANCE_TWILIGHT
        //| WHITE_BALANCE_SHADE
        ;

    /* vendor specifics */
    highResolutionCallbackW = 3264;
    highResolutionCallbackH = 1836;
    highSpeedRecording60WFHD = 1920;
    highSpeedRecording60HFHD = 1080;
    highSpeedRecording60W = 1008;
    highSpeedRecording60H = 566;
    highSpeedRecording120W = 1008;
    highSpeedRecording120H = 566;
    scalableSensorSupport = true;
    bnsSupport = true;

    if (bnsSupport == true) {
        previewSizeLutMax     = sizeof(PREVIEW_SIZE_LUT_2P2_BNS) / (sizeof(int) * SIZE_OF_LUT);
        videoSizeLutMax       = sizeof(VIDEO_SIZE_LUT_2P2_BNS)   / (sizeof(int) * SIZE_OF_LUT);
        pictureSizeLutMax     = sizeof(PICTURE_SIZE_LUT_2P2)     / (sizeof(int) * SIZE_OF_LUT);
        previewSizeLut        = PREVIEW_SIZE_LUT_2P2_BNS;
        videoSizeLut          = VIDEO_SIZE_LUT_2P2_BNS;
        pictureSizeLut        = PICTURE_SIZE_LUT_2P2;
        videoSizeLutHighSpeed = VIDEO_SIZE_LUT_HIGH_SPEED_2P2_BNS;
        sizeTableSupport      = true;
    } else {
        previewSizeLutMax     = 0;
        pictureSizeLutMax     = 0;
        videoSizeLutMax       = 0;
        previewSizeLut        = NULL;
        pictureSizeLut        = NULL;
        videoSizeLut          = NULL;
        videoSizeLutHighSpeed = NULL;
        sizeTableSupport      = false;
    }
};

ExynosSensorS5K6B2::ExynosSensorS5K6B2()
{
    maxPreviewW = 1920;
    maxPreviewH = 1080;
    maxPictureW = 1920;
    maxPictureH = 1080;
    maxVideoW = 1920;
    maxVideoH = 1080;
    maxSensorW = 1920;
    maxSensorH = 1080;
    maxThumbnailW = 512;
    maxThumbnailH = 384;

    fNumberNum = 22;
    fNumberDen = 10;
    focalLengthNum = 420;
    focalLengthDen = 100;
    focusDistanceNum = 0;
    focusDistanceDen = 0;
    apertureNum = 227;
    apertureDen = 100;
    horizontalViewAngle = 51.2f;
    verticalViewAngle = 39.4f;
    focalLengthIn35mmLength = 31;

    minFps = 1;
    maxFps = 30;

    minExposureCompensation = -4;
    maxExposureCompensation = 4;
    exposureCompensationStep = 0.5f;
    maxNumDetectedFaces = 16;
    maxNumFocusAreas = 1;
    maxNumMeteringAreas = 32;
    maxZoomLevel = ZOOM_LEVEL_MAX;
    maxZoomRatio = 400;

    zoomSupport = true;
    smoothZoomSupport = false;
    videoSnapshotSupport = true;
    videoStabilizationSupport = true;
    autoWhiteBalanceLockSupport = true;
    autoExposureLockSupport = true;

    antiBandingList =
          ANTIBANDING_AUTO
        | ANTIBANDING_50HZ
        | ANTIBANDING_60HZ
        | ANTIBANDING_OFF
        ;

    effectList =
          EFFECT_NONE
        | EFFECT_MONO
        | EFFECT_NEGATIVE
        /*| EFFECT_SOLARIZE*/
        | EFFECT_SEPIA
        | EFFECT_POSTERIZE
        /*| EFFECT_WHITEBOARD*/
        /*| EFFECT_BLACKBOARD*/
        | EFFECT_AQUA
        ;

    flashModeList =
          FLASH_MODE_OFF
        /*| FLASH_MODE_AUTO*/
        /*| FLASH_MODE_ON*/
        /*| FLASH_MODE_RED_EYE*/
        /*| FLASH_MODE_TORCH*/
        ;

    focusModeList =
        /* FOCUS_MODE_AUTO*/
        FOCUS_MODE_INFINITY
        /*| FOCUS_MODE_MACRO*/
        /*| FOCUS_MODE_FIXED*/
        /*| FOCUS_MODE_EDOF*/
        /*| FOCUS_MODE_CONTINUOUS_VIDEO*/
        /*| FOCUS_MODE_CONTINUOUS_PICTURE*/
        /*| FOCUS_MODE_TOUCH*/
        ;

    sceneModeList =
          SCENE_MODE_AUTO
        | SCENE_MODE_ACTION
        | SCENE_MODE_PORTRAIT
        | SCENE_MODE_LANDSCAPE
        | SCENE_MODE_NIGHT
        | SCENE_MODE_NIGHT_PORTRAIT
        | SCENE_MODE_THEATRE
        | SCENE_MODE_BEACH
        | SCENE_MODE_SNOW
        | SCENE_MODE_SUNSET
        | SCENE_MODE_STEADYPHOTO
        | SCENE_MODE_FIREWORKS
        | SCENE_MODE_SPORTS
        | SCENE_MODE_PARTY
        | SCENE_MODE_CANDLELIGHT;

    whiteBalanceList =
          WHITE_BALANCE_AUTO
        | WHITE_BALANCE_INCANDESCENT
        | WHITE_BALANCE_FLUORESCENT
        /* WHITE_BALANCE_WARM_FLUORESCENT*/
        | WHITE_BALANCE_DAYLIGHT
        | WHITE_BALANCE_CLOUDY_DAYLIGHT
        /* WHITE_BALANCE_TWILIGHT*/
        /* WHITE_BALANCE_SHADE*/
        ;

    previewSizeLutMax     = 0;
    pictureSizeLutMax     = 0;
    videoSizeLutMax       = 0;
    previewSizeLut        = NULL;
    pictureSizeLut        = NULL;
    videoSizeLut          = NULL;
    videoSizeLutHighSpeed = NULL;
    sizeTableSupport      = false;

    /* vendor specifics */
    highResolutionCallbackW = 3264;
    highResolutionCallbackH = 1836;
    highSpeedRecording60WFHD = 1920;
    highSpeedRecording60HFHD = 1080;
    highSpeedRecording60W = 1008;
    highSpeedRecording60H = 566;
    highSpeedRecording120W = 1008;
    highSpeedRecording120H = 566;
    scalableSensorSupport = true;
    bnsSupport = false;
};


ExynosSensorS5K3H7::ExynosSensorS5K3H7()
{
    maxPreviewW = 1920;
    maxPreviewH = 1080;
    maxPictureW = 3248;
    maxPictureH = 2438;
    maxVideoW = 1920;
    maxVideoH = 1080;
    maxSensorW = 3248;
    maxSensorH = 2438;

    maxThumbnailW = 512;
    maxThumbnailH = 384;

    fNumberNum = 22;
    fNumberDen = 10;
    focalLengthNum = 420;
    focalLengthDen = 100;
    focusDistanceNum = 0;
    focusDistanceDen = 0;
    apertureNum = 227;
    apertureDen = 100;
    horizontalViewAngle = 51.2f;
    verticalViewAngle = 39.4f;
    focalLengthIn35mmLength = 31;

    minFps = 1;
    maxFps = 30;

    minExposureCompensation = -4;
    maxExposureCompensation = 4;
    exposureCompensationStep = 0.5f;
    maxNumDetectedFaces = 16;
    maxNumFocusAreas = 2;
    maxNumMeteringAreas = 32;
    maxZoomLevel = ZOOM_LEVEL_MAX;
    maxZoomRatio = 400;

    zoomSupport = true;
    smoothZoomSupport = false;
    videoSnapshotSupport = true;
    videoStabilizationSupport = false;
    autoWhiteBalanceLockSupport = true;
    autoExposureLockSupport = true;

    antiBandingList =
          ANTIBANDING_AUTO
        | ANTIBANDING_50HZ
        | ANTIBANDING_60HZ
        | ANTIBANDING_OFF
        ;

    effectList =
          EFFECT_NONE
        | EFFECT_MONO
        | EFFECT_NEGATIVE
        /*| EFFECT_SOLARIZE*/
        | EFFECT_SEPIA
        | EFFECT_POSTERIZE
        /*| EFFECT_WHITEBOARD*/
        /*| EFFECT_BLACKBOARD*/
        | EFFECT_AQUA
        ;

    flashModeList =
          FLASH_MODE_OFF
        | FLASH_MODE_AUTO
        | FLASH_MODE_ON
        /*| FLASH_MODE_RED_EYE*/
        | FLASH_MODE_TORCH;

    focusModeList =
          FOCUS_MODE_AUTO
        /*| FOCUS_MODE_INFINITY*/
        | FOCUS_MODE_MACRO
        /*| FOCUS_MODE_FIXED*/
        /*| FOCUS_MODE_EDOF*/
        | FOCUS_MODE_CONTINUOUS_VIDEO
        | FOCUS_MODE_CONTINUOUS_PICTURE
        | FOCUS_MODE_TOUCH;

    sceneModeList =
          SCENE_MODE_AUTO
        | SCENE_MODE_ACTION
        | SCENE_MODE_PORTRAIT
        | SCENE_MODE_LANDSCAPE
        | SCENE_MODE_NIGHT
        | SCENE_MODE_NIGHT_PORTRAIT
        | SCENE_MODE_THEATRE
        | SCENE_MODE_BEACH
        | SCENE_MODE_SNOW
        | SCENE_MODE_SUNSET
        | SCENE_MODE_STEADYPHOTO
        | SCENE_MODE_FIREWORKS
        | SCENE_MODE_SPORTS
        | SCENE_MODE_PARTY
        | SCENE_MODE_CANDLELIGHT;

    whiteBalanceList =
          WHITE_BALANCE_AUTO
        | WHITE_BALANCE_INCANDESCENT
        | WHITE_BALANCE_FLUORESCENT
        /*| WHITE_BALANCE_WARM_FLUORESCENT*/
        | WHITE_BALANCE_DAYLIGHT
        | WHITE_BALANCE_CLOUDY_DAYLIGHT
        /*| WHITE_BALANCE_TWILIGHT*/
        /*| WHITE_BALANCE_SHADE*/
        ;

    /* vendor specifics */
    /*
    burstPanoramaW = 3264;
    burstPanoramaH = 1836;
    highSpeedRecording60WFHD = 1920;
    highSpeedRecording60HFHD = 1080;
    highSpeedRecording60W = 1008;
    highSpeedRecording60H = 566;
    highSpeedRecording120W = 1008;
    highSpeedRecording120H = 566;
    scalableSensorSupport = true;
    */
    bnsSupport = false;

    if (bnsSupport == true) {
        previewSizeLutMax     = 0;
        pictureSizeLutMax     = 0;
        videoSizeLutMax       = 0;
        previewSizeLut        = NULL;
        pictureSizeLut        = NULL;
        videoSizeLut          = NULL;
        videoSizeLutHighSpeed = NULL;
        sizeTableSupport      = false;
    } else {
        previewSizeLutMax     = sizeof(PREVIEW_SIZE_LUT_3H7) / (sizeof(int) * SIZE_OF_LUT);
        videoSizeLutMax       = sizeof(VIDEO_SIZE_LUT_3H7)   / (sizeof(int) * SIZE_OF_LUT);
        pictureSizeLutMax     = sizeof(PICTURE_SIZE_LUT_3H7) / (sizeof(int) * SIZE_OF_LUT);
        previewSizeLut        = PREVIEW_SIZE_LUT_3H7;
        videoSizeLut          = VIDEO_SIZE_LUT_3H7;
        pictureSizeLut        = PICTURE_SIZE_LUT_3H7;
        videoSizeLutHighSpeed = VIDEO_SIZE_LUT_HIGH_SPEED_3H7;
        sizeTableSupport      = true;
    }
};

ExynosSensorS5K3H5::ExynosSensorS5K3H5()
{
    maxPreviewW = 1920;
    maxPreviewH = 1080;
    maxPictureW = 3248;
    maxPictureH = 2438;
    maxVideoW = 1920;
    maxVideoH = 1080;
    maxSensorW = 3248;
    maxSensorH = 2438;

    maxThumbnailW = 512;
    maxThumbnailH = 384;

    fNumberNum = 22;
    fNumberDen = 10;
    focalLengthNum = 420;
    focalLengthDen = 100;
    focusDistanceNum = 0;
    focusDistanceDen = 0;
    apertureNum = 227;
    apertureDen = 100;
    horizontalViewAngle = 51.2f;
    verticalViewAngle = 39.4f;
    focalLengthIn35mmLength = 31;

    minFps = 1;
    maxFps = 30;

    minExposureCompensation = -4;
    maxExposureCompensation = 4;
    exposureCompensationStep = 0.5f;
    maxNumDetectedFaces = 16;
    maxNumFocusAreas = 2;
    maxNumMeteringAreas = 32;
    maxZoomLevel = ZOOM_LEVEL_MAX;
    maxZoomRatio = 400;

    zoomSupport = true;
    smoothZoomSupport = false;
    videoSnapshotSupport = true;
    videoStabilizationSupport = false;
    autoWhiteBalanceLockSupport = true;
    autoExposureLockSupport = true;

    antiBandingList =
          ANTIBANDING_AUTO
        | ANTIBANDING_50HZ
        | ANTIBANDING_60HZ
        | ANTIBANDING_OFF
        ;

    effectList =
          EFFECT_NONE
        | EFFECT_MONO
        | EFFECT_NEGATIVE
        /*| EFFECT_SOLARIZE*/
        | EFFECT_SEPIA
        | EFFECT_POSTERIZE
        /*| EFFECT_WHITEBOARD*/
        /*| EFFECT_BLACKBOARD*/
        | EFFECT_AQUA
        ;

    flashModeList =
          FLASH_MODE_OFF
        | FLASH_MODE_AUTO
        | FLASH_MODE_ON
        /*| FLASH_MODE_RED_EYE*/
        | FLASH_MODE_TORCH;

    focusModeList =
          FOCUS_MODE_AUTO
        /*| FOCUS_MODE_INFINITY*/
        | FOCUS_MODE_MACRO
        /*| FOCUS_MODE_FIXED*/
        /*| FOCUS_MODE_EDOF*/
        | FOCUS_MODE_CONTINUOUS_VIDEO
        | FOCUS_MODE_CONTINUOUS_PICTURE
        | FOCUS_MODE_TOUCH;

    sceneModeList =
          SCENE_MODE_AUTO
        | SCENE_MODE_ACTION
        | SCENE_MODE_PORTRAIT
        | SCENE_MODE_LANDSCAPE
        | SCENE_MODE_NIGHT
        | SCENE_MODE_NIGHT_PORTRAIT
        | SCENE_MODE_THEATRE
        | SCENE_MODE_BEACH
        | SCENE_MODE_SNOW
        | SCENE_MODE_SUNSET
        | SCENE_MODE_STEADYPHOTO
        | SCENE_MODE_FIREWORKS
        | SCENE_MODE_SPORTS
        | SCENE_MODE_PARTY
        | SCENE_MODE_CANDLELIGHT;

    whiteBalanceList =
          WHITE_BALANCE_AUTO
        | WHITE_BALANCE_INCANDESCENT
        | WHITE_BALANCE_FLUORESCENT
        /*| WHITE_BALANCE_WARM_FLUORESCENT*/
        | WHITE_BALANCE_DAYLIGHT
        | WHITE_BALANCE_CLOUDY_DAYLIGHT
        /*| WHITE_BALANCE_TWILIGHT*/
        /*| WHITE_BALANCE_SHADE*/
        ;

    previewSizeLutMax     = 0;
    pictureSizeLutMax     = 0;
    videoSizeLutMax       = 0;
    previewSizeLut        = NULL;
    pictureSizeLut        = NULL;
    videoSizeLut          = NULL;
    videoSizeLutHighSpeed = NULL;
    sizeTableSupport      = false;

    /* vendor specifics */
    /*
    burstPanoramaW = 3264;
    burstPanoramaH = 1836;
    highSpeedRecording60WFHD = 1920;
    highSpeedRecording60HFHD = 1080;
    highSpeedRecording60W = 1008;
    highSpeedRecording60H = 566;
    highSpeedRecording120W = 1008;
    highSpeedRecording120H = 566;
    scalableSensorSupport = true;
    */
    bnsSupport = false;
};

ExynosSensorS5K4H5::ExynosSensorS5K4H5()
{
    maxPreviewW = 1920;
    maxPreviewH = 1080;
    maxPictureW = 3264;
    maxPictureH = 2448;
    maxVideoW = 1920;
    maxVideoH = 1080;
    maxSensorW = 3264;
    maxSensorH = 2448;

    maxThumbnailW = 512;
    maxThumbnailH = 384;

    fNumberNum = 22;
    fNumberDen = 10;
    focalLengthNum = 420;
    focalLengthDen = 100;
    focusDistanceNum = 0;
    focusDistanceDen = 0;
    apertureNum = 227;
    apertureDen = 100;
    horizontalViewAngle = 51.2f;
    verticalViewAngle = 39.4f;
    focalLengthIn35mmLength = 31;

    minFps = 1;
    maxFps = 30;

    minExposureCompensation = -4;
    maxExposureCompensation = 4;
    exposureCompensationStep = 0.5f;
    maxNumDetectedFaces = 16;
    maxNumFocusAreas = 2;
    maxNumMeteringAreas = 32;
    maxZoomLevel = ZOOM_LEVEL_MAX;
    maxZoomRatio = 400;

    zoomSupport = true;
    smoothZoomSupport = false;
    videoSnapshotSupport = true;
    videoStabilizationSupport = false;
    autoWhiteBalanceLockSupport = false;
    autoExposureLockSupport = true;

    antiBandingList =
          ANTIBANDING_AUTO
        | ANTIBANDING_50HZ
        | ANTIBANDING_60HZ
        | ANTIBANDING_OFF
        ;

    effectList =
          EFFECT_NONE
        | EFFECT_MONO
        | EFFECT_NEGATIVE
        /*| EFFECT_SOLARIZE*/
        | EFFECT_SEPIA
        | EFFECT_POSTERIZE
        /*| EFFECT_WHITEBOARD*/
        /*| EFFECT_BLACKBOARD*/
        | EFFECT_AQUA
        ;

    flashModeList =
          FLASH_MODE_OFF
        | FLASH_MODE_AUTO
        | FLASH_MODE_ON
        /*| FLASH_MODE_RED_EYE*/
        | FLASH_MODE_TORCH;

    focusModeList =
          FOCUS_MODE_AUTO
        /*| FOCUS_MODE_INFINITY*/
        | FOCUS_MODE_MACRO
        /*| FOCUS_MODE_FIXED*/
        /*| FOCUS_MODE_EDOF*/
        | FOCUS_MODE_CONTINUOUS_VIDEO
        | FOCUS_MODE_CONTINUOUS_PICTURE
        | FOCUS_MODE_TOUCH;

    sceneModeList =
          SCENE_MODE_AUTO
        | SCENE_MODE_ACTION
        | SCENE_MODE_PORTRAIT
        | SCENE_MODE_LANDSCAPE
        | SCENE_MODE_NIGHT
        | SCENE_MODE_NIGHT_PORTRAIT
        | SCENE_MODE_THEATRE
        | SCENE_MODE_BEACH
        | SCENE_MODE_SNOW
        | SCENE_MODE_SUNSET
        | SCENE_MODE_STEADYPHOTO
        | SCENE_MODE_FIREWORKS
        | SCENE_MODE_SPORTS
        | SCENE_MODE_PARTY
        | SCENE_MODE_CANDLELIGHT;

    whiteBalanceList =
          WHITE_BALANCE_AUTO
        | WHITE_BALANCE_INCANDESCENT
        | WHITE_BALANCE_FLUORESCENT
        /*| WHITE_BALANCE_WARM_FLUORESCENT*/
        | WHITE_BALANCE_DAYLIGHT
        | WHITE_BALANCE_CLOUDY_DAYLIGHT
        /*| WHITE_BALANCE_TWILIGHT*/
        /*| WHITE_BALANCE_SHADE*/
        ;

    /* vendor specifics */
    /*
    burstPanoramaW = 3264;
    burstPanoramaH = 1836;
    highSpeedRecording60WFHD = 1920;
    highSpeedRecording60HFHD = 1080;
    highSpeedRecording60W = 1008;
    highSpeedRecording60H = 566;
    highSpeedRecording120W = 1008;
    highSpeedRecording120H = 566;
    scalableSensorSupport = true;
    */
    bnsSupport = false;

    if (bnsSupport == true) {
        previewSizeLutMax     = 0;
        pictureSizeLutMax     = 0;
        videoSizeLutMax       = 0;
        previewSizeLut        = NULL;
        pictureSizeLut        = NULL;
        videoSizeLut          = NULL;
        videoSizeLutHighSpeed = NULL;
        sizeTableSupport      = false;
    } else {
        previewSizeLutMax     = sizeof(PREVIEW_SIZE_LUT_4H5) / (sizeof(int) * SIZE_OF_LUT);
        videoSizeLutMax       = sizeof(VIDEO_SIZE_LUT_4H5)   / (sizeof(int) * SIZE_OF_LUT);
        pictureSizeLutMax     = sizeof(PICTURE_SIZE_LUT_4H5) / (sizeof(int) * SIZE_OF_LUT);
        previewSizeLut        = PREVIEW_SIZE_LUT_4H5;
        videoSizeLut          = VIDEO_SIZE_LUT_4H5;
        pictureSizeLut        = PICTURE_SIZE_LUT_4H5;
        videoSizeLutHighSpeed = VIDEO_SIZE_LUT_HIGH_SPEED_4H5;
        sizeTableSupport      = true;
    }
};

ExynosSensorS5K6A3::ExynosSensorS5K6A3()
{
    maxPreviewW = 1280;
    maxPreviewH = 720;
    maxPictureW = 1392;
    maxPictureH = 1402;
    maxVideoW = 1920;
    maxVideoH = 1080;
    maxSensorW = 1392;
    maxSensorH = 1402;

    maxThumbnailW = 512;
    maxThumbnailH = 384;

    fNumberNum = 22;
    fNumberDen = 10;
    focalLengthNum = 420;
    focalLengthDen = 100;
    focusDistanceNum = 0;
    focusDistanceDen = 0;
    apertureNum = 227;
    apertureDen = 100;
    horizontalViewAngle = 51.2f;
    verticalViewAngle = 39.4f;
    focalLengthIn35mmLength = 31;

    minFps = 1;
    maxFps = 30;

    minExposureCompensation = -4;
    maxExposureCompensation = 4;
    exposureCompensationStep = 0.5f;
    maxNumDetectedFaces = 16;
    maxNumFocusAreas = 2;
    maxNumMeteringAreas = 32;
    maxZoomLevel = ZOOM_LEVEL_MAX;
    maxZoomRatio = 400;

    zoomSupport = true;
    smoothZoomSupport = false;
    videoSnapshotSupport = true;
    videoStabilizationSupport = false;
    autoWhiteBalanceLockSupport = false;
    autoExposureLockSupport = true;

    antiBandingList =
          ANTIBANDING_AUTO
        | ANTIBANDING_50HZ
        | ANTIBANDING_60HZ
        | ANTIBANDING_OFF
        ;

    effectList =
          EFFECT_NONE
        | EFFECT_MONO
        | EFFECT_NEGATIVE
        /*| EFFECT_SOLARIZE*/
        | EFFECT_SEPIA
        | EFFECT_POSTERIZE
        /*| EFFECT_WHITEBOARD*/
        /*| EFFECT_BLACKBOARD*/
        | EFFECT_AQUA
        ;

    flashModeList =
          FLASH_MODE_OFF;
        /*| FLASH_MODE_AUTO*/
        /*| FLASH_MODE_ON*/
        /*| FLASH_MODE_RED_EYE*/
        /*| FLASH_MODE_TORCH;*/

    focusModeList =
        /*FOCUS_MODE_AUTO*/
        FOCUS_MODE_INFINITY;
        /*| FOCUS_MODE_INFINITY*/
        /*| FOCUS_MODE_MACRO*/
        /*| FOCUS_MODE_FIXED*/
        /*| FOCUS_MODE_EDOF*/
        /*| FOCUS_MODE_CONTINUOUS_VIDEO*/
        /*| FOCUS_MODE_CONTINUOUS_PICTURE*/
        /*| FOCUS_MODE_TOUCH*/
        ;

    sceneModeList =
          SCENE_MODE_AUTO
        | SCENE_MODE_ACTION
        | SCENE_MODE_PORTRAIT
        | SCENE_MODE_LANDSCAPE
        | SCENE_MODE_NIGHT
        | SCENE_MODE_NIGHT_PORTRAIT
        | SCENE_MODE_THEATRE
        | SCENE_MODE_BEACH
        | SCENE_MODE_SNOW
        | SCENE_MODE_SUNSET
        | SCENE_MODE_STEADYPHOTO
        | SCENE_MODE_FIREWORKS
        | SCENE_MODE_SPORTS
        | SCENE_MODE_PARTY
        | SCENE_MODE_CANDLELIGHT;

    whiteBalanceList =
          WHITE_BALANCE_AUTO
        | WHITE_BALANCE_INCANDESCENT
        | WHITE_BALANCE_FLUORESCENT
        /*| WHITE_BALANCE_WARM_FLUORESCENT*/
        | WHITE_BALANCE_DAYLIGHT
        | WHITE_BALANCE_CLOUDY_DAYLIGHT
        /*| WHITE_BALANCE_TWILIGHT*/
        /*| WHITE_BALANCE_SHADE*/
        ;

    previewSizeLutMax     = 0;
    pictureSizeLutMax     = 0;
    videoSizeLutMax       = 0;
    previewSizeLut        = NULL;
    pictureSizeLut        = NULL;
    videoSizeLut          = NULL;
    videoSizeLutHighSpeed = NULL;
    sizeTableSupport      = false;

    /* vendor specifics */
    /*
    burstPanoramaW = 3264;
    burstPanoramaH = 1836;
    highSpeedRecording60WFHD = 1920;
    highSpeedRecording60HFHD = 1080;
    highSpeedRecording60W = 1008;
    highSpeedRecording60H = 566;
    highSpeedRecording120W = 1008;
    highSpeedRecording120H = 566;
    scalableSensorSupport = true;
    */
    bnsSupport = false;
};

ExynosSensorIMX175::ExynosSensorIMX175()
{
    maxPreviewW = 1920;
    maxPreviewH = 1080;
    maxPictureW = 3264;
    maxPictureH = 2448;
    maxVideoW = 1920;
    maxVideoH = 1080;
    maxSensorW = 3264;
    maxSensorH = 2448;

    maxThumbnailW = 512;
    maxThumbnailH = 384;

    fNumberNum = 26;
    fNumberDen = 10;
    focalLengthNum = 370;
    focalLengthDen = 100;
    focusDistanceNum = 0;
    focusDistanceDen = 0;
    apertureNum = 276;
    apertureDen = 100;
    horizontalViewAngle = 51.2f;
    verticalViewAngle = 39.4f;
    focalLengthIn35mmLength = 31;

    minFps = 1;
    maxFps = 30;

    minExposureCompensation = -4;
    maxExposureCompensation = 4;
    exposureCompensationStep = 0.5f;
    maxNumDetectedFaces = 16;
    maxNumFocusAreas = 2;
    maxNumMeteringAreas = 32;
    maxZoomLevel = ZOOM_LEVEL_MAX;
    maxZoomRatio = 400;

    zoomSupport = true;
    smoothZoomSupport = false;
    videoSnapshotSupport = true;
    videoStabilizationSupport = false;
    autoWhiteBalanceLockSupport = true;
    autoExposureLockSupport = true;

    antiBandingList =
        ANTIBANDING_AUTO
        | ANTIBANDING_50HZ
        | ANTIBANDING_60HZ
        | ANTIBANDING_OFF
        ;

    effectList =
        EFFECT_NONE
        | EFFECT_MONO
        | EFFECT_NEGATIVE
        /*| EFFECT_SOLARIZE*/
        | EFFECT_SEPIA
        | EFFECT_POSTERIZE
        /*| EFFECT_WHITEBOARD*/
        /*| EFFECT_BLACKBOARD*/
        | EFFECT_AQUA
        ;

    flashModeList =
        FLASH_MODE_OFF
        | FLASH_MODE_AUTO
        | FLASH_MODE_ON
        /*| FLASH_MODE_RED_EYE*/
        | FLASH_MODE_TORCH;

    focusModeList =
        FOCUS_MODE_AUTO
        /*| FOCUS_MODE_INFINITY*/
        | FOCUS_MODE_MACRO
        /*| FOCUS_MODE_FIXED*/
        /*| FOCUS_MODE_EDOF*/
        | FOCUS_MODE_CONTINUOUS_VIDEO
        | FOCUS_MODE_CONTINUOUS_PICTURE
        | FOCUS_MODE_TOUCH;

    sceneModeList =
        SCENE_MODE_AUTO
        | SCENE_MODE_ACTION
        | SCENE_MODE_PORTRAIT
        | SCENE_MODE_LANDSCAPE
        | SCENE_MODE_NIGHT
        | SCENE_MODE_NIGHT_PORTRAIT
        | SCENE_MODE_THEATRE
        | SCENE_MODE_BEACH
        | SCENE_MODE_SNOW
        | SCENE_MODE_SUNSET
        | SCENE_MODE_STEADYPHOTO
        | SCENE_MODE_FIREWORKS
        | SCENE_MODE_SPORTS
        | SCENE_MODE_PARTY
        | SCENE_MODE_CANDLELIGHT;

    whiteBalanceList =
        WHITE_BALANCE_AUTO
        | WHITE_BALANCE_INCANDESCENT
        | WHITE_BALANCE_FLUORESCENT
        /*| WHITE_BALANCE_WARM_FLUORESCENT*/
        | WHITE_BALANCE_DAYLIGHT
        | WHITE_BALANCE_CLOUDY_DAYLIGHT
        /*| WHITE_BALANCE_TWILIGHT*/
        /*| WHITE_BALANCE_SHADE*/
        ;

    /* vendor specifics */
    /*
       burstPanoramaW = 3264;
       burstPanoramaH = 1836;
       highSpeedRecording60WFHD = 1920;
       highSpeedRecording60HFHD = 1080;
       highSpeedRecording60W = 1008;
       highSpeedRecording60H = 566;
       highSpeedRecording120W = 1008;
       highSpeedRecording120H = 566;
       scalableSensorSupport = true;
     */
    bnsSupport = false;

    if (bnsSupport == true) {
        previewSizeLutMax     = 0;
        pictureSizeLutMax     = 0;
        videoSizeLutMax       = 0;
        previewSizeLut        = NULL;
        pictureSizeLut        = NULL;
        videoSizeLut          = NULL;
        videoSizeLutHighSpeed = NULL;
        sizeTableSupport      = false;
    } else {
        previewSizeLutMax     = sizeof(PREVIEW_SIZE_LUT_IMX175) / (sizeof(int) * SIZE_OF_LUT);
        videoSizeLutMax       = sizeof(VIDEO_SIZE_LUT_IMX175)   / (sizeof(int) * SIZE_OF_LUT);
        pictureSizeLutMax     = sizeof(PICTURE_SIZE_LUT_IMX175) / (sizeof(int) * SIZE_OF_LUT);
        previewSizeLut        = PREVIEW_SIZE_LUT_IMX175;
        videoSizeLut          = VIDEO_SIZE_LUT_IMX175;
        pictureSizeLut        = PICTURE_SIZE_LUT_IMX175;
        videoSizeLutHighSpeed = VIDEO_SIZE_LUT_HIGH_SPEED_IMX175;
        sizeTableSupport      = true;
    }
};

ExynosSensorS5K8B1::ExynosSensorS5K8B1()
{
    maxPreviewW = 1920;
    maxPreviewH = 1080;
    maxPictureW = 1920;
    maxPictureH = 1080;
    maxVideoW = 1920;
    maxVideoH = 1080;
    maxSensorW = 1920;
    maxSensorH = 1080;
    maxThumbnailW = 512;
    maxThumbnailH = 384;

    fNumberNum = 22;
    fNumberDen = 10;
    focalLengthNum = 420;
    focalLengthDen = 100;
    focusDistanceNum = 0;
    focusDistanceDen = 0;
    apertureNum = 227;
    apertureDen = 100;
    horizontalViewAngle = 51.2f;
    verticalViewAngle = 39.4f;
    focalLengthIn35mmLength = 31;

    minFps = 1;
    maxFps = 30;

    minExposureCompensation = -4;
    maxExposureCompensation = 4;
    exposureCompensationStep = 0.5f;
    maxNumDetectedFaces = 16;
    maxNumFocusAreas = 1;
    maxNumMeteringAreas = 32;
    maxZoomLevel = ZOOM_LEVEL_MAX;
    maxZoomRatio = 400;

    zoomSupport = true;
    smoothZoomSupport = false;
    videoSnapshotSupport = true;
    videoStabilizationSupport = true;
    autoWhiteBalanceLockSupport = true;
    autoExposureLockSupport = true;

    antiBandingList =
          ANTIBANDING_AUTO
        | ANTIBANDING_50HZ
        | ANTIBANDING_60HZ
        | ANTIBANDING_OFF
        ;

    effectList =
          EFFECT_NONE
        | EFFECT_MONO
        | EFFECT_NEGATIVE
        /*| EFFECT_SOLARIZE*/
        | EFFECT_SEPIA
        | EFFECT_POSTERIZE
        /*| EFFECT_WHITEBOARD*/
        /*| EFFECT_BLACKBOARD*/
        | EFFECT_AQUA
        ;

    flashModeList =
          FLASH_MODE_OFF
        /*| FLASH_MODE_AUTO*/
        /*| FLASH_MODE_ON*/
        /*| FLASH_MODE_RED_EYE*/
        /*| FLASH_MODE_TORCH*/
        ;

    focusModeList =
        /* FOCUS_MODE_AUTO*/
        FOCUS_MODE_INFINITY
        /*| FOCUS_MODE_MACRO*/
        /*| FOCUS_MODE_FIXED*/
        /*| FOCUS_MODE_EDOF*/
        /*| FOCUS_MODE_CONTINUOUS_VIDEO*/
        /*| FOCUS_MODE_CONTINUOUS_PICTURE*/
        /*| FOCUS_MODE_TOUCH*/
        ;

    sceneModeList =
          SCENE_MODE_AUTO
        | SCENE_MODE_ACTION
        | SCENE_MODE_PORTRAIT
        | SCENE_MODE_LANDSCAPE
        | SCENE_MODE_NIGHT
        | SCENE_MODE_NIGHT_PORTRAIT
        | SCENE_MODE_THEATRE
        | SCENE_MODE_BEACH
        | SCENE_MODE_SNOW
        | SCENE_MODE_SUNSET
        | SCENE_MODE_STEADYPHOTO
        | SCENE_MODE_FIREWORKS
        | SCENE_MODE_SPORTS
        | SCENE_MODE_PARTY
        | SCENE_MODE_CANDLELIGHT;

    whiteBalanceList =
          WHITE_BALANCE_AUTO
        | WHITE_BALANCE_INCANDESCENT
        | WHITE_BALANCE_FLUORESCENT
        /* WHITE_BALANCE_WARM_FLUORESCENT*/
        | WHITE_BALANCE_DAYLIGHT
        | WHITE_BALANCE_CLOUDY_DAYLIGHT
        /* WHITE_BALANCE_TWILIGHT*/
        /* WHITE_BALANCE_SHADE*/
        ;

    previewSizeLutMax     = 0;
    pictureSizeLutMax     = 0;
    videoSizeLutMax       = 0;
    previewSizeLut        = NULL;
    pictureSizeLut        = NULL;
    videoSizeLut          = NULL;
    videoSizeLutHighSpeed = NULL;
    sizeTableSupport      = false;

    /* vendor specifics */
    highResolutionCallbackW = 3264;
    highResolutionCallbackH = 1836;
    highSpeedRecording60WFHD = 1920;
    highSpeedRecording60HFHD = 1080;
    highSpeedRecording60W = 1008;
    highSpeedRecording60H = 566;
    highSpeedRecording120W = 1008;
    highSpeedRecording120H = 566;
    scalableSensorSupport = true;
    bnsSupport = false;
};

}; /* namespace android */
