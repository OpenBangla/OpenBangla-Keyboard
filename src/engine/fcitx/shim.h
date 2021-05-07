/** Shim for supporting Fcitx 5.0.5 version **/
/*
 * SPDX-FileCopyrightText: 2021~2021 CSSlayer <wengxt@gmail.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 */

#include <string>
#include <fcitx-utils/standardpath.h>
#include <fcitx-utils/stringutils.h>

using namespace fcitx;

#if !(FCITX_PATCH > 5)
#include <sys/stat.h>
#include <cstdint>
#include <utility>

struct Timespec {
    int64_t sec;
    int64_t nsec;
};

template <typename T>
inline std::enable_if_t<(&T::st_mtim, true), Timespec>
modifiedTime(const T &p) {
    return {p.st_mtim.tv_sec, p.st_mtim.tv_nsec};
}

// This check is necessary because on FreeBSD st_mtimespec is defined as
// st_mtim. This would cause a redefinition.
#if !defined(st_mtimespec)
template <typename T>
inline std::enable_if_t<(&T::st_mtimespec, true), Timespec>
modifiedTime(const T &p) {
    return {p.st_mtimespec.tv_sec, p.st_mtimespec.tv_nsec};
}
#endif

#if !defined(st_mtimensec) && !defined(__alpha__)
template <typename T>
inline std::enable_if_t<(&T::st_mtimensec, true), Timespec>
modifiedTime(const T &p) {
    return {p.st_mtime, p.st_mtimensec};
}
#endif

bool isAbsolutePath(const std::string &path) {
    return !path.empty() && path[0] == '/';
}

bool checkBoolEnvVar(const char *name) {
    const char *var = getenv(name);
    bool value = false;
    if (var && var[0] &&
        (strcmp(var, "True") == 0 || strcmp(var, "true") == 0 ||
         strcmp(var, "1") == 0)) {
        value = true;
    }
    return value;
}

std::string constructPath(const std::string &basepath,
                          const std::string &path) {
    if (basepath.empty()) {
        return {};
    }
    return fs::cleanPath(stringutils::joinPath(basepath, path));
}

int64_t getTimestamp(const std::string &path) {
    struct stat stats;
    if (stat(path.c_str(), &stats) != 0) {
        return 0;
    }
    return modifiedTime(stats).sec;
};

int64_t timestamp(StandardPath::Type type, const std::string &path) {
    bool skipFcitx = checkBoolEnvVar("SKIP_FCITX_PATH");
    bool skipUser = checkBoolEnvVar("SKIP_FCITX_USER_PATH");
    StandardPath std(skipFcitx, skipUser);

    if (isAbsolutePath(path)) {
        return getTimestamp(path);
    }

    int64_t timestamp = 0;
    std.scanDirectories(type,
                    [&timestamp, &path](const std::string &dirPath, bool) {
                        auto fullPath = constructPath(dirPath, path);
                        timestamp = std::max(timestamp, getTimestamp(fullPath));
                        return true;
                    });
    return timestamp;
}

#else

int64_t timestamp(StandardPath::Type type, const std::string &path) {
    return StandardPath::global().timestamp(type, path);
}

#endif /* !(FCITX_PATCH > 5) */
