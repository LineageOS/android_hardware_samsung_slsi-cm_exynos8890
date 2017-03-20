/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ANDROID_EXYNOS_HWC_MODULE_H_
#define ANDROID_EXYNOS_HWC_MODULE_H_

#include <hardware/hwcomposer.h>
#include "decon.h"

#define VSYNC_DEV_PREFIX "/sys/devices/"
#define VSYNC_DEV_MIDDLE ""
#define VSYNC_DEV_NAME  "13960000.decon_f/vsync"

#define FIMD_WORD_SIZE_BYTES   16
#define FIMD_BURSTLEN   8
#define FIMD_ADDED_BURSTLEN_BYTES     4
#define FIMD_BW_OVERLAP_CHECK

#define DUAL_VIDEO_OVERLAY_SUPPORT

/* Framebuffer API specific defines (decon.h) */
#define WIN_STATE_DISABLED  decon_win_config::DECON_WIN_STATE_DISABLED
#define WIN_STATE_COLOR     decon_win_config::DECON_WIN_STATE_COLOR
#define WIN_STATE_BUFFER    decon_win_config::DECON_WIN_STATE_BUFFER
#define BLENDING_NONE       DECON_BLENDING_NONE
#define BLENDING_MAX        DECON_BLENDING_MAX
#define PIXEL_FORMAT_MAX    DECON_PIXEL_FORMAT_MAX

const size_t SOC_NUM_HW_WINDOWS = MAX_DECON_WIN;

typedef decon_win_config fb_win_config;
typedef decon_win_config_data fb_win_config_data;

inline decon_blending halBlendingToSocBlending(int32_t blending)
{
    switch (blending) {
        case HWC_BLENDING_NONE:
            return DECON_BLENDING_NONE;
        case HWC_BLENDING_PREMULT:
            return DECON_BLENDING_PREMULT;
        case HWC_BLENDING_COVERAGE:
            return DECON_BLENDING_COVERAGE;

        default:
            return DECON_BLENDING_MAX;
    }
}

inline decon_pixel_format halFormatToSocFormat(int format)
{
    switch (format) {
    case HAL_PIXEL_FORMAT_RGBA_8888:
        return DECON_PIXEL_FORMAT_RGBA_8888;
    case HAL_PIXEL_FORMAT_RGBX_8888:
        return DECON_PIXEL_FORMAT_RGBX_8888;
    case HAL_PIXEL_FORMAT_RGB_565:
        return DECON_PIXEL_FORMAT_RGB_565;
    case HAL_PIXEL_FORMAT_BGRA_8888:
        return DECON_PIXEL_FORMAT_BGRA_8888;
#ifdef EXYNOS_SUPPORT_BGRX_8888
    case HAL_PIXEL_FORMAT_BGRX_8888:
        return DECON_PIXEL_FORMAT_BGRX_8888;
#endif
    default:
        return DECON_PIXEL_FORMAT_MAX;
    }
}

static decon_idma_type getIdmaType(int32_t index)
{
    decon_idma_type ret = IDMA_G1;

    switch(index) {
    case 0:
        ret = IDMA_G1;
        break;
    case 1:
        ret = IDMA_G2;
        break;
    case 2:
        ret = IDMA_G3;
        break;
    case 3:
        ret = IDMA_VG0;
        break;
    case 4:
        ret = IDMA_VG1;
        break;
    case 5:
        ret = IDMA_VGR0;
        break;
    case 6:
        ret = IDMA_VGR1;
        break;
    case 7:
        ret = IDMA_G0;
        break;
    default:
        ALOGE("%s: cannot handle index %d", __func__, index);
        break;
    }

    return ret;
}

static bool isVppType(enum decon_idma_type idma_type)
{
    switch (idma_type) {
    case IDMA_VG0:
    case IDMA_VG1:
    case IDMA_VGR0:
    case IDMA_VGR1:
        return true;
    default:
        return false;
    }
}

#ifdef FIMD_BW_OVERLAP_CHECK
const size_t MAX_NUM_FIMD_DMA_CH = 2;
const uint32_t FIMD_DMA_CH_IDX[] = {0, 1, 1, 1, 0};
const uint32_t FIMD_DMA_CH_BW_SET1[MAX_NUM_FIMD_DMA_CH] = {1920 * 1080 *2, 1920 * 1080 *2};
const uint32_t FIMD_DMA_CH_BW_SET2[MAX_NUM_FIMD_DMA_CH] = {2560 * 1600, 2560 * 1600 *2};
const uint32_t FIMD_DMA_CH_OVERLAP_CNT_SET1[MAX_NUM_FIMD_DMA_CH] = {2, 2};
const uint32_t FIMD_DMA_CH_OVERLAP_CNT_SET2[MAX_NUM_FIMD_DMA_CH] = {1, 2};

inline void fimd_bw_overlap_limits_init(int xres, int yres,
            uint32_t *fimd_dma_chan_max_bw, uint32_t *fimd_dma_chan_max_overlap_cnt)
{
    if (xres * yres > 1920 * 1080) {
        for (size_t i = 0; i < MAX_NUM_FIMD_DMA_CH; i++) {
            fimd_dma_chan_max_bw[i] = FIMD_DMA_CH_BW_SET2[i];
            fimd_dma_chan_max_overlap_cnt[i] = FIMD_DMA_CH_OVERLAP_CNT_SET2[i];
        }
    } else {
        for (size_t i = 0; i < MAX_NUM_FIMD_DMA_CH; i++) {
            fimd_dma_chan_max_bw[i] = FIMD_DMA_CH_BW_SET1[i];
            fimd_dma_chan_max_overlap_cnt[i] = FIMD_DMA_CH_OVERLAP_CNT_SET1[i];
        }
    }
}
#endif

const size_t GSC_DST_W_ALIGNMENT_RGB888 = 16;
const size_t GSC_DST_CROP_W_ALIGNMENT_RGB888 = 1;
const size_t GSC_W_ALIGNMENT = 16;
const size_t GSC_H_ALIGNMENT = 16;
const size_t GSC_DST_H_ALIGNMENT_RGB888 = 1;
const size_t FIMD_GSC_IDX = 0;
const size_t FIMD_GSC_SEC_IDX = 1;
const size_t HDMI_GSC_IDX = 2;
#ifdef USES_VIRTUAL_DISPLAY
const size_t WFD_GSC_IDX = 3;
#else
const size_t WFD_GSC_DRM_IDX = 3;
#endif
const int FIMD_GSC_USAGE_IDX[] = {FIMD_GSC_IDX, FIMD_GSC_SEC_IDX};
#ifdef USES_VIRTUAL_DISPLAY
const int AVAILABLE_GSC_UNITS[] = { 0, 1, 1, 1 };
#else
const int AVAILABLE_GSC_UNITS[] = { 0, 1, 1, 5 };
#endif

#endif
