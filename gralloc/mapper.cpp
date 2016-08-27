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
#include <errno.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>

#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <cutils/log.h>
#include <cutils/atomic.h>

#include <hardware/hardware.h>
#include <hardware/gralloc.h>

#include "gralloc_priv.h"

#include <ion/ion.h>
#include <linux/ion.h>

#include "exynos_format.h"

/*****************************************************************************/

static int c_size_get(private_handle_t* hnd)
{
    int size = 0, stride = 0, vstride = 0;

    size = hnd->size;

    int val = hnd->format - 0x101;

    switch(hnd->format) {
    case HAL_PIXEL_FORMAT_RGBA_8888:
    case HAL_PIXEL_FORMAT_EXYNOS_YV12_M:
    case HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_P_M:
        vstride = hnd->vstride / 2;
        stride = ALIGN(hnd->stride / 2, 16);
        size = (vstride * stride) + 256;
        break;

    case HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M_FULL:
    case HAL_PIXEL_FORMAT_EXYNOS_YCrCb_420_SP_M:
    case HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M:
        vstride = ALIGN(hnd->vstride / 2, 8);
        stride = hnd->stride;
        size = (vstride * stride) + 256;
        break;

    case HAL_PIXEL_FORMAT_EXYNOS_YCbCr_420_SP_M_TILED:
        vstride = ALIGN(hnd->height / 2, 32);
        stride = hnd->stride;
        size = (vstride * stride) + 256;
        break;

    default:
        size = 0;
    }

    ALOGD_IF(DBG_LVL > 0, "%s fmt=0x%x size=%d val=%d 1<<val=0x%x", __FUNCTION__,
           hnd->format, size, val, 1<<val);
    return size;
}

int getIonFd(gralloc_module_t const *module)
{
    private_module_t* m = const_cast<private_module_t*>(reinterpret_cast<const private_module_t*>(module));
    if (m->ionfd == -1)
        m->ionfd = ion_open();
    return m->ionfd;
}

int grallocMap(gralloc_module_t const* module, buffer_handle_t handle)
{
    int size = 0;
    private_handle_t* hnd = (private_handle_t*)handle;

    ALOGD_IF(DBG_LVL > 0, "%s flags=0x%x", __FUNCTION__, hnd->flags);

    size = c_size_get(hnd);

    if (hnd->flags & (GRALLOC_USAGE_HW_FIMC1 | GRALLOC_USAGE_PROTECTED) == GRALLOC_USAGE_PROTECTED) {
        ALOGD("%s protected mode, return 0", __FUNCTION__);
        return 0;
    }

    if (hnd->flags & (GRALLOC_USAGE_CAMERA | GRALLOC_USAGE_PROTECTED)) {
        hnd->base = 0;
        hnd->base1 = 0;
        hnd->base2 = 0;
        ALOGD("%s camera or protected mode, return 0", __FUNCTION__);
        return 0;
    }

    void* mappedAddress = mmap(0, hnd->size, PROT_READ|PROT_WRITE, MAP_SHARED,
                               hnd->fd, 0);
    if (mappedAddress == MAP_FAILED) {
        ALOGE("%s: could not mmap %s", __FUNCTION__, strerror(errno));
        return -errno;
    }

    ALOGD("%s: base %p %d %d %d %d\n", __FUNCTION__, mappedAddress, hnd->size,
          hnd->width, hnd->height, hnd->stride);

    hnd->base = (uint64_t) mappedAddress;
    ion_sync_fd(getIonFd(module), hnd->fd);

    if (hnd->fd1 >= 0) {
        mappedAddress = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED,
                             hnd->fd1, 0);
        /*if (mappedAddress == MAP_FAILED) {
            ALOGE("%s: could not mmap base1 %s", __func__, strerror(errno));
            return -errno;
        }*/

        ALOGD("%s: base1 %p %d %d %d %d\n", __FUNCTION__, mappedAddress, size,
              hnd->width, hnd->height, hnd->stride);

        hnd->base1 = (uint64_t) mappedAddress;
        ion_sync_fd(getIonFd(module), hnd->fd1);
    }

    if (hnd->fd2 >= 0) {
        mappedAddress = mmap(0, size, PROT_READ|PROT_WRITE, MAP_SHARED,
                             hnd->fd2, 0);
        /*if (mappedAddress == MAP_FAILED) {
            ALOGE("%s: could not mmap base2 %s", __func__, strerror(errno));
            return -errno;
        }*/

        ALOGD("%s: base2 %p %d %d %d %d\n", __FUNCTION__, mappedAddress, size,
              hnd->width, hnd->height, hnd->stride);

        hnd->base2 = (uint64_t) mappedAddress;
        ion_sync_fd(getIonFd(module), hnd->fd2);
    }

    return 0;
}

int grallocUnmap(gralloc_module_t const* module, buffer_handle_t handle)
{
    int size = 0;
    private_handle_t* hnd = (private_handle_t*)handle;

    ALOGD_IF(DBG_LVL > 0, "%s hnd->base=0x%x", __FUNCTION__, hnd->base);

    if (!hnd->base)
        return 0;

    size = c_size_get(hnd);

    if (munmap((void *) hnd->base, hnd->size) < 0) {
        ALOGE("%s :could not unmap %s %p %d", __func__, strerror(errno),
              hnd->base, hnd->size);
    }

    ALOGD_IF(DBG_LVL > 0, "%s: base %p %d %d %d %d\n", __func__, hnd->base, hnd->size,
          hnd->width, hnd->height, hnd->stride);

    hnd->base = 0;

    if ((hnd->fd1 >= 0) && (hnd->base1)) {
        if (munmap((void *) hnd->base1, size) < 0) {
            ALOGE("%s :could not unmap base1 %s %p %d", __func__, strerror(errno),
                  hnd->base1, size);
        }

        hnd->base1 = 0;
    }

    if ((hnd->fd2 >= 0) && (hnd->base2)) {
        if (munmap((void *) hnd->base2, size) < 0) {
            ALOGE("%s :could not unmap base2 %s %p %d", __func__, strerror(errno),
                  hnd->base2, size);
        }

        hnd->base2 = 0;
    }

    return 0;
}

/*****************************************************************************/

static pthread_mutex_t sMapLock = PTHREAD_MUTEX_INITIALIZER;

/*****************************************************************************/

int gralloc_register_buffer(gralloc_module_t const* module,
                            buffer_handle_t handle)
{
    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    private_handle_t* hnd = (private_handle_t*)handle;

    ALOGD_IF(DBG_LVL > 0, "%s: base %p %d %d %d %d\n", __func__, hnd->base, hnd->size,
          hnd->width, hnd->height, hnd->stride);

    int mapret;
    mapret = grallocMap(module,handle);

    int ret;
    ret = ion_import(getIonFd(module), hnd->fd, &hnd->handle);
    if (ret)
        ALOGE("%s: error importing handle %d %x\n", __FUNCTION__, hnd->fd, hnd->format);
    if (hnd->fd1 >= 0) {
        ret = ion_import(getIonFd(module), hnd->fd1, &hnd->handle1);
        if (ret)
            ALOGE("%s: error importing handle1 %d %x\n", __FUNCTION__, hnd->fd1, hnd->format);
    }
    if (hnd->fd2 >= 0) {
        ret = ion_import(getIonFd(module), hnd->fd2, &hnd->handle2);
        if (ret)
            ALOGE("%s: error importing handle2 %d %x\n", __FUNCTION__, hnd->fd2, hnd->format);
    }

    return mapret;
}

int gralloc_unregister_buffer(gralloc_module_t const* module,
                              buffer_handle_t handle)
{
    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    private_handle_t* hnd = (private_handle_t*)handle;

    ALOGD_IF(DBG_LVL > 0, "%s: base %p %d %d %d %d\n", __func__, hnd->base, hnd->size,
          hnd->width, hnd->height, hnd->stride);

    grallocUnmap(module, handle);

    if (hnd->handle)
        ion_free(getIonFd(module), hnd->handle);
    if (hnd->handle1)
        ion_free(getIonFd(module), hnd->handle1);
    if (hnd->handle2)
        ion_free(getIonFd(module), hnd->handle2);

    return 0;
}

int gralloc_lock(gralloc_module_t const* module,
                 buffer_handle_t handle, int usage,
                 int l, int t, int w, int h,
                 void** vaddr)
{
    // this is called when a buffer is being locked for software
    // access. in thin implementation we have nothing to do since
    // not synchronization with the h/w is needed.
    // typically this is used to wait for the h/w to finish with
    // this buffer if relevant. the data cache may need to be
    // flushed or invalidated depending on the usage bits and the
    // hardware.

    ALOGD_IF(DBG_LVL > 0, "%s usage=0x%x l=%d t=%d w=%d h=%d", __FUNCTION__,
          usage, l, t, w, h);

    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    private_handle_t* hnd = (private_handle_t*)handle;

    if (hnd->format2 == HAL_PIXEL_FORMAT_YCbCr_420_888) {
        ALOGE("%s: Format %x should be locked with lock_ycbcr", __FUNCTION__);
        return -EINVAL;
    }

    if (!hnd->base)
        grallocMap(module, hnd);

    vaddr[0] = (void *)hnd->base;

    if (hnd->fd1 >= 0)
        vaddr[1] = (void *)hnd->base1;

    if (hnd->fd2 >= 0)
        vaddr[2] = (void *)hnd->base2;

    return 0;
}

int gralloc_unlock(gralloc_module_t const* module,
                   buffer_handle_t handle)
{
    // we're done with a software buffer. nothing to do in this
    // implementation. typically this is used to flush the data cache.
    private_handle_t* hnd = (private_handle_t*)handle;

    ALOGD_IF(DBG_LVL > 0, "%s", __FUNCTION__);

    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    if (hnd->flags & GRALLOC_USAGE_SW_READ_MASK == GRALLOC_USAGE_SW_READ_OFTEN) {
        ion_sync_fd(getIonFd(module), hnd->fd);
        if (hnd->fd1 >= 0)
            ion_sync_fd(getIonFd(module), hnd->fd1);
        if (hnd->fd2 >= 0)
            ion_sync_fd(getIonFd(module), hnd->fd2);
    }

    return 0;
}

int gralloc_lock_ycbcr(gralloc_module_t const* module,
            buffer_handle_t handle, int usage,
            int l, int t, int w, int h,
            struct android_ycbcr *ycbcr)
{
    ALOGD_IF(DBG_LVL > 0, "%s", __FUNCTION__);

    if (private_handle_t::validate(handle) < 0)
        return -EINVAL;

    private_handle_t* hnd = (private_handle_t*)handle;

    if (!ycbcr) {
        ALOGE("%s empty ycbcr", __FUNCTION__);
        return -EINVAL;
    }

    if (hnd->format2 != HAL_PIXEL_FORMAT_YCbCr_420_888) {
        ALOGE("%s format2 is not HAL_PIXEL_FORMAT_YCbCr_420_888", __FUNCTION__);
        return -EINVAL;
    }

    if (hnd->flags & (GRALLOC_USAGE_HW_CAMERA_WRITE | GRALLOC_USAGE_SW_READ_MASK)) {
        if (hnd->format == HAL_PIXEL_FORMAT_YCrCb_420_SP) {
            memset(ycbcr, 0, sizeof(struct android_ycbcr));

            ycbcr->y = (void *) hnd->base;
            ycbcr->cb = (void *) (hnd->base + (hnd->width * hnd->height) + 1);
            ycbcr->cr = (void *) (hnd->base + (hnd->width * hnd->height));
            ycbcr->ystride = hnd->width;
            ycbcr->cstride = hnd->width;
            ycbcr->chroma_step = 2;

            ALOGD_IF(DBG_LVL > 0, "%s flags=0x%x y=0x%x cb=0x%x cr=0x%x", __FUNCTION__, hnd->flags, ycbcr->y, ycbcr->cb, ycbcr->cr);
        } else {
            ALOGE("%s wrong format %d", __FUNCTION__, hnd->format);
            return -EINVAL;
        }
    } else {
        ALOGE("%s wrong flags 0x%x", __FUNCTION__, hnd->flags);
        return -EINVAL;
    }

    return 0;
}
