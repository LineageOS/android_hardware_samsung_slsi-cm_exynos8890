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

#ifndef EXYNOS_CAMERA_SENSOR_INFO_H
#define EXYNOS_CAMERA_SENSOR_INFO_H

#include <videodev2.h>
#include <videodev2_exynos_camera.h>
#include "ExynosCameraConfig.h"
#include "ExynosCameraSizeTable.h"
#include "fimc-is-metadata.h"

/*TODO: This values will be changed */
#define BACK_CAMERA_AUTO_FOCUS_DISTANCES_STR       "0.10,1.20,Infinity"
#define FRONT_CAMERA_FOCUS_DISTANCES_STR           "0.20,0.25,Infinity"

#define BACK_CAMERA_MACRO_FOCUS_DISTANCES_STR      "0.10,0.20,Infinity"
#define BACK_CAMERA_INFINITY_FOCUS_DISTANCES_STR   "0.10,1.20,Infinity"

#define BACK_CAMERA_FOCUS_DISTANCE_INFINITY        "Infinity"
#define FRONT_CAMERA_FOCUS_DISTANCE_INFINITY       "Infinity"

#define UNIQUE_ID_BUF_SIZE          (32)

#define EFFECTMODE_META_2_HAL(x) (1 << x)

namespace android {

#ifdef SENSOR_NAME_GET_FROM_FILE
int getSensorIdFromFile(int camId);
#endif

struct exynos_camera_info {
public:
    int     previewW;
    int     previewH;
    int     previewFormat;
    int     previewStride;

    int     pictureW;
    int     pictureH;
    int     pictureFormat;

    int     videoW;
    int     videoH;

    /* This size for internal */
    int     hwSensorW;
    int     hwSensorH;
    int     hwPreviewW;
    int     hwPreviewH;
    int     previewSizeRatioId;
    int     hwPictureW;
    int     hwPictureH;
    int     pictureSizeRatioId;
    int     hwDisW;
    int     hwDisH;
    int     videoSizeRatioId;
    int     hwPreviewFormat;

    int     hwBayerCropW;
    int     hwBayerCropH;
    int     hwBayerCropX;
    int     hwBayerCropY;

    int     bnsW;
    int     bnsH;

    int     jpegQuality;
    int     thumbnailW;
    int     thumbnailH;
    int     thumbnailQuality;

    int     intelligentMode;
    bool    visionMode;
    int     visionModeFps;
    int     visionModeAeTarget;

    bool    recordingHint;
    bool    dualMode;
    bool    dualRecordingHint;
    bool    effectHint;
    bool    highSpeedRecording;
    bool    videoStabilization;
    bool    swVdisMode;
    bool    swVdisUIMode;
    bool    highResolutionCallbackMode;
    bool    is3dnrMode;
    bool    isDrcMode;
    bool    isOdcMode;

    int     zoom;
    int     rotation;
    int     flipHorizontal;
    int     flipVertical;
    bool    autoExposureLock;

    int     meteringMode;
    bool    isTouchMetering;

    int     sceneMode;
    int     focusMode;
    int     flashMode;
    int     whiteBalanceMode;
    bool    autoWhiteBalanceLock;
    int     numValidFocusArea;

    double  gpsLatitude;
    double  gpsLongitude;
    double  gpsAltitude;
    long    gpsTimeStamp;

    long long int cityId;
    unsigned char weatherId;

    bool    hdrMode;
    bool    wdrMode;
    int     shotMode;
    bool    antiShake;
    int     vtMode;
    bool    gamma;
    bool    slowAe;
    int     seriesShotCount;

    bool    scalableSensorMode;
    char    imageUniqueId[UNIQUE_ID_BUF_SIZE];
    bool    samsungCamera;

    int     autoFocusMacroPosition;
    int     deviceOrientation;
    uint32_t    bnsScaleRatio;

    int     seriesShotMode;
#ifdef BURST_CAPTURE
    int     seriesShotSaveLocation;
    char    seriesShotFilePath[100];
#endif
};

struct ExynosSensorInfo {
public:
    int     maxPreviewW;
    int     maxPreviewH;
    int     maxPictureW;
    int     maxPictureH;
    int     maxVideoW;
    int     maxVideoH;
    int     maxSensorW;
    int     maxSensorH;

    int     maxThumbnailW;
    int     maxThumbnailH;

    int     fNumberNum;
    int     fNumberDen;
    int     focalLengthNum;
    int     focalLengthDen;
    int     focusDistanceNum;
    int     focusDistanceDen;
    int     apertureNum;
    int     apertureDen;
    float   horizontalViewAngle;
    float   verticalViewAngle;
    int     focalLengthIn35mmLength;

    int     minFps;
    int     maxFps;

    int     minExposureCompensation;
    int     maxExposureCompensation;
    float   exposureCompensationStep;
    int     maxNumDetectedFaces;
    uint32_t     maxNumFocusAreas;
    uint32_t     maxNumMeteringAreas;
    int     maxZoomLevel;
    int     maxZoomRatio;

    bool    zoomSupport;
    bool    smoothZoomSupport;
    bool    videoSnapshotSupport;
    bool    videoStabilizationSupport;
    bool    autoWhiteBalanceLockSupport;
    bool    autoExposureLockSupport;

    int     antiBandingList;
    int     effectList;
    int     flashModeList;
    int     focusModeList;
    int     sceneModeList;
    int     whiteBalanceList;

    int     previewSizeLutMax;
    int     pictureSizeLutMax;
    int     videoSizeLutMax;
    int     (*previewSizeLut)[SIZE_OF_LUT];
    int     (*pictureSizeLut)[SIZE_OF_LUT];
    int     (*videoSizeLut)[SIZE_OF_LUT];
    int     (*videoSizeLutHighSpeed)[SIZE_OF_LUT];
    bool    sizeTableSupport;

    /* vendor specifics */
    int     highResolutionCallbackW;
    int     highResolutionCallbackH;
    int     highSpeedRecording60WFHD;
    int     highSpeedRecording60HFHD;
    int     highSpeedRecording60W;
    int     highSpeedRecording60H;
    int     highSpeedRecording120W;
    int     highSpeedRecording120H;
    bool    scalableSensorSupport;
    bool    bnsSupport;
public:
    ExynosSensorInfo();
};

struct ExynosSensorIMX135 : public ExynosSensorInfo {
public:
    ExynosSensorIMX135();
};

struct ExynosSensorIMX134 : public ExynosSensorInfo {
public:
    ExynosSensorIMX134();
};

struct ExynosSensorS5K3L2 : public ExynosSensorInfo {
public:
    ExynosSensorS5K3L2();
};

struct ExynosSensorS5K2P2 : public ExynosSensorInfo {
public:
    ExynosSensorS5K2P2();
};

struct ExynosSensorS5K6B2 : public ExynosSensorInfo {
public:
    ExynosSensorS5K6B2();
};

struct ExynosSensorS5K3H7 : public ExynosSensorInfo {
public:
    ExynosSensorS5K3H7();
};

struct ExynosSensorS5K3H5 : public ExynosSensorInfo {
public:
    ExynosSensorS5K3H5();
};

struct ExynosSensorS5K4H5 : public ExynosSensorInfo {
public:
    ExynosSensorS5K4H5();
};

struct ExynosSensorS5K6A3 : public ExynosSensorInfo {
public:
    ExynosSensorS5K6A3();
};

struct ExynosSensorIMX175 : public ExynosSensorInfo {
public:
    ExynosSensorIMX175();
};

struct ExynosSensorS5K8B1 : public ExynosSensorInfo {
public:
    ExynosSensorS5K8B1();
};

/* Helpper functions */
struct ExynosSensorInfo *createSensorInfo(int sensorName);
int getSensorId(int camId);

bool needGSCForCapture(int camId);

static int PREVIEW_LIST[][SIZE_OF_RESOLUTION] =
{
    { 5312, 2988, SIZE_RATIO_16_9},
    { 4128, 3096, SIZE_RATIO_4_3},
    { 4096, 3072, SIZE_RATIO_4_3},
    { 3840, 2160, SIZE_RATIO_16_9},
    { 3200, 2400, SIZE_RATIO_4_3},
    { 3072, 1728, SIZE_RATIO_16_9},
    { 2592, 1944, SIZE_RATIO_4_3},
    { 2592, 1936, SIZE_RATIO_4_3},  /* not exactly matched ratio */
    { 2560, 1920, SIZE_RATIO_4_3},
    { 2560, 1440, SIZE_RATIO_16_9},
    { 2048, 1536, SIZE_RATIO_4_3},
    { 1920, 1440, SIZE_RATIO_4_3},
    { 1920, 1080, SIZE_RATIO_16_9},
    { 1600, 1200, SIZE_RATIO_4_3},
/*    { 1446, 1080, SIZE_RATIO_4_3}, *//* preview ratio for 2592x1936 */
/* remove for CTS */
    { 1440, 1080, SIZE_RATIO_4_3},
    /* { 1392, 1392, SIZE_RATIO_1_1}, */ /* remove for CTS */
    { 1280,  960, SIZE_RATIO_4_3},
    { 1280,  720, SIZE_RATIO_16_9},
/*    { 1080, 1080, SIZE_RATIO_1_1}, */
    { 1056,  864, SIZE_RATIO_11_9},
    { 1024,  768, SIZE_RATIO_4_3},
    {  960,  720, SIZE_RATIO_4_3},
/*    { 1024,  574, SIZE_RATIO_16_9}, vendor specific *//* not exactly matched ratio */
/*    { 1008,  568, SIZE_RATIO_16_9}, vendor specific *//* not exactly matched ratio */
    {  800,  600, SIZE_RATIO_4_3},
    {  800,  480, SIZE_RATIO_5_3},
    {  800,  450, SIZE_RATIO_16_9},
    {  720,  720, SIZE_RATIO_1_1},
    {  720,  480, SIZE_RATIO_3_2},
    {  640,  480, SIZE_RATIO_4_3},
    {  528,  432, SIZE_RATIO_11_9},
    {  480,  320, SIZE_RATIO_3_2},
    {  480,  270, SIZE_RATIO_16_9},
    {  352,  288, SIZE_RATIO_11_9},
    {  320,  240, SIZE_RATIO_4_3},
/* SCP can support up to 1/16 x 1/16 scale down. */
/*	  {  176,  144, SIZE_RATIO_11_9} */
};

static int PICTURE_LIST[][SIZE_OF_RESOLUTION] =
{
    { 5312, 2988, SIZE_RATIO_16_9},
    { 4128, 3096, SIZE_RATIO_4_3},
    { 4128, 2322, SIZE_RATIO_16_9},
    { 4096, 3072, SIZE_RATIO_4_3},
    { 4096, 2304, SIZE_RATIO_16_9},
    { 3840, 2160, SIZE_RATIO_16_9},
    { 3264, 2448, SIZE_RATIO_4_3},
    { 3264, 1836, SIZE_RATIO_16_9},
    { 3200, 2400, SIZE_RATIO_4_3},
    { 3072, 1728, SIZE_RATIO_16_9},
    { 2988, 2988, SIZE_RATIO_1_1},
    { 2592, 1944, SIZE_RATIO_4_3},
    { 2592, 1936, SIZE_RATIO_4_3},  /* not exactly matched ratio */
    { 2560, 1920, SIZE_RATIO_4_3},
    { 2448, 2448, SIZE_RATIO_1_1},
    { 2048, 1536, SIZE_RATIO_4_3},
    { 2048, 1152, SIZE_RATIO_16_9},
    { 1920, 1080, SIZE_RATIO_16_9},
    { 1600, 1200, SIZE_RATIO_4_3},
    { 1440, 1080, SIZE_RATIO_4_3},
    /* { 1392, 1392, SIZE_RATIO_1_1}, */ /* remove for CTS */
    { 1280,  960, SIZE_RATIO_4_3},
    { 1280,  720, SIZE_RATIO_16_9},
    { 1024,  768, SIZE_RATIO_4_3},
/*    { 1024,  574, SIZE_RATIO_16_9}, vendor specific *//* not exactly matched ratio */
/*    { 1008,  568, SIZE_RATIO_16_9}, vendor specific *//* not exactly matched ratio */
    {  800,  600, SIZE_RATIO_4_3},
    {  800,  480, SIZE_RATIO_5_3},
    {  800,  450, SIZE_RATIO_16_9},
    {  720,  480, SIZE_RATIO_3_2},
    {  640,  480, SIZE_RATIO_4_3},
    /* {  528,  432, SIZE_RATIO_11_9}, */ /* remove for CTS */
    {  512,  384, SIZE_RATIO_4_3},
    {  512,  288, SIZE_RATIO_11_9},
    {  480,  320, SIZE_RATIO_3_2},
    /* {  352,  288, SIZE_RATIO_16_9}, */ /* remove for CTS */
    {  320,  240, SIZE_RATIO_4_3},
    {  320,  180, SIZE_RATIO_16_9},
/* TODO : will be support after enable REPROCESSING */
/*    {  176,  144, SIZE_RATIO_11_9} */
};

static int THUMBNAIL_LIST[][SIZE_OF_RESOLUTION] =
{
    {  512,  384, SIZE_RATIO_4_3},
    {  320,  240, SIZE_RATIO_4_3},
    {    0,    0, SIZE_RATIO_1_1}
};

static int VIDEO_LIST[][SIZE_OF_RESOLUTION] =
{
/* 
 * When GED camera App goes into Recording mode,
 * App set the Preview size to the 1st size of the video list.
 * In case the 1st size of the list is UHD,
 * although App do FHD recording, App do preview with UHD.
 * So, the useless electric current is wasted.
 * Conclusion: The 1st of the list set the LCD size until GED camera App is modified.
 */
    { 1920, 1080, SIZE_RATIO_16_9},
    { 3840, 2160, SIZE_RATIO_16_9},
    { 1440, 1080, SIZE_RATIO_4_3},
    { 1280,  720, SIZE_RATIO_16_9},
    {  960,  720, SIZE_RATIO_4_3},
    {  800,  450, SIZE_RATIO_16_9},
    {  720,  480, SIZE_RATIO_3_2},
    {  640,  480, SIZE_RATIO_4_3},
    {  480,  320, SIZE_RATIO_3_2},
    {  352,  288, SIZE_RATIO_11_9},
    {  320,  240, SIZE_RATIO_4_3},
    {  176,  144, SIZE_RATIO_11_9}
};

static int FPS_RANGE_LIST[][2] =
{
    {   5000,   5000},
    {   7000,   7000},
    {  15000,  15000},
    {  24000,  24000},
    {  27000,  27000},
    {   4000,  30000},
    {  10000,  30000},
    {  15000,  30000},
    {  30000,  30000},
    {  30000,  60000},
    {  60000,  60000},
    {  60000, 120000},
    { 120000, 120000},
};

enum CAMERA_ID {
    CAMERA_ID_BACK  = 0,
    CAMERA_ID_FRONT = 1,
    CAMERA_ID_MAX,
};

enum MODE {
    MODE_PREVIEW = 0,
    MODE_PICTURE,
    MODE_VIDEO,
    MODE_THUMBNAIL,
};

enum {
    ANTIBANDING_AUTO = (1 << 0),
    ANTIBANDING_50HZ = (1 << 1),
    ANTIBANDING_60HZ = (1 << 2),
    ANTIBANDING_OFF  = (1 << 3),
};

enum {
    SCENE_MODE_AUTO           = (1 << 0),
    SCENE_MODE_ACTION         = (1 << 1),
    SCENE_MODE_PORTRAIT       = (1 << 2),
    SCENE_MODE_LANDSCAPE      = (1 << 3),
    SCENE_MODE_NIGHT          = (1 << 4),
    SCENE_MODE_NIGHT_PORTRAIT = (1 << 5),
    SCENE_MODE_THEATRE        = (1 << 6),
    SCENE_MODE_BEACH          = (1 << 7),
    SCENE_MODE_SNOW           = (1 << 8),
    SCENE_MODE_SUNSET         = (1 << 9),
    SCENE_MODE_STEADYPHOTO    = (1 << 10),
    SCENE_MODE_FIREWORKS      = (1 << 11),
    SCENE_MODE_SPORTS         = (1 << 12),
    SCENE_MODE_PARTY          = (1 << 13),
    SCENE_MODE_CANDLELIGHT    = (1 << 14),
};

enum {
    FOCUS_MODE_AUTO               = (1 << 0),
    FOCUS_MODE_INFINITY           = (1 << 1),
    FOCUS_MODE_MACRO              = (1 << 2),
    FOCUS_MODE_FIXED              = (1 << 3),
    FOCUS_MODE_EDOF               = (1 << 4),
    FOCUS_MODE_CONTINUOUS_VIDEO   = (1 << 5),
    FOCUS_MODE_CONTINUOUS_PICTURE = (1 << 6),
    FOCUS_MODE_TOUCH              = (1 << 7),
    FOCUS_MODE_CONTINUOUS_PICTURE_MACRO = (1 << 8),
};

enum {
    WHITE_BALANCE_AUTO             = (1 << 0),
    WHITE_BALANCE_INCANDESCENT     = (1 << 1),
    WHITE_BALANCE_FLUORESCENT      = (1 << 2),
    WHITE_BALANCE_WARM_FLUORESCENT = (1 << 3),
    WHITE_BALANCE_DAYLIGHT         = (1 << 4),
    WHITE_BALANCE_CLOUDY_DAYLIGHT  = (1 << 5),
    WHITE_BALANCE_TWILIGHT         = (1 << 6),
    WHITE_BALANCE_SHADE            = (1 << 7),
};

enum {
    FLASH_MODE_OFF     = (1 << 0),
    FLASH_MODE_AUTO    = (1 << 1),
    FLASH_MODE_ON      = (1 << 2),
    FLASH_MODE_RED_EYE = (1 << 3),
    FLASH_MODE_TORCH   = (1 << 4),
};

enum {
    EFFECT_NONE       = (1 << COLORCORRECTION_MODE_FAST),
    EFFECT_MONO       = (1 << COLORCORRECTION_MODE_EFFECT_MONO),
    EFFECT_NEGATIVE   = (1 << COLORCORRECTION_MODE_EFFECT_NEGATIVE),
    EFFECT_SOLARIZE   = (1 << COLORCORRECTION_MODE_EFFECT_SOLARIZE),
    EFFECT_SEPIA      = (1 << COLORCORRECTION_MODE_EFFECT_SEPIA),
    EFFECT_POSTERIZE  = (1 << COLORCORRECTION_MODE_EFFECT_POSTERIZE),
    EFFECT_WHITEBOARD = (1 << COLORCORRECTION_MODE_EFFECT_WHITEBOARD),
    EFFECT_BLACKBOARD = (1 << COLORCORRECTION_MODE_EFFECT_BLACKBOARD),
    EFFECT_AQUA       = (1 << COLORCORRECTION_MODE_EFFECT_AQUA),
};

/* Metering */
enum {
    METERING_MODE_AVERAGE = (1 << 0),
    METERING_MODE_CENTER  = (1 << 1),
    METERING_MODE_MATRIX  = (1 << 2),
    METERING_MODE_SPOT    = (1 << 3),
};

/* Contrast */
enum {
    CONTRAST_AUTO    = (1 << 0),
    CONTRAST_MINUS_2 = (1 << 1),
    CONTRAST_MINUS_1 = (1 << 2),
    CONTRAST_DEFAULT = (1 << 3),
    CONTRAST_PLUS_1  = (1 << 4),
    CONTRAST_PLUS_2  = (1 << 5),
};

/* Shot mode */
enum SHOT_MODE {
    SHOT_MODE_NORMAL         = 0x00,
    SHOT_MODE_AUTO           = 0x01,
    SHOT_MODE_BEAUTY_FACE    = 0x02,
    SHOT_MODE_BEST_PHOTO     = 0x03,
    SHOT_MODE_DRAMA          = 0x04,
    SHOT_MODE_BEST_FACE      = 0x05,
    SHOT_MODE_ERASER         = 0x06,
    SHOT_MODE_PANORAMA       = 0x07,
    SHOT_MODE_3D_PANORAMA    = 0x08,
    SHOT_MODE_RICH_TONE      = 0x09,
    SHOT_MODE_NIGHT          = 0x0A,
    SHOT_MODE_STORY          = 0x0B,
    SHOT_MODE_AUTO_PORTRAIT  = 0x0C,
    SHOT_MODE_PET            = 0x0D,
    SHOT_MODE_GOLF           = 0x0E,
    SHOT_MODE_ANIMATED_SCENE = 0x0F,
    SHOT_MODE_NIGHT_SCENE    = 0x10,
    SHOT_MODE_SPORTS         = 0x11,
    SHOT_MODE_SLOWMOTION     = 0x12,
    SHOT_MODE_MAGIC          = 0x13,
    SHOT_MODE_MAX,
};

enum SERIES_SHOT_MODE {
    SERIES_SHOT_MODE_NONE       = 0,
    SERIES_SHOT_MODE_LLS        = 1,
    SERIES_SHOT_MODE_SIS        = 2,
    SERIES_SHOT_MODE_BURST      = 3,
    SERIES_SHOT_MODE_ERASER     = 4,
    SERIES_SHOT_MODE_BEST_FACE  = 5,
    SERIES_SHOT_MODE_BEST_PHOTO = 6,
    SERIES_SHOT_MODE_MAGIC      = 7,
    SERIES_SHOT_MODE_MAX,
};
}; /* namespace android */
#endif
