/*
 *  OpenBangla Keyboard
 *  Copyright (C) 2025 Muhammad Mominul Huque <mominul2082@gmail.com>
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

 enum class DesktopEnvironment {
    Unknown,
    Windows,
    macOS,
    GNOME,
    KDE,
    XFCE,
    LXDE,
    LXQt,
    MATE,
    Cinnamon,
    Budgie,
    Unity,
    Pantheon,
    Deepin,
    Wayland,
    X11
};

DesktopEnvironment detectDesktopEnvironment();
QString desktopEnvironmentToString(DesktopEnvironment de);
bool isAppIndicatorEnabled();
bool shouldShowTrayIcon();
