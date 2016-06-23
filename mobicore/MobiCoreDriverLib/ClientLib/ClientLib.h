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
/**
 * MobiCore Driver API.
 *
 * The MobiCore (MC) Driver API provides access functions to the t-base trusted execution environment and the contained Trusted Applications.
 */
#ifndef CLIENTLIB_H_
#define CLIENTLIB_H_

#define __MC_CLIENT_LIB_API extern "C"

#ifndef WIN32
/* Mark only the following functions for export */
#pragma GCC visibility push(default)
#endif

#include "Device.h"

/** Open a new connection to a t-base device.
 *
 * mcOpenDevice() initializes all device specific resources required to communicate
 * with an t-base instance located on the specified device in the system. If the device
 * does not exist the function will return MC_DRV_ERR_UNKNOWN_DEVICE.
 *
 * @param [out] device Device struct for the connection/driver.
 *
 * @return MC_DRV_OK if operation has been successfully completed.
 * @return MC_DRV_ERR_INVALID_OPERATION if device already opened.
 * @return MC_DRV_ERR_DAEMON_UNREACHABLE when problems with daemon occur.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when deviceId is unknown.
 * @return MC_DRV_ERR_INVALID_DEVICE_FILE if kernel module under /dev/mobicore cannot be opened
 *
 * Uses a Mutex.
 */
__MC_CLIENT_LIB_API mcResult_t mcOpenDevice_(
    Device**            device
);

/** Close the connection to a t-base device.
 * When closing a device, active sessions have to be closed beforehand.
 * Resources associated with the device will be released.
 * The device may be opened again after it has been closed.
 *
 * @param [in] device Device struct for the connection/driver.
 *
 * @return MC_DRV_OK if operation has been successfully completed.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when device id is invalid.
 * @return MC_DRV_ERR_SESSION_PENDING when a session is still open.
 * @return MC_DRV_ERR_DAEMON_UNREACHABLE when problems with daemon occur.
 *
 * Uses a Mutex.
 */
__MC_CLIENT_LIB_API mcResult_t mcCloseDevice_(
    Device*             device
);

__MC_CLIENT_LIB_API bool mcDeviceIsValid_(
    Device*             device
);

__MC_CLIENT_LIB_API mcResult_t mcDeviceHasOpenSessions_(
    Device*             device
);

/** Open a new session to a Trusted Application. The Trusted Application with the given UUID has to be available in the flash filesystem.
 *
 * Write MCP open message to buffer and notify t-base about the availability of a new command.
 * Waits till t-base responds with the new session ID (stored in the MCP buffer).
 *
 * @param [in,out] session On success, the session data will be returned. Note that session.deviceId has to be the device id of an opened device.
 * @param [in] uuid UUID of the Trusted Application to be opened.
 * @param [in] tci TCI buffer for communicating with the Trusted Application.
 * @param [in] tciLen Length of the TCI buffer. Maximum allowed value is MC_MAX_TCI_LEN.
 *
 * @return MC_DRV_OK if operation has been successfully completed.
 * @return MC_DRV_INVALID_PARAMETER if session parameter is invalid.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when device id is invalid.
 * @return MC_DRV_ERR_DAEMON_UNREACHABLE when problems with daemon socket occur.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when daemon returns an error.
 * @return MC_DRV_ERR_TRUSTED_APPLICATION_NOT_FOUND when Trusted Application or driver cannot be loaded.
 *
 * Uses a Mutex.
 */
__MC_CLIENT_LIB_API mcResult_t mcOpenSession_(
    Device*             device,
    uint32_t*           session_id,
    const mcUuid_t*     uuid,
    uint8_t*            tci,
    uint32_t            tciLen
);

/** Open a new session to a Trusted Application(Trustlet). The Trusted Application will be loaded from the memory buffer.
 *
 * Write MCP open message to buffer and notify t-base about the availability of a new command.
 * Waits till t-base responds with the new session ID (stored in the MCP buffer).
 *
 * @param [in,out] session On success, the session data will be returned. Note that session.deviceId has to be the device id of an opened device.
 * @param [in] spid Service Provider ID(for Service provider trustlets otherwise ignored)
 * @param [in] trustedapp memory buffer containing the Trusted Application binary
 * @param [in] tlen length of the memory buffer containing the Trusted Application
 * @param [in] tci TCI buffer for communicating with the Trusted Application.
 * @param [in] tciLen Length of the TCI buffer. Maximum allowed value is MC_MAX_TCI_LEN.
 *
 * @return MC_DRV_OK if operation has been successfully completed.
 * @return MC_DRV_INVALID_PARAMETER if session parameter is invalid.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when device id is invalid.
 * @return MC_DRV_ERR_DAEMON_UNREACHABLE when problems with daemon socket occur.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when daemon returns an error.
 * @return MC_DRV_ERR_TRUSTED_APPLICATION_NOT_FOUND when Trusted Application cannot be loaded.
 *
 * Uses a Mutex.
 */
__MC_CLIENT_LIB_API mcResult_t mcOpenTrustlet_(
    Device*             device,
    uint32_t*           session_id,
    mcSpid_t            spid,
    uint8_t*            trustedapp,
    uint32_t            tLen,
    uint8_t*            tci,
    uint32_t            tciLen
);

__MC_CLIENT_LIB_API mcResult_t mcOpenGPTA_(
    Device*             device,
    uint32_t*           session_id,
    const mcUuid_t*     uuid,
    uint8_t*            tci,
    uint32_t            len
);

/** Close a Trusted Application session.
 *
 * Closes the specified t-base session. The call will block until the session has been closed.
 *
 * @pre Device deviceId has to be opened in advance.
 *
 * @param [in] session Session to be closed.
 *
 * @return MC_DRV_OK if operation has been successfully completed.
 * @return MC_DRV_INVALID_PARAMETER if session parameter is invalid.
 * @return MC_DRV_ERR_UNKNOWN_SESSION when session id is invalid.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when device id of session is invalid.
 * @return MC_DRV_ERR_DAEMON_UNREACHABLE when problems with daemon occur.
 * @return MC_DRV_ERR_INVALID_DEVICE_FILE when daemon cannot open trustlet file.
 *
 * Uses a Mutex.
 */
__MC_CLIENT_LIB_API mcResult_t mcCloseSession_(
    Device*             device,
    uint32_t            session_id
);

/** Notify a session.
 * Notifies the session end point about available message data.
 * If the session parameter is correct, notify will always succeed.
 * Corresponding errors can only be received by mcWaitNotification().
 * @pre A session has to be opened in advance.
 *
 * @param session The session to be notified.
 *
 * @return MC_DRV_OK if operation has been successfully completed.
 * @return MC_DRV_INVALID_PARAMETER if session parameter is invalid.
 * @return MC_DRV_ERR_UNKNOWN_SESSION when session id is invalid.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when device id of session is invalid.
 */
__MC_CLIENT_LIB_API mcResult_t mcNotify_(
    Device*             device,
    uint32_t            session_id
);

/** Wait for a notification.
 *
 * Wait for a notification issued by t-base for a specific session.
 * The timeout parameter specifies the number of milliseconds the call will wait for a notification.
 * If the caller passes 0 as timeout value the call will immediately return. If timeout value is below 0 the call will block
 * until a notification for the session has been received.
 *
 * @attention if timeout is below 0, call will block:
 * Caller has to trust the other side to send a notification to wake him up again.
 *
 * @param [in] session The session the notification should correspond to.
 * @param [in] timeout Time in milliseconds to wait (MC_NO_TIMEOUT : direct return, > 0 : milliseconds, MC_INFINITE_TIMEOUT : wait indefinitely, MC_INFINITE_TIMEOUT_INTERRUPTIBLE : wait indefinitely except if signal received)
 *
 * @return MC_DRV_OK if notification is available.
 * @return MC_DRV_ERR_TIMEOUT if no notification arrived in time.
 * @return MC_DRV_INFO_NOTIFICATION if a problem with the session was encountered. Get more details with mcGetSessionErrorCode().
 * @return MC_DRV_ERR_NOTIFICATION if a problem with the socket occurred.
 * @return MC_DRV_INVALID_PARAMETER if a parameter is invalid.
 * @return MC_DRV_ERR_UNKNOWN_SESSION when session id is invalid.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when device id of session is invalid.
 */
__MC_CLIENT_LIB_API mcResult_t mcWaitNotification_(
    Device*             device,
    uint32_t            session_id,
    int32_t             timeout
);

/**
 * Allocate a block of world shared memory (WSM).
 * The MC driver allocates a contiguous block of memory which can be used as WSM.
 * This implicates that the allocated memory is aligned according to the alignment parameter.
 * Always returns a buffer of size WSM_SIZE aligned to 4K.
 *
 * @param [in]  deviceId The ID of an opened device to retrieve the WSM from.
 * @param [in]  align The alignment (number of pages) of the memory block (e.g. 0x00000001 for 4kB).
 * @param [in]  len Length of the block in bytes.
 * @param [out] wsm Virtual address of the world shared memory block.
 * @param [in]  wsmFlags Platform specific flags describing the memory to be allocated.
 *
 * @attention: align and wsmFlags are currently ignored
 *
 * @return MC_DRV_OK if operation has been successfully completed.
 * @return MC_DRV_INVALID_PARAMETER if a parameter is invalid.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when device id is invalid.
 * @return MC_DRV_ERR_NO_FREE_MEMORY if no more contiguous memory is available in this size or for this process.
 *
 * Uses a Mutex.
 */
__MC_CLIENT_LIB_API mcResult_t mcMallocWsm_(
    Device*             device,
    uint32_t            len,
    uint8_t**           wsm
);

/**
 * Free a block of world shared memory (WSM).
 * The MC driver will free a block of world shared memory (WSM) previously allocated with
 * mcMallocWsm(). The caller has to assure that the address handed over to the driver
 * is a valid WSM address.
 *
 * @param [in] deviceId The ID to which the given address belongs.
 * @param [in] wsm Address of WSM block to be freed.
 *
 * @return MC_DRV_OK if operation has been successfully completed.
 * @return MC_DRV_INVALID_PARAMETER if a parameter is invalid.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when device id is invalid.
 * @return MC_DRV_ERR_FREE_MEMORY_FAILED on failures.
 *
 * Uses a Mutex.
 */
__MC_CLIENT_LIB_API mcResult_t mcFreeWsm_(
    Device*             device,
    uint8_t*            wsm
);

/**
 * Map additional bulk buffer between a Client Application (CA) and the Trusted Application (TA) for a session.
 * Memory allocated in user space of the CA can be mapped as additional communication channel
 * (besides TCI) to the Trusted Application. Limitation of the Trusted Application memory structure apply: only 6 chunks can be mapped
 * with a maximum chunk size of 1 MiB each.
 *
 * @attention It is up to the application layer (CA) to inform the Trusted Application about the additional mapped bulk memory.
 *
 * @param [in] session Session handle with information of the deviceId and the sessionId. The
 * given buffer is mapped to the session specified in the sessionHandle.
 * @param [in] buf Virtual address of a memory portion (relative to CA) to be shared with the Trusted Application, already includes a possible offset!
 * @param [in] len length of buffer block in bytes.
 * @param [out] mapInfo Information structure about the mapped Bulk buffer between the CA (NWd) and
 * the TA (SWd).
 *
 * @return MC_DRV_OK if operation has been successfully completed.
 * @return MC_DRV_INVALID_PARAMETER if a parameter is invalid.
 * @return MC_DRV_ERR_UNKNOWN_SESSION when session id is invalid.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when device id of session is invalid.
 * @return MC_DRV_ERR_DAEMON_UNREACHABLE when problems with daemon occur.
 * @return MC_DRV_ERR_BULK_MAPPING when buf is already uses as bulk buffer or when registering the buffer failed.
 *
 * Uses a Mutex.
 */
__MC_CLIENT_LIB_API mcResult_t mcMap_(
    Device*             device,
    uint32_t            session_id,
    void*               buf,
    uint32_t            len,
    mcBulkMap_t*        mapInfo
);

/**
 * Remove additional mapped bulk buffer between Client Application (CA) and the Trusted Application (TA) for a session.
 *
 * @attention The bulk buffer will immediately be unmapped from the session context.
 * @attention The application layer (CA) must inform the TA about unmapping of the additional bulk memory before calling mcUnmap!
 *
 * @param [in] session Session handle with information of the deviceId and the sessionId. The
 * given buffer is unmapped from the session specified in the sessionHandle.
 * @param [in] buf Virtual address of a memory portion (relative to CA) shared with the TA, already includes a possible offset!
 * @param [in] mapInfo Information structure about the mapped Bulk buffer between the CA (NWd) and
 * the TA (SWd).
 * @attention The clientlib currently ignores the len field in mapInfo.
 *
 * @return MC_DRV_OK if operation has been successfully completed.
 * @return MC_DRV_INVALID_PARAMETER if a parameter is invalid.
 * @return MC_DRV_ERR_UNKNOWN_SESSION when session id is invalid.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when device id of session is invalid.
 * @return MC_DRV_ERR_DAEMON_UNREACHABLE when problems with daemon occur.
 * @return MC_DRV_ERR_BULK_UNMAPPING when buf was not registered earlier or when unregistering failed.
 *
 * Uses a Mutex.
 */
__MC_CLIENT_LIB_API mcResult_t mcUnmap_(
    Device*             device,
    uint32_t            session_id,
    void*               buf,
    mcBulkMap_t*        mapInfo
);

/**
 * Get additional error information of the last error that occurred on a session.
 * After the request the stored error code will be deleted.
 *
 * @param [in] session Session handle with information of the deviceId and the sessionId.
 * @param [out] lastErr >0 Trusted Application has terminated itself with this value, <0 Trusted Application is dead because of an error within t-base (e.g. Kernel exception).
 * See also notificationPayload_t enum in MCI definition at "mcinq.h".
 *
 * @return MC_DRV_OK if operation has been successfully completed.
 * @return MC_DRV_INVALID_PARAMETER if a parameter is invalid.
 * @return MC_DRV_ERR_UNKNOWN_SESSION when session id is invalid.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when device id of session is invalid.
 */
__MC_CLIENT_LIB_API mcResult_t mcGetSessionErrorCode_(
    Device*             device,
    uint32_t            session_id,
    int32_t*            lastErr
);

/**
 * Get t-base version information of a device.
 *
 * @param [in] deviceId of an open device.
 * @param [out] versionInfo t-base version info.
 *
 * @return MC_DRV_OK if operation has been successfully completed.
 * @return MC_DRV_ERR_UNKNOWN_DEVICE when device is not open.
 * @return MC_DRV_INVALID_PARAMETER if a parameter is invalid.
 * @return MC_DRV_ERR_DAEMON_UNREACHABLE when problems with daemon occur.
 */
__MC_CLIENT_LIB_API mcResult_t mcGetMobiCoreVersion_(
    Device*             device,
    mcVersionInfo_t*    versionInfo
);
#ifndef WIN32
#pragma GCC visibility pop
#endif
#endif /** CLIENTLIB_H_ */
