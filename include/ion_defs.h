/*
 * Copyright (C) 2012 Samsung Electronics Co., Ltd.
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

#ifndef _LIB_ION_DEFS_H_
#define _LIB_ION_DEFS_H_

/**
 * allocation flags - the lower 16 bits are used by core ion, the upper 16
 * bits are reserved for use by the heaps themselves.
 */
#define ION_FLAG_CACHED 1               /* mappings of this buffer should be
                                           cached, ion will do cache
                                           maintenance when the buffer is
                                           mapped for dma */
#define ION_FLAG_CACHED_NEEDS_SYNC 2    /* mappings of this buffer will created
                                           at mmap time, if this is set
                                           caches must be managed manually */
#define ION_FLAG_PRESERVE_KMAP 4        /* deprecated. ignored. */
#define ION_FLAG_NOZEROED 8             /* Allocated buffer is not initialized
                                           with zero value and userspace is not
                                           able to access the buffer
                                         */
#define ION_FLAG_PROTECTED 16           /* this buffer would be used in secure
                                           world. if this is set, all cpu accesses
                                           are prohibited.
                                         */

#define ION_HEAP_SYSTEM_ID          0
#define ION_HEAP_EXYNOS_CONTIG_ID   4
#define ION_HEAP_CHUNK_ID           6

//#define ION_HEAP_SYSTEM_MASK            (1 << 0)
//#define ION_HEAP_SYSTEM_CONTIG_MASK     (1 << 1)
#define ION_HEAP_EXYNOS_CONTIG_MASK     (1 << 4)
#define ION_HEAP_EXYNOS_MASK            (1 << 5)

#define ION_EXYNOS_VIDEO_MASK         (1 << 21)
#define ION_EXYNOS_G2D_WFD_MASK       (1 << 22)
#define ION_EXYNOS_MFC_INPUT_MASK     (1 << 25)
#define ION_EXYNOS_MFC_OUTPUT_MASK    (1 << 26)
#define ION_EXYNOS_GSC_MASK           (1 << 27)
#define ION_EXYNOS_FIMD_VIDEO_MASK    (1 << 28)
#define ION_EXYNOS_VIDEO_EXT_MASK     (1 << 31)


/* ION_MSYNC_FLAGS
 * values of @flags parameter to ion_msync()
 *
 * IMSYNC_DEV_TO_READ: Device only reads the buffer
 * IMSYNC_DEV_TO_WRITE: Device may writes to the buffer
 * IMSYNC_DEV_TO_RW: Device reads and writes to the buffer
 *
 * IMSYNC_SYNC_FOR_DEV: ion_msync() for device to access the buffer
 * IMSYNC_SYNC_FOR_CPU: ion_msync() for CPU to access the buffer after device
 *                      has accessed it.
 *
 * The values must be ORed with one of IMSYNC_DEV_* and one of IMSYNC_SYNC_*.
 * Otherwise, ion_msync() will not effect.
 */
enum ION_MSYNC_FLAGS {
    IMSYNC_DEV_TO_READ = 0,
    IMSYNC_DEV_TO_WRITE = 1,
    IMSYNC_DEV_TO_RW = 2,
    IMSYNC_SYNC_FOR_DEV = 0x10000,
    IMSYNC_SYNC_FOR_CPU = 0x20000,
};

#endif /* _LIB_ION_DEFS_H_ */
