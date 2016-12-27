/*
 * Copyright (C) 2010 The Android Open Source Project
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

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/mount.h>

#include <linux/kdev_t.h>
#include <sys/wait.h>
#include <linux/fs.h>
#include <sys/ioctl.h>

#define LOG_TAG "Vold"
#include <logwrap/logwrap.h>
#include <cutils/log.h>
#include <cutils/properties.h>
#include "VoldUtil.h"
#include "Exfat.h"

static char EXFAT_PATH[] = "/system/bin/exfat";
static char MKFS_EXFAT_PATH[] = "/system/bin/mkexfat";

//extern "C" int logwrap(int argc, const char **argv, int background);
extern "C" int mount(const char *, const char *, const char *, unsigned long, const void *);

int Exfat::Mount(const std::string& source, const std::string& target, bool ro,
        bool remount, bool executable, int ownerUid, int ownerGid, int permMask,
        bool createLost) {
    int rc;
    unsigned long flags;
    char mountData[255];

    const char* c_source = source.c_str();
    const char* c_target = target.c_str();

    flags = MS_NODEV | MS_NOSUID | MS_DIRSYNC;

    flags |= (executable ? 0 : MS_NOEXEC);
    flags |= (ro ? MS_RDONLY : 0);
    flags |= (remount ? MS_REMOUNT : 0);

    /*
     * Note: This is a temporary hack. If the sampling profiler is enabled,
     * we make the SD card world-writable so any process can write snapshots.
     *
     * TODO: Remove this code once we have a drop box in system_server.
     */
    char value[PROPERTY_VALUE_MAX];
    property_get("persist.sampling_profiler", value, "");
    if (value[0] == '1') {
        SLOGW("The SD card is world-writable because the"
            " 'persist.sampling_profiler' system property is set to '1'.");
        permMask = 0;
    }

    sprintf(mountData,
            "utf8,uid=%d,gid=%d,fmask=%o,dmask=%o",
            ownerUid, ownerGid, permMask, permMask);

    rc = mount(c_source, c_target, "exfat", flags, mountData);

    if (rc && errno == EROFS) {
        SLOGE("%s appears to be a read only filesystem - retrying mount RO", c_source);
        flags |= MS_RDONLY;
        rc = mount(c_source, c_target, "exfat", flags, mountData);
    }

    if (rc == 0 && createLost) {
        char *lost_path;
        asprintf(&lost_path, "%s/LOST.DIR", c_target);
        if (access(lost_path, F_OK)) {
            /*
             * Create a LOST.DIR in the root so we have somewhere to put
             * lost cluster chains (fsck_msdos doesn't currently do this)
             */
            if (mkdir(lost_path, 0755)) {
                SLOGE("Unable to create LOST.DIR (%s)", strerror(errno));
            }
        }
        free(lost_path);
    }

    return rc;
}

int Exfat::unMount(const char *mountPoint) {
	int rc = 0;
    do {
		int status;
        const char *args[3];
        args[0] = "umount";
        args[1] = mountPoint;
        args[2] = NULL;

        rc = android_fork_execvp(ARRAY_SIZE(args), (char **)args, &status,
                false, true);
		if(!rc){
			SLOGI("unMount Exfat device %s OK", mountPoint);
            return 0;
		}else{
			SLOGE("unMount Exfat device %s failed", mountPoint);
			return -1;
		}
    } while (0);
    return rc;
}

int Exfat::format(const char *fsPath, unsigned int numSectors) {
    const char *args[3];
    int rc;
	int status;
	
    args[0] = MKFS_EXFAT_PATH;
    args[1] = fsPath;
    args[2] = NULL;
	
	rc = android_fork_execvp(ARRAY_SIZE(args), (char **)args, &status, false,
            true);
	if(!rc){
		SLOGI("Format Exfat device %s OK", fsPath);
        return 0;
	}else{
		SLOGE("Format Exfat device %s failed", fsPath);
		return -1;
	}
}