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

#include <QCoreApplication>
#include <QProcess>
#include <QDebug>
#include <QStringList>

#include "PlatformConfig.h"

DesktopEnvironment detectDesktopEnvironment() {
    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();

    // Detect Windows/macOS first
    #ifdef Q_OS_WIN
    return DesktopEnvironment::Windows;
    #endif

    #ifdef Q_OS_MACOS
    return DesktopEnvironment::macOS;
    #endif

    // Linux/UNIX detection
    QStringList desktopEnvs {
        env.value("XDG_CURRENT_DESKTOP"),
        env.value("DESKTOP_SESSION"),
        env.value("GDMSESSION"),
        env.value("XDG_SESSION_DESKTOP")
    };

    const QString de = desktopEnvs.join(';').toLower();

    if (de.contains("gnome")) return DesktopEnvironment::GNOME;
    if (de.contains("kde")) return DesktopEnvironment::KDE;
    if (de.contains("xfce")) return DesktopEnvironment::XFCE;
    if (de.contains("lxde")) return DesktopEnvironment::LXDE;
    if (de.contains("lxqt")) return DesktopEnvironment::LXQt;
    if (de.contains("mate")) return DesktopEnvironment::MATE;
    if (de.contains("cinnamon")) return DesktopEnvironment::Cinnamon;
    if (de.contains("budgie")) return DesktopEnvironment::Budgie;
    if (de.contains("unity")) return DesktopEnvironment::Unity;
    if (de.contains("pantheon")) return DesktopEnvironment::Pantheon;
    if (de.contains("deepin")) return DesktopEnvironment::Deepin;

    // Fallback session type detection
    if (!env.value("WAYLAND_DISPLAY").isEmpty())
        return DesktopEnvironment::Wayland;
    if (env.value("XDG_SESSION_TYPE").contains("x11"))
        return DesktopEnvironment::X11;

    return DesktopEnvironment::Unknown;
}

QString desktopEnvironmentToString(DesktopEnvironment de) {
    switch(de) {
    case DesktopEnvironment::Windows: return "Windows";
    case DesktopEnvironment::macOS: return "macOS";
    case DesktopEnvironment::GNOME: return "GNOME";
    case DesktopEnvironment::KDE: return "KDE";
    case DesktopEnvironment::XFCE: return "XFCE";
    case DesktopEnvironment::LXDE: return "LXDE";
    case DesktopEnvironment::LXQt: return "LXQt";
    case DesktopEnvironment::MATE: return "MATE";
    case DesktopEnvironment::Cinnamon: return "Cinnamon";
    case DesktopEnvironment::Budgie: return "Budgie";
    case DesktopEnvironment::Unity: return "Unity";
    case DesktopEnvironment::Pantheon: return "Pantheon";
    case DesktopEnvironment::Deepin: return "Deepin";
    case DesktopEnvironment::Wayland: return "Wayland";
    case DesktopEnvironment::X11: return "X11";
    default: return "Unknown";
    }
}

bool isAppIndicatorEnabled() {
    QProcess process;
    process.setProgram("gnome-extensions");
    process.setArguments(QStringList() << "list" << "--enabled");
    
    process.start();
    if (!process.waitForFinished()) {
        qWarning() << "Failed to execute gnome-extensions command";
        return false;
    }
    
    if (process.exitStatus() != QProcess::NormalExit || process.exitCode() != 0) {
        qWarning() << "Command execution failed";
        return false;
    }
    
    const QString output = QString::fromUtf8(process.readAllStandardOutput());
    const QString targetUUID = "appindicatorsupport@rgcjonas.gmail.com";
    
    // Check each line for exact match
    const QStringList extensions = output.split('\n', Qt::SkipEmptyParts);
    for (const QString &ext : extensions) {
        if (ext.trimmed() == targetUUID) {
            return true;
        }
    }
    
    return false;
}

bool shouldShowTrayIcon() {
    DesktopEnvironment de = detectDesktopEnvironment();
    if (de == DesktopEnvironment::GNOME) {
        return isAppIndicatorEnabled();
    } else if(de == DesktopEnvironment::Pantheon) {
        // Pantheon (Elementary OS) does not support AppIndicator
        return false;
    }

    return true;
}
