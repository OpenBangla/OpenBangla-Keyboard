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

#include <cstdio>

#define LOG_INFO(...) std::fprintf(stdout, "[OpenBangla-Info]: " __VA_ARGS__)

#if DEBUG
#define LOG_DEBUG(...) std::fprintf(stdout, "[OpenBangla-Debug]: " __VA_ARGS__)
#else
#define LOG_DEBUG(...)
#endif

#define LOG_ERROR(...) std::fprintf(stderr, "[OpenBangla-Error]: " __VA_ARGS__)

#endif /* end of include guard: LOG_H */
