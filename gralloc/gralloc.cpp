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

#include <limits.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>

#include <ion/ion.h>
#include <linux/ion.h>
#include <cutils/log.h>
#include <cutils/atomic.h>

#include <hardware/hardware.h>
#include <hardware/gralloc.h>

#include "gralloc_priv.h"
#include "exynos_format.h"
#include "ion_defs.h"

#define MB_1 (1024*1024)

#define GRALLOC_USAGE_FOREIGN_BUFFERS 0x00200000

/*
 * sRGB color pixel formats:
 *
 * The red, green and blue components are stored in sRGB space, and converted
 * to linear space when read, using the standard sRGB to linear equation:
 *
 * Clinear = Csrgb / 12.92                  for Csrgb <= 0.04045
 *         = (Csrgb + 0.055 / 1.055)^2.4    for Csrgb >  0.04045
 *
 * When written the inverse transformation is performed:
 *
 * Csrgb = 12.92 * Clinear                  for Clinear <= 0.0031308
 *       = 1.055 * Clinear^(1/2.4) - 0.055  for Clinear >  0.0031308
 *
 *
 *  The alpha component, if present, is always stored in linear space and
 *  is left unmodified when read or written.
 *
 */
#define HAL_PIXEL_FORMAT_sRGB_A_8888  0xC
#define HAL_PIXEL_FORMAT_sRGB_X_8888  0xD

/*****************************************************************************/

struct gralloc_context_t {
    alloc_device_t  device;
    /* our private data here */
};

static int gralloc_alloc_buffer(alloc_device_t* dev,
                                size_t size, int usage, buffer_handle_t* pHandle);

/*****************************************************************************/

int fb_device_open(const hw_module_t* module, const char* name,
                   hw_device_t** device);

static int gralloc_device_open(const hw_module_t* module, const char* name,
                               hw_device_t** device);

extern int gralloc_lock(gralloc_module_t const* module,
                        buffer_handle_t handle, int usage,
                        int l, int t, int w, int h,
                        void** vaddr);

extern int gralloc_unlock(gralloc_module_t const* module,
                          buffer_handle_t handle);

extern int gralloc_lock_ycbcr(gralloc_module_t const* module,
            buffer_handle_t handle, int usage,
            int l, int t, int w, int h,
            struct android_ycbcr *ycbcr);

extern int gralloc_register_buffer(gralloc_module_t const* module,
                                   buffer_handle_t handle);

extern int gralloc_unregister_buffer(gralloc_module_t const* module,
                                     buffer_handle_t handle);

extern int grallocUnmap(gralloc_module_t const* module, buffer_handle_t handle);

/*****************************************************************************/

static struct hw_module_methods_t gralloc_module_methods = {
open: gralloc_device_open
};

struct private_module_t HAL_MODULE_INFO_SYM = {
    base: {
        common: {
            tag: HARDWARE_MODULE_TAG,
            version_major: 1,
            version_minor: 0,
            id: GRALLOC_HARDWARE_MODULE_ID,
            name: "Graphics Memory Allocator Module",
            author: "The Android Open Source Project",
            methods: &gralloc_module_methods,
            .dso = NULL, /* remove compilation warnings */
            .reserved = {0}, /* remove compilation warnings */
        },
        registerBuffer: gralloc_register_buffer,
        unregisterBuffer: gralloc_unregister_buffer,
        lock: gralloc_lock,
        unlock: gralloc_unlock,
        perform: NULL, /* remove compilation warnings */
        lock_ycbcr: gralloc_lock_ycbcr,
        lockAsync: NULL, /* remove compilation warnings */
        unlockAsync: NULL, /* remove compilation warnings */
        lockAsync_ycbcr: NULL, /* remove compilation warnings */
        .reserved_proc = {0},
    },
    framebuffer: 0,
    flags: 0,
    numBuffers: 0,
    bufferMask: 0,
    lock: PTHREAD_MUTEX_INITIALIZER,
    refcount: 0,
    currentBuffer: 0,
    ionfd: -1,
    info: {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,{0}},
    finfo: {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,{0}},
    xres: 0,
    yres: 0,
    line_length: 0,
    xdpi: 0.0f,
    ydpi: 0.0f,
    fps: 0.0f,
	swapInterval: 0,
    fb_fd: 0,
    enableVSync: 0,
    queue: NULL,
    queue_lock: PTHREAD_MUTEX_INITIALIZER,
};

/*****************************************************************************/

static unsigned int _select_heap(int usage)
{
    unsigned int heap_mask;

    if (usage & GRALLOC_USAGE_PROTECTED) {
        if ( (usage & (GRALLOC_USAGE_HW_FIMC1 | GRALLOC_USAGE_HW_ION)) == GRALLOC_USAGE_HW_FIMC1)
            heap_mask = ION_HEAP_SYSTEM_MASK;
        else
            heap_mask = ION_HEAP_EXYNOS_CONTIG_MASK;
    } else if (usage & GRALLOC_USAGE_YUV_ADDR) {
        heap_mask = ION_HEAP_EXYNOS_CONTIG_MASK;
    } else {
        heap_mask = ION_HEAP_SYSTEM_MASK;
    }

    if (usage & GRALLOC_USAGE_PRIVATE_NONECACHE)
        return ION_HEAP_EXYNOS_CONTIG_MASK;
    else
        return heap_mask;
}

static int gralloc_alloc_rgb(int ionfd, int w, int h, int format, int usage,
                             unsigned int ion_flags, private_handle_t **hnd, int *stride)
{
    size_t size = 0, bpr, alignment = 0;
    int bpp = 0, vstride = 0, l_usage = usage, fd, err;
    unsigned int l_ion_flags = ion_flags;
    unsigned int heap_mask = _select_heap(usage);

    ALOGD_IF(DBG_LVL > 0, "%s fmt=0x%x usage=0x%x w=%d h=%d ion_flags=0x%x heap_mask=0x%x", __FUNCTION__,
          format, usage, w, h, ion_flags, heap_mask);

    switch (format) {
        case HAL_PIXEL_FORMAT_RGBA_8888: //0x1 ok
        case HAL_PIXEL_FORMAT_RGBX_8888: //0x2 ok
        case HAL_PIXEL_FORMAT_BGRA_8888: //0x5 ok
        case HAL_PIXEL_FORMAT_sRGB_A_8888: //0xC
        case HAL_PIXEL_FORMAT_sRGB_X_8888: //0xD
        case HAL_PIXEL_FORMAT_EXYNOS_ARGB_8888: //0x108
            bpp = 4;
            break;
        case HAL_PIXEL_FORMAT_RGB_888: //0x3
            bpp = 3;
            break;
        case HAL_PIXEL_FORMAT_RGB_565: //0x4
        case HAL_PIXEL_FORMAT_RAW16: //0x20
        case HAL_PIXEL_FORMAT_RAW_OPAQUE: //0x24
            bpp = 2;
            break;
        case HAL_PIXEL_FORMAT_BLOB: //0x21
            *stride = w;
            vstride = h;
            size = w * h;
            break;
        default:
            ALOGD_IF(DBG_LVL > 0, "%s format=0x%x won't be handled here...", __FUNCTION__, format);
            return -EINVAL;
    }

    if (format != HAL_PIXEL_FORMAT_BLOB) { // || (format == HAL_PIXEL_FORMAT_RGB_888)
        bpr = ALIGN(w*bpp, 16);
        vstride = ALIGN(h, 16);
        if (vstride < h + 2)
            size = bpr * (h + 2);
        else
            size = bpr * vstride;
        *stride = bpr / bpp;
        size += 256;
    }

    if (usage & GRALLOC_USAGE_PROTECTED) {
        if (usage & (GRALLOC_USAGE_HW_ION | GRALLOC_USAGE_HW_FIMC1) == (GRALLOC_USAGE_HW_ION | GRALLOC_USAGE_HW_FIMC1))
            l_ion_flags |= ION_EXYNOS_G2D_WFD_MASK;
        else {
            if (usage &  GRALLOC_USAGE_VIDEO_EXT)
                l_ion_flags |= ION_EXYNOS_VIDEO_EXT_MASK;
            else
                l_ion_flags |= ION_EXYNOS_VIDEO_MASK;
        }

        l_ion_flags |= ION_HEAP_EXYNOS_CONTIG_MASK;
    }

    err = ion_alloc_fd(ionfd, size, alignment, heap_mask, l_ion_flags, &fd);
    if (err) {
        if (usage & GRALLOC_USAGE_PRIVATE_NONECACHE) {
            l_usage = usage & ~GRALLOC_USAGE_PRIVATE_NONECACHE;
            err = ion_alloc_fd(ionfd, size, alignment, _select_heap(l_usage), l_ion_flags, &fd);
            if (err)
                return err;
        }
    }

    *hnd = new private_handle_t(fd, size, usage, w, h, format, format, *stride, vstride);

    return err;
}

static int gralloc_alloc_framework_yuv(int ionfd, int w, int h, int format, int format2,
                                       int usage, unsigned int ion_flags,
                                       private_handle_t **hnd, int *stride)
{
    size_t size;
    int err, fd;
    unsigned int heap_mask = _select_heap(usage);

    ALOGD_IF(DBG_LVL > 0, "%s fmt=0x%x fmt2=0x%x usage=0x%x w=%d h=%d ion_flags=0x%x  heap_mask=0x%x", __FUNCTION__,
          format, format2, usage, w, h, ion_flags, heap_mask);

    switch (format) {
        case HAL_PIXEL_FORMAT_YV12: //0x32315659
        case HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_P: //0x11f
            *stride = ALIGN(w, 16);
            size = (*stride * h) + (ALIGN(*stride / 2, 16) * h) + 256;
            break;
        case HAL_PIXEL_FORMAT_YCrCb_420_SP: //0x11
            *stride = w;
            size = (*stride * h * 3 / 2) + 256;
            break;
        default:
            ALOGE("%s: invalid yuv format %d\n", __FUNCTION__, format);
            return -EINVAL;
    }

    if (format2 == HAL_PIXEL_FORMAT_YCbCr_420_888) {
        *stride = 0;
    }

    err = ion_alloc_fd(ionfd, size, 0, heap_mask, ion_flags, &fd);
    if (err)
        return err;

    *hnd = new private_handle_t(fd, size, usage, w, h, format, format2, *stride, h);

    return err;
}

static int gralloc_alloc_yuv(int ionfd, int w, int h, int format,
                             int usage, unsigned int ion_flags,
                             private_handle_t **hnd, int *stride)
{
    size_t luma_size, chroma_size;
    int err, planes, fd, fd1, fd2 = 0, format2 = format;
    size_t luma_vstride;
    unsigned int heap_mask = _select_heap(usage);

    ALOGD_IF(DBG_LVL > 0, "%s fmt=0x%x usage=0x%x w=%d h=%d ion_flags=0x%x heap_mask=0x%x", __FUNCTION__,
          format, usage, w, h, ion_flags, heap_mask);

    *stride = ALIGN(w, 16);

    if (format == HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED) {
        ALOGD_IF(DBG_LVL > 0, "HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED : usage(%x), flags(%x)\n", usage, ion_flags);

        if ((usage & GRALLOC_USAGE_HW_CAMERA_ZSL) == GRALLOC_USAGE_HW_CAMERA_ZSL) {
            format2 = HAL_PIXEL_FORMAT_YCbCr_422_I; // YUYV
        } else {
            if (usage & (GRALLOC_USAGE_HW_VIDEO_ENCODER | GRALLOC_USAGE_HW_TEXTURE)) {
                format2 = HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M; // NV12M
            } else {
                format2 = HAL_PIXEL_FORMAT_IMPLEMENTATION_DEFINED;
            }
        }
    } else {
        if (format == HAL_PIXEL_FORMAT_YCbCr_420_888) { //0x23
            if (usage & GRALLOC_USAGE_HW_CAMERA_WRITE) {
                format2 = HAL_PIXEL_FORMAT_YCrCb_420_SP;
            } else {
                ALOGD("%s: HAL_PIXEL_FORMAT_YCbCr_420_888", __FUNCTION__);
            }
        }
    }

    if (usage & GRALLOC_USAGE_PROTECTED) {
        if (usage & GRALLOC_USAGE_FOREIGN_BUFFERS) {
            ion_flags |= ION_EXYNOS_VIDEO_EXT_MASK;
        } else {
            ion_flags |= ION_EXYNOS_VIDEO_MASK;
        }
        ion_flags |= ION_HEAP_EXYNOS_CONTIG_MASK;
    } else {
        if (usage & GRALLOC_USAGE_PRIVATE_2) {
            ion_flags |= ION_EXYNOS_VIDEO_MASK;
        }
    }

    switch (format2) {
         case HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M: //0x11d
         case HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M: //0x105
         case HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL: //0x11e
            {
                luma_vstride = ALIGN(h, 16);
                luma_size = (luma_vstride * *stride) + 256;
                size_t chroma_vstride = luma_vstride / 2;
                chroma_size = (chroma_vstride * *stride) + 256;
                planes = 2;
                break;
            }
        case HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_TILED: //0x107
            {
                size_t chroma_vstride = ALIGN(h / 2, 32);
                luma_vstride = ALIGN(h, 32);
                luma_size = (luma_vstride * *stride) + 256;
                chroma_size = (chroma_vstride * *stride) + 256;
                planes = 2;
                break;
            }
        case HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_P_M: //0x101
        case HAL_PIXEL_FORMAT_EXYNOS_YV12_M: //0x11c
            {
                *stride = ALIGN(w, 32);
                luma_vstride = ALIGN(h, 16);
                luma_size = (luma_vstride * *stride) + 256;
                chroma_size = ( ALIGN( *stride / 2, 16) * (luma_vstride / 2) ) + 256;
                planes = 3;
                break;
            }
        case HAL_PIXEL_FORMAT_YV12: //0x32315659
        case HAL_PIXEL_FORMAT_YCrCb_420_SP: //0x11
            return gralloc_alloc_framework_yuv(ionfd, w, h, format, format2, usage,
                                               ion_flags, hnd, stride);
        case HAL_PIXEL_FORMAT_YCbCr_422_I: //0x14
            {
                luma_vstride = h;
                luma_size = (luma_vstride * *stride * 2) + 256;
                chroma_size = 0;
                planes = 1;
                break;
            }
        default:
            ALOGE("%s invalid yuv format %d\n", __FUNCTION__, format);
            return -EINVAL;
    }

    err = ion_alloc_fd(ionfd, luma_size, 0, heap_mask, ion_flags, &fd);
    if (err)
        return err;
    if (planes == 1) {
         ALOGD_IF(DBG_LVL > 0, "%s planes=%d size=%d", __FUNCTION__, planes, luma_size);
        *hnd = new private_handle_t(fd, luma_size, usage, w, h,
                                    format, format2, *stride, luma_vstride);
    } else {
        err = ion_alloc_fd(ionfd, chroma_size, 0, heap_mask, ion_flags, &fd1);
        if (err)
            goto err1;
        if (planes == 3) {
            ALOGD_IF(DBG_LVL > 0, "%s planes=%d luma_size=%d chroma_size=%d", __FUNCTION__, planes, luma_size, chroma_size);
            err = ion_alloc_fd(ionfd, chroma_size, 0, heap_mask, ion_flags, &fd2);
            if (err)
                goto err2;

            *hnd = new private_handle_t(fd, fd1, fd2, luma_size, usage, w, h,
                                        format, format2, *stride, luma_vstride);
        } else {
            ALOGD_IF(DBG_LVL > 0, "%s planes=%d luma_size=%d chroma_size=%d", __FUNCTION__, planes, luma_size, chroma_size);
            *hnd = new private_handle_t(fd, fd1, luma_size, usage, w, h, format,
                                        format2, *stride, luma_vstride);
        }
    }
    return err;

err2:
    close(fd1);
err1:
    close(fd);
    return err;
}

static int gralloc_alloc(alloc_device_t* dev,
                         int w, int h, int format, int usage,
                         buffer_handle_t* pHandle, int* pStride)
{
    int stride = 0;
    int err;
    unsigned int ion_flags = 0;
    int l_usage = usage;
    private_handle_t *hnd = NULL;

    if (!pHandle || !pStride || w < 0 || h < 0)
        return -EINVAL;

    private_module_t* m = reinterpret_cast<private_module_t*>
        (dev->common.module);
    gralloc_module_t* module = reinterpret_cast<gralloc_module_t*>
        (dev->common.module);

    ALOGD_IF(DBG_LVL > 0, "%s fmt=0x%x usage=0x%x w=%d h=%d", __FUNCTION__,
          format, usage, w, h);

    if ( (l_usage & GRALLOC_USAGE_SW_READ_MASK) == GRALLOC_USAGE_SW_READ_OFTEN ) {
        ion_flags = ION_FLAG_CACHED | ION_FLAG_CACHED_NEEDS_SYNC;
#if defined(__aarch64__)
        if (l_usage & GRALLOC_USAGE_HW_RENDER)
            ion_flags |= 0x20;
#endif
    }

    if (l_usage & GRALLOC_USAGE_CAMERA)
        ion_flags |= ION_FLAG_NOZEROED;

    if (l_usage & GRALLOC_USAGE_PRIVATE_NONECACHE) {
        if ((w * h) != (m->xres * m->yres)) /* TODO: check this */
            l_usage |= ~GRALLOC_USAGE_PRIVATE_NONECACHE;
    }

    err = gralloc_alloc_rgb(m->ionfd, w, h, format, l_usage, ion_flags, &hnd,
                            &stride);
    if (err)
        err = gralloc_alloc_yuv(m->ionfd, w, h, format, l_usage, ion_flags,
                                &hnd, &stride);
    if (err)
        goto err;

/*    err = gralloc_register_buffer(module, hnd);
    if (err)
        goto err;*/

    *pHandle = hnd;
    *pStride = stride;
    return 0;
err:
    if (!hnd)
        return err;
    close(hnd->fd);
    if (hnd->fd1 >= 0)
        close(hnd->fd1);
    if (hnd->fd2 >= 0)
        close(hnd->fd2);
    return err;
}

static int gralloc_free(alloc_device_t* dev,
                        buffer_handle_t handle)
{
    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    ALOGD_IF(DBG_LVL > 0, "%s", __FUNCTION__);

    private_handle_t const* hnd = reinterpret_cast<private_handle_t const*>(handle);
    gralloc_module_t* module = reinterpret_cast<gralloc_module_t*>(dev->common.module);

    if (!hnd->base)
        grallocUnmap(module, hnd);

    close(hnd->fd);
    if (hnd->fd1 >= 0)
        close(hnd->fd1);
    if (hnd->fd2 >= 0)
        close(hnd->fd2);

    delete hnd;
    return 0;
}

/*****************************************************************************/

static int gralloc_close(struct hw_device_t *dev)
{
    gralloc_context_t* ctx = reinterpret_cast<gralloc_context_t*>(dev);

    ALOGD_IF(DBG_LVL > 0, "%s", __FUNCTION__);

    if (ctx) {
        free(ctx);
    }
    return 0;
}

int gralloc_device_open(const hw_module_t* module, const char* name,
                        hw_device_t** device)
{
    int status = -EINVAL;

#if defined(__aarch64__)
    ALOGD_IF(DBG_LVL > 0, "%s 64 bit name=%s", __FUNCTION__, name);
#else
    ALOGD_IF(DBG_LVL > 0, "%s 32 bit name=%s", __FUNCTION__, name);
#endif

    //ALOGD("%s sizeof(private_handle_t)=%d sizeof(native_handle_t)=%d", __FUNCTION__, sizeof(private_handle_t), sizeof(native_handle_t));
    //ALOGD("%s sNumInts=%d needed sNumInts=%d", __FUNCTION__, private_handle_t::sNumInts(), (((sizeof(private_handle_t) - sizeof(native_handle_t))/sizeof(int)) - private_handle_t::sNumFds));

    if (!strcmp(name, GRALLOC_HARDWARE_GPU0)) {
        gralloc_context_t *dev;
        dev = (gralloc_context_t*)malloc(sizeof(*dev));

        /* initialize our state here */
        memset(dev, 0, sizeof(*dev));

        /* initialize the procs */
        dev->device.common.tag = HARDWARE_DEVICE_TAG;
        dev->device.common.version = 0;
        dev->device.common.module = const_cast<hw_module_t*>(module);
        dev->device.common.close = gralloc_close;

        dev->device.alloc = gralloc_alloc;
        dev->device.free = gralloc_free;

        private_module_t *p = reinterpret_cast<private_module_t*>(dev->device.common.module);
        if (p->ionfd == -1)
            p->ionfd = ion_open();

        ALOGD_IF(DBG_LVL > 0, "%s ionfd=%d", __FUNCTION__, p->ionfd);

        *device = &dev->device.common;
        status = 0;
    } else {
        status = fb_device_open(module, name, device);
    }
    return status;
}
