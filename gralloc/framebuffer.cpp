/*
 * Copyright (C) 2008 The Android Open Source Project
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

#include <sys/mman.h>

#include <dlfcn.h>

#include <cutils/ashmem.h>
#include <cutils/log.h>

#include <hardware/hardware.h>
#include <hardware/gralloc.h>

#include <GLES/gl.h>

#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

#include <utils/Vector.h>

#include <cutils/log.h>
#include <cutils/atomic.h>
#include <cutils/properties.h>

#if HAVE_ANDROID_OS
#include <linux/fb.h>
#endif

#include "gralloc_priv.h"
#include "gralloc_vsync.h"
#include "decon-fb.h"

/*****************************************************************************/

// numbers of buffers for page flipping
#define NUM_BUFFERS 3
const size_t NUM_HW_WINDOWS = 8;

#define FBIO_WAITFORVSYNC       _IOW('F', 0x20, __u32)

struct hwc_callback_entry
{
    void (*callback)(void *, private_handle_t *);
    void *data;
};

typedef android::Vector<struct hwc_callback_entry> hwc_callback_queue_t;

struct fb_context_t {
    framebuffer_device_t  device;
};

/*****************************************************************************/

/*
 * Keep track of the vsync state to avoid making excessive ioctl calls.
 * States: -1 --> unknown; 0 --> disabled; 1 --> enabled.
 */
static int vsync_state = -1;

static enum decon_pixel_format exynos5_format_to_decon(int format)
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
    default:
        return DECON_PIXEL_FORMAT_MAX;
    }
}

static int fb_setSwapInterval(struct framebuffer_device_t* dev,
                              int interval)
{
    if (interval < dev->minSwapInterval) {
        interval = dev->minSwapInterval;
    } else if (interval > dev->maxSwapInterval) {
        interval = dev->maxSwapInterval;
    }

    private_module_t* m = reinterpret_cast<private_module_t*>(dev->common.module);
    m->swapInterval = interval;

    if (interval == 0 && vsync_state != 0) {
        gralloc_vsync_disable(dev);
        vsync_state = 0;
    } else if (vsync_state != 1) {
        gralloc_vsync_enable(dev);
        vsync_state = 1;
    }

    ALOGV("%s: vsync state is: %d", __func__, vsync_state);

    return 0;
}

static int fb_post(struct framebuffer_device_t* dev, buffer_handle_t buffer)
{
    struct private_module_t* mod = (private_module_t*)dev->common.module;
    private_handle_t *handle = private_handle_t::dynamicCast(buffer);
    struct decon_win_config_data win_data;
    struct decon_win_config *cfg = win_data.config;

    ALOGD("%s E", __FUNCTION__);

    memset(cfg, 0, sizeof(win_data.config));
    for (size_t i = 0; i < NUM_HW_WINDOWS; i++)
        cfg[i].fence_fd = -1;

    if (private_handle_t::validate(buffer) < 0)
        return -EINVAL;

    cfg[0].state = cfg->DECON_WIN_STATE_BUFFER;
    cfg[0].fd_idma[0] = handle->fd;
    cfg[0].fd_idma[1]= -1;
    cfg[0].fd_idma[2]= -1;
    cfg[0].plane_alpha = 255;
    cfg[0].blending = DECON_BLENDING_NONE;
    cfg[0].idma_type = IDMA_G1;
    cfg[0].src = {0, 0, mod->xres, mod->yres, mod->xres, mod->yres};
    cfg[0].dst = {0, 0, mod->xres, mod->yres, mod->xres, mod->yres};
    cfg[0].format = exynos5_format_to_decon(handle->format);
    cfg[0].fence_fd = -1;

    ALOGD("%s format=0x%x", __FUNCTION__, handle->format);

    int ret = ioctl(mod->fb_fd, S3CFB_WIN_CONFIG, &win_data);
    for (size_t i = 0; i < NUM_HW_WINDOWS; i++)
        if (cfg[i].fence_fd != -1)
            close(cfg[i].fence_fd);
    if (ret < 0) {
        ALOGE("%s: ioctl S3CFB_WIN_CONFIG failed: %s", __FUNCTION__, strerror(errno));
        return ret;
    }

    close(win_data.fence);

    if (mod->enableVSync) {
        unsigned int test=0;

        if (ioctl(mod->fb_fd, FBIO_WAITFORVSYNC, &test) < 0) {
            ALOGE("%s FBIO_WAITFORVSYNC failed", __FUNCTION__);
            return 0;
        }
    }

    //ALOGD("%s X", __FUNCTION__);
    return 0;
}

/*****************************************************************************/

static int fb_close(struct hw_device_t *dev)
{
    fb_context_t* ctx = (fb_context_t*)dev;

    //ALOGD("%s", __FUNCTION__);

    if (ctx) {
        free(ctx);
    }
    return 0;
}

int init_fb(struct private_module_t* module)
{
    int fd = -1;
    char name[64];

    //ALOGD("%s", __FUNCTION__);

    fd = open("/dev/graphics/fb0", O_RDWR);
    if (fd < 0) {
        ALOGE("%s: /dev/graphics/fb0 Open fail", __FUNCTION__);
        return -errno;
    }

    struct fb_fix_screeninfo finfo;
    if (ioctl(fd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        ALOGE("%s: Fail to get FB Screen Info", __FUNCTION__);
        close(fd);
        return -errno;
    }

    struct fb_var_screeninfo info;
    if (ioctl(fd, FBIOGET_VSCREENINFO, &info) == -1) {
        ALOGE("%s: First, Fail to get FB VScreen Info", __FUNCTION__);
        close(fd);
        return -errno;
    }

    /*ALOGD("%s res=(%d,%d) virt_res=(%d,%d) offset(%d,%d) bpp=%d grayscale=%d nonstd=%d activate=0x%x \
                (%d,%d) accel=0x%x pixclock=0x%x margin(%d-%d,%d-%d) len(%d,%d) sync=%d vmode=%d \
                rotate=%d colorspace=%d reserved={%d,%d,%d,%d}", __FUNCTION__,
                info.xres, info.yres, info.xres_virtual, info.yres_virtual,
                info.xoffset, info.yoffset, info.bits_per_pixel, info.grayscale, info.nonstd,
                info.activate, info.width, info.height, info.accel_flags, info.pixclock,
                info.left_margin, info.right_margin, info.upper_margin, info.lower_margin,
                info.hsync_len, info.vsync_len, info.sync, info.vmode,
                info.rotate, info.colorspace,
                info.reserved[0], info.reserved[1], info.reserved[2], info.reserved[3]);*/

    info.reserved[0] = info.xres;
    info.reserved[1] = info.yres;
    info.reserved[2] = 0;
    info.reserved[3] = 0;

    info.xoffset = 0;
    info.yoffset = 0;
    info.activate = FB_ACTIVATE_NOW;

#ifdef GRALLOC_16_BITS
    /*
     * Explicitly request 5/6/5
     */
    info.bits_per_pixel = 16;
    info.red.offset     = 11;
    info.red.length     = 5;
    info.green.offset   = 5;
    info.green.length   = 6;
    info.blue.offset    = 0;
    info.blue.length    = 5;
    info.transp.offset  = 0;
    info.transp.length  = 0;
#else
    /*
     * Explicitly request 8/8/8
     */
    info.bits_per_pixel = 32;
    info.red.offset     = 16;
    info.red.length     = 8;
    info.green.offset   = 8;
    info.green.length   = 8;
    info.blue.offset    = 0;
    info.blue.length    = 8;
    info.transp.offset  = 0;
    info.transp.length  = 0;
#endif

    if (ioctl(fd, FBIOPUT_VSCREENINFO, &info) == -1) {
        ALOGW("%s: FBIOPUT_VSCREENINFO failed", __FUNCTION__);
    }

    int32_t refreshRate = 1000000000000000LLU /
    (
        uint64_t( info.upper_margin + info.lower_margin + info.yres)
        * ( info.left_margin  + info.right_margin + info.xres)
        * info.pixclock
    );

    if (refreshRate == 0)
        refreshRate = 60 * 1000;  /* 60 Hz */

    float xdpi = (info.xres * 25.4f) / info.width;
    float ydpi = (info.yres * 25.4f) / info.height;
    ALOGI("using (id=%s)\n"
          "xres         = %d px\n"
          "yres         = %d px\n"
          "width        = %d mm (%f dpi)\n"
          "height       = %d mm (%f dpi)\n"
          "refresh rate = %.2f Hz\n",
          finfo.id, info.xres, info.yres, info.width, xdpi, info.height,
          ydpi, (float)(refreshRate / 1000));

    module->xres = info.xres;
    module->yres = info.yres;
    module->line_length = info.xres;
    module->xdpi = xdpi;
    module->ydpi = ydpi;
    module->fps = (float)(refreshRate / 1000);
    module->fb_fd = fd;

    memcpy(&module->info, &info, sizeof(struct fb_var_screeninfo));
    memcpy(&module->finfo, &finfo, sizeof(struct fb_fix_screeninfo));

    char value[PROPERTY_VALUE_MAX];
    property_get("debug.gralloc.vsync", value, "1");
    module->enableVSync = atoi(value);

    //close(fd);

    return 0;
}

int compositionComplete(struct framebuffer_device_t* dev)
{
    /* By doing a finish here we force the GL driver to start rendering
     all the drawcalls up to this point, and to wait for the rendering to be complete.*/
    glFinish();
    /* The rendering of the backbuffer is now completed.
     When SurfaceFlinger later does a call to eglSwapBuffer(), the swap will be done
     synchronously in the same thread, and not asynchronoulsy in a background thread later.
     The SurfaceFlinger requires this behaviour since it releases the lock on all the
     SourceBuffers (Layers) after the compositionComplete() function returns.
     However this "bad" behaviour by SurfaceFlinger should not affect performance,
     since the Applications that render the SourceBuffers (Layers) still get the
     full renderpipeline using asynchronous rendering. So they perform at maximum speed,
     and because of their complexity compared to the Surface flinger jobs, the Surface flinger
     is normally faster even if it does everyhing synchronous and serial.
     */
    return 0;
}

int fb_device_open(hw_module_t const* module, const char* name,
                   hw_device_t** device)
{
    int status = -EINVAL;
#ifdef GRALLOC_16_BITS
    int bits_per_pixel = 16;
    int format = HAL_PIXEL_FORMAT_RGB_565;
#else
    int bits_per_pixel = 32;
#ifdef USE_BGRA_8888
    int format = HAL_PIXEL_FORMAT_BGRA_8888;
#else
    int format = HAL_PIXEL_FORMAT_RGBA_8888;
#endif
#endif

    //ALOGD("%s", __FUNCTION__);

    alloc_device_t* gralloc_device;
    status = gralloc_open(module, &gralloc_device);
    if (status < 0) {
        ALOGE("%s: Fail to Open gralloc device", __FUNCTION__);
        return status;
    }

    framebuffer_device_t *dev = (framebuffer_device_t *)malloc(sizeof(framebuffer_device_t));
    if (dev == NULL) {
        ALOGE("%s: Failed to allocate memory for dev", __FUNCTION__);
        gralloc_close(gralloc_device);
        return status;
    }

    private_module_t* m = (private_module_t*)module;
    status = init_fb(m);
    if (status < 0) {
        ALOGE("%s: Fail to init framebuffer", __FUNCTION__);
        free(dev);
        gralloc_close(gralloc_device);
        return status;
    }

    /* initialize our state here */
    memset(dev, 0, sizeof(*dev));

    /* initialize the procs */
    dev->common.tag = HARDWARE_DEVICE_TAG;
    dev->common.version = 0;
    dev->common.module = const_cast<hw_module_t*>(module);
    dev->common.close = fb_close;
    dev->setSwapInterval = fb_setSwapInterval;
    dev->post = fb_post;
    dev->setUpdateRect = 0;
    dev->compositionComplete = &compositionComplete;
    m->queue = new hwc_callback_queue_t;
    pthread_mutex_init(&m->queue_lock, NULL);

    int stride = m->line_length / (bits_per_pixel >> 3);
    const_cast<uint32_t&>(dev->flags) = 0;
    const_cast<uint32_t&>(dev->width) = m->xres;
    const_cast<uint32_t&>(dev->height) = m->yres;
    const_cast<int&>(dev->stride) = stride;
    const_cast<int&>(dev->format) = format;
    const_cast<float&>(dev->xdpi) = m->xdpi;
    const_cast<float&>(dev->ydpi) = m->ydpi;
    const_cast<float&>(dev->fps) = m->fps;
    const_cast<int&>(dev->minSwapInterval) = 1;
    const_cast<int&>(dev->maxSwapInterval) = 1;
    *device = &dev->common;
    status = 0;

    return status;
}
