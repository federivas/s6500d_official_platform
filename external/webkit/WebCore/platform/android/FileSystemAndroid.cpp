/*
Copyright (C) 2011 for LGPL 2.1

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
*/

/*
 * Copyright 2009, The Android Open Source Project
 * Copyright (C) 2007 Holger Hans Peter Freyther
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 
 */

#include "config.h"
#include "FileSystem.h"

#include "CString.h"
#include "StringBuilder.h"
#include "cutils/log.h"
#include <dirent.h>
#include <dlfcn.h>
#include <errno.h>
#include <fnmatch.h>
#include <sys/stat.h>

namespace WebCore {

// Global static used to store the base to the plugin path.
// This is set in WebSettings.cpp
String sPluginPath;

CString fileSystemRepresentation(const String& path)
{
    return path.utf8();
}

CString openTemporaryFile(const char* prefix, PlatformFileHandle& handle)
{
    int number = rand() % 10000 + 1;
    CString filename;
    do {
        StringBuilder builder;
        builder.append(sPluginPath);
        builder.append('/');
        builder.append(prefix);
        builder.append(String::number(number));
        filename = builder.toString().utf8();
        const char* fstr = filename.data();
        handle = open(filename.data(), O_WRONLY | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR);
        number++;
        
        if (handle != -1)
            return filename;
    } while (errno == EEXIST);
    
    return CString();
}

bool unloadModule(PlatformModule module)
{
    return !dlclose(module);
}

void closeFile(PlatformFileHandle& handle)
{
    if (isHandleValid(handle)) {
        close(handle);
        handle = invalidPlatformFileHandle;
    }
}

int writeToFile(PlatformFileHandle handle, const char* data, int length)
{
    int totalBytesWritten = 0;
    while (totalBytesWritten < length) {
        int bytesWritten = write(handle, data, (size_t)(length - totalBytesWritten));
        if (bytesWritten < 0 && errno != EINTR)
            return -1;
        if (bytesWritten > 0)
            totalBytesWritten += bytesWritten;
    }

    return totalBytesWritten;
}

String homeDirectoryPath() 
{
    return sPluginPath;
}

Vector<String> listDirectory(const String& path, const String& filter)
{
    Vector<String> entries;
    CString cpath = path.utf8();
    CString cfilter = filter.utf8();
    DIR* dir = opendir(cpath.data());
    if (dir) {
        struct dirent* dp;
        while (dp = readdir(dir)) {
            const char* name = dp->d_name;
            if (!strcmp(name, ".") || !strcmp(name, ".."))
                continue;
            if (fnmatch(cfilter.data(), name, 0))
                continue;
            char filePath[1024];
            if ((int) (sizeof(filePath) - 1) < snprintf(filePath,
                    sizeof(filePath), "%s/%s", cpath.data(), name)) {
                continue; // buffer overflow
            }
            entries.append(filePath);
        }
        closedir(dir);
    }
    return entries;
}
/*SISO_HTMLCOMPOSER begin*/
CString openLocalFile(const String& basePath, const String& extension , PlatformFileHandle& handle)
{
    int number = rand() % 10000 + 1;
    CString filename;
    do {
        StringBuilder builder;
        builder.append(basePath);
		if(!fileExists(String("/sdcard/temp"))){
		        bool ret = makeAllDirectories(String("/sdcard/temp/"));
		}
        filename = builder.toString().utf8();
        const char* fstr = filename.data();
		handle = open(filename.data(), O_WRONLY | O_CREAT /*| O_EXCL*/, S_IRUSR | S_IWUSR);
        number++;
        if (handle != -1)
            return filename;
    } while (errno == EEXIST);

    return CString();
}
/*SISO_HTMLCOMPOSER END*/
} // namespace WebCore
