/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2015-2016 Muhammad Mominul Huque <mominul2082@gmail.com>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/* Logging functions */

#ifndef LOG_H
#define LOG_H

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
extern "C" {
    void NSLog(CFStringRef format, ...);
}

// Helper macro to create format string
#define NS_FORMAT(fmt) CFSTR("[OpenBangla-Info]: " fmt)
#define LOG_INFO(fmt, ...) NSLog(NS_FORMAT(fmt), ##__VA_ARGS__)

#if DEBUG
#define NS_DEBUG_FORMAT(fmt) CFSTR("[OpenBangla-Debug]: " fmt)
#define LOG_DEBUG(fmt, ...) NSLog(NS_DEBUG_FORMAT(fmt), ##__VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#define NS_ERROR_FORMAT(fmt) CFSTR("[OpenBangla-Error]: " fmt)
#define LOG_ERROR(fmt, ...) NSLog(NS_ERROR_FORMAT(fmt), ##__VA_ARGS__)

#else
#include <cstdio>

#define LOG_INFO(...) std::fprintf(stdout, "[OpenBangla-Info]: " __VA_ARGS__)

#if DEBUG
#define LOG_DEBUG(...) std::fprintf(stdout, "[OpenBangla-Debug]: " __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#define LOG_ERROR(...) std::fprintf(stderr, "[OpenBangla-Error]: " __VA_ARGS__)

#endif /* __APPLE__ */

#endif /* end of include guard: LOG_H */