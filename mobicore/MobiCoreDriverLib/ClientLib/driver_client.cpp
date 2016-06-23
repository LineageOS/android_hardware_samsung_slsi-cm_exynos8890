/*
 * Copyright (c) 2013-2015 TRUSTONIC LIMITED
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the TRUSTONIC LIMITED nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <vector>

#include <errno.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <pthread.h>

#include <MobiCoreDriverApi.h>  // mcResult_t
#include <tee_client_api.h>     // TEEC_UUID
#include <GpTci.h>              // _TEEC_TCI
#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "McDriverClient"
#include "log.h"
#include "ClientLib.h"
#include "driver_client.h"

/* Convert TEE errors into errno */
static int mcresult_to_errno[] = {
    0,                  // MC_DRV_OK                             0
    ENOMSG,             // MC_DRV_NO_NOTIFICATION                1
    EBADMSG,            // MC_DRV_ERR_NOTIFICATION               2
    ENOLINK,            // MC_DRV_ERR_NOT_IMPLEMENTED            3
    ENOSPC,             // MC_DRV_ERR_OUT_OF_RESOURCES           4
    EHOSTDOWN,          // MC_DRV_ERR_INIT                       5
    ENOLINK,            // MC_DRV_ERR_UNKNOWN                    6
    ENODEV,             // MC_DRV_ERR_UNKNOWN_DEVICE             7
    ENXIO,              // MC_DRV_ERR_UNKNOWN_SESSION            8
    EPERM,              // MC_DRV_ERR_INVALID_OPERATION          9
    EBADE,              // MC_DRV_ERR_INVALID_RESPONSE          10
    ETIME,              // MC_DRV_ERR_TIMEOUT                   11
    ENOMEM,             // MC_DRV_ERR_NO_FREE_MEMORY            12
    EUCLEAN,            // MC_DRV_ERR_FREE_MEMORY_FAILED        13
    ENOTEMPTY,          // MC_DRV_ERR_SESSION_PENDING           14
    EHOSTUNREACH,       // MC_DRV_ERR_DAEMON_UNREACHABLE        15
    ENOENT,             // MC_DRV_ERR_INVALID_DEVICE_FILE       16
    EINVAL,             // MC_DRV_ERR_INVALID_PARAMETER         17
    EPROTO,             // MC_DRV_ERR_KERNEL_MODULE             18
    EADDRINUSE,         // MC_DRV_ERR_BULK_MAPPING              19
    EADDRNOTAVAIL,      // MC_DRV_ERR_BULK_UNMAPPING            20
    ECOMM,              // MC_DRV_INFO_NOTIFICATION             21
    EUNATCH,            // MC_DRV_ERR_NQ_FAILED                 22
    ENOLINK,            // MC_DRV_ERR_DAEMON_VERSION            23
    ENOLINK,            // MC_DRV_ERR_CONTAINER_VERSION         24
    EKEYREJECTED,       // MC_DRV_ERR_WRONG_PUBLIC_KEY          25
    ENOLINK,            // MC_DRV_ERR_CONTAINER_TYPE_MISMATCH   26
    ENOLINK,            // MC_DRV_ERR_CONTAINER_LOCKED          27
    ENOLINK,            // MC_DRV_ERR_SP_NO_CHILD               28
    ENOLINK,            // MC_DRV_ERR_TL_NO_CHILD               29
    ENOLINK,            // MC_DRV_ERR_UNWRAP_ROOT_FAILED        30
    ENOLINK,            // MC_DRV_ERR_UNWRAP_SP_FAILED          31
    ENOLINK,            // MC_DRV_ERR_UNWRAP_TRUSTLET_FAILED    32
    EBADF,              // MC_DRV_ERR_DAEMON_DEVICE_NOT_OPEN    33
    ENOLINK,            // MC_DRV_ERR_TA_ATTESTATION_ERROR      34
    EINTR,              // MC_DRV_ERR_INTERRUPTED_BY_SIGNAL     35
    ECONNREFUSED,       // MC_DRV_ERR_SERVICE_BLOCKED           36
    ECONNABORTED,       // MC_DRV_ERR_SERVICE_LOCKED            37
    ECONNRESET,         // MC_DRV_ERR_SERVICE_KILLED            38
    ENOLINK,            // MC_DRV_ERR_NO_FREE_INSTANCES         39
};

static int set_errno_from_mcresult(mcResult_t result) {
    // Out of range, analyse some special cases
    if (result > 39) {
        switch (result) {
            case 0xe0012:
#if ( __WORDSIZE == 64 )
                errno = EINVAL;
#else
                errno = ENOMEM;
#endif
                break;
            default:
                errno = EBADSLT;
        }
        return -1;
    }
    int local_errno = mcresult_to_errno[result];
    if (local_errno > 0) {
        errno = local_errno;
        return -1;
    }
    return 0;
}

struct DriverClient::Impl {
    struct GpSession {
        uint32_t id;
        _TEEC_TCI* tci;
        GpSession(uint32_t i, uint64_t t): id(i) {
            tci = reinterpret_cast<_TEEC_TCI*>(static_cast<uintptr_t>(t));
        }
    };
    pthread_mutex_t gp_sessions_mutex;
    std::vector<GpSession*> gp_sessions;
    Device* device;
    Impl(): device(nullptr) {
        ::pthread_mutex_init(&gp_sessions_mutex, nullptr);
    }
    ~Impl() {
        delete device;
        ::pthread_mutex_destroy(&gp_sessions_mutex);
    }
};

DriverClient::DriverClient(): pimpl_(new Impl) {
}

DriverClient::~DriverClient() {
    delete pimpl_;
}

int DriverClient::open() {
    int ret = set_errno_from_mcresult(mcOpenDevice_(&pimpl_->device));
    if (ret) {
        _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
        return ret;
    }
    LOG_I("driver client open");
    return 0;
}

int DriverClient::close() {
    mcResult_t mc_result = mcCloseDevice_(pimpl_->device);
    if (mc_result != MC_DRV_ERR_SESSION_PENDING) {
        // Device closed
        delete pimpl_->device;
        pimpl_->device = nullptr;
    }
    int ret = set_errno_from_mcresult(mc_result);
    if (ret) {
        _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
        return ret;
    }
    LOG_I("driver client closed");
    return 0;
}

bool DriverClient::isOpen() const {
    return pimpl_->device && pimpl_->device->isValid();
}

int DriverClient::hasOpenSessions() const {
    if (!pimpl_->device->isValid()) {
        // Connection dead
        return 0;
    }
    return set_errno_from_mcresult(mcDeviceHasOpenSessions_(pimpl_->device));
}

int DriverClient::openSession(struct mc_ioctl_open_session& session) {
    if (session.identity.login_type != LOGIN_PUBLIC) {
        errno = EINVAL;
        _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
        return -1;
    }
    mcResult_t mc_result;
    auto uuid = reinterpret_cast<const mcUuid_t*>(&session.uuid);
    auto tci = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(session.tci));
    if (session.is_gp_uuid) {
        mc_result = mcOpenGPTA_(pimpl_->device, &session.sid, uuid, tci, session.tcilen);
    } else {
        mc_result = mcOpenSession_(pimpl_->device, &session.sid, uuid, tci, session.tcilen);
    }
    int ret = set_errno_from_mcresult(mc_result);
    if (ret) {
        _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
        return ret;
    }
    if (session.is_gp_uuid) {
        pthread_mutex_lock(&pimpl_->gp_sessions_mutex);
        pimpl_->gp_sessions.push_back(
            new Impl::GpSession(session.sid, session.tci));
        pthread_mutex_unlock(&pimpl_->gp_sessions_mutex);
    }
    return ret;
}

int DriverClient::openTrustlet(struct mc_ioctl_open_trustlet& trustlet) {
    mcResult_t mc_result;
    auto trustapp = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(trustlet.buffer));
    auto tci = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(trustlet.tci));
    mc_result = mcOpenTrustlet_(pimpl_->device, &trustlet.sid, trustlet.spid,
                                trustapp, trustlet.tlen, tci, trustlet.tcilen);
    int ret = set_errno_from_mcresult(mc_result);
    if (ret) {
        _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
        return ret;
    }
    return ret;
}

int DriverClient::closeSession(uint32_t session_id) {
    mcResult_t mc_result = mcCloseSession_(pimpl_->device, session_id);
    int ret = set_errno_from_mcresult(mc_result);
    if (ret) {
        _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
        return ret;
    }
    pthread_mutex_lock(&pimpl_->gp_sessions_mutex);
    for (auto it = pimpl_->gp_sessions.begin();
            it != pimpl_->gp_sessions.end(); it++) {
        auto gp_session = *it;
        if (gp_session->id == session_id) {
            pimpl_->gp_sessions.erase(it);
            delete gp_session;
            break;
        }
    }
    pthread_mutex_unlock(&pimpl_->gp_sessions_mutex);
    return ret;
}

int DriverClient::notify(uint32_t session_id) {
    mcResult_t mc_result = mcNotify_(pimpl_->device, session_id);
    int ret = set_errno_from_mcresult(mc_result);
    if (ret) {
        _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
    }
    return ret;
}

int DriverClient::waitNotification(const struct mc_ioctl_wait& wait) {
    mcResult_t mc_result = mcWaitNotification_(pimpl_->device, wait.sid, wait.timeout);
    int ret = set_errno_from_mcresult(mc_result);
    if (ret) {
        _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
    }
    return ret;
}

int DriverClient::malloc(uint8_t **buffer, uint32_t length) {
    mcResult_t mc_result = mcMallocWsm_(pimpl_->device, length, buffer);
    int ret = set_errno_from_mcresult(mc_result);
    if (ret) {
        _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
    }
    return ret;
}

int DriverClient::free(uint8_t *buffer, uint32_t) {
    mcResult_t mc_result = mcFreeWsm_(pimpl_->device, buffer);
    int ret = set_errno_from_mcresult(mc_result);
    if (ret) {
        _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
    }
    return ret;
}

int DriverClient::map(struct mc_ioctl_map& map) {
    int ret = 0;
    bool mapped = false;
    int i;
    for (i = 0; i < MC_MAP_MAX; i++) {
        if (!map.bufs[i].va) {
            continue;
        }
        if (ret) {
            // Error reported, make sure we don't unmap skipped buffers
            map.bufs[i].va = 0;
            continue;
        }
        mcBulkMap_t map_handle;
        auto buf = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(map.bufs[i].va));
        mcResult_t mc_result = mcMap_(pimpl_->device, map.sid, buf, map.bufs[i].len, &map_handle);
        ret = set_errno_from_mcresult(mc_result);
        if (ret) {
            _LOG_E("%s in %s, buffer #%d", strerror(errno), __FUNCTION__, i);
            continue;
        }
        mapped = true;
#if ( __WORDSIZE != 64 )
        map.bufs[i].sva = reinterpret_cast<uintptr_t>(map_handle.sVirtualAddr);
#else
        map.bufs[i].sva = map_handle.sVirtualAddr;
#endif
    }
    if (!ret) {
        if (!mapped) {
            // No buffers in set
            errno = EINVAL;
            return -1;
        }
        return 0;
    } else if (!mapped) {
        // No buffers to unmap
        return ret;
    }
    // Unwind
    unmap(map);
    return ret;
}

int DriverClient::unmap(const struct mc_ioctl_map& map) {
    int ret = 0;
    bool unmapped = false;
    for (int i = 0; i < MC_MAP_MAX; i++) {
        if (!map.bufs[i].va) {
            continue;
        }
        mcBulkMap_t map_handle;
        map_handle.sVirtualLen = map.bufs[i].len;
#if ( __WORDSIZE != 64 )
        map_handle.sVirtualAddr = reinterpret_cast<void*>(static_cast<uintptr_t>(map.bufs[i].sva));
#else
        map_handle.sVirtualAddr = static_cast<uint32_t>(map.bufs[i].sva);
#endif
        auto buf = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(map.bufs[i].va));
        mcResult_t mc_result = mcUnmap_(pimpl_->device, map.sid, buf, &map_handle);
        int local_ret = set_errno_from_mcresult(mc_result);
        if (local_ret) {
            ret = local_ret;
            _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
            // Keep going
            continue;
        }
        unmapped = true;
    }
    if (!ret && !unmapped) {
        errno = EINVAL;
        ret = -1;
    }
    return ret;
}

int DriverClient::getError(struct mc_ioctl_geterr& err) {
    mcResult_t mc_result = mcGetSessionErrorCode_(pimpl_->device, err.sid, &err.value);
    int ret = set_errno_from_mcresult(mc_result);
    if (ret) {
        _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
    }
    return ret;
}

int DriverClient::getVersion(struct mc_version_info& version_info) {
    mcVersionInfo_t versionInfo;
    int ret = mcGetMobiCoreVersion_(pimpl_->device, &versionInfo);
    if (ret) {
        _LOG_E("%s in %s", strerror(errno), __FUNCTION__);
    } else {
        ::memcpy(&version_info, &versionInfo, sizeof(version_info));
    }
    return ret;
}

// This class will need to handle the GP protocol at some point, but for now it
// only deals with cancellation
int DriverClient::gpRequestCancellation(uint32_t session_id) {
    bool found = false;
    pthread_mutex_lock(&pimpl_->gp_sessions_mutex);
    for (auto it = pimpl_->gp_sessions.begin(); it != pimpl_->gp_sessions.end();
         it++) {
        auto gp_session = *it;
        if (gp_session->id == session_id) {
            // Will be reset by caller at next InvokeCommand
            gp_session->tci->operation.isCancelled = true;
            found = true;
        }
    }
    pthread_mutex_unlock(&pimpl_->gp_sessions_mutex);
    if (!found) {
        errno = ENOENT;
        return -1;
    }
    return notify(session_id);
}
