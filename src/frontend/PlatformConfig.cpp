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
#include <QString>

#include "PlatformConfig.h"
#include "Log.h"

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
    const QString targetUUID2 = "ubuntu-appindicators@ubuntu.com";
    
    // Check each line for exact match
    const QStringList extensions = output.split('\n', Qt::SkipEmptyParts);
    for (const QString &ext : extensions) {
        auto trimmedExt = ext.trimmed();
        if (trimmedExt == targetUUID || trimmedExt == targetUUID2) {
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

QString getGnomeInputSources() {
    QString key = "/org/gnome/desktop/input-sources/sources";

    QProcess process;
    process.start("dconf", {"read", key});
    process.waitForFinished();

    if (process.exitCode() == 0) {
        QString output = process.readAllStandardOutput().trimmed();
        return output;
    } else {
        LOG_ERROR("getGnomeInputSources Error: %s\n", process.readAllStandardError().toStdString().c_str());
        return "";
    }
}

void writeDConfSetting(QString key, QString value) {
    // Execute: dconf write <key> "<value>"
    QProcess process;
    process.start("dconf", {"write", key, value});
    process.waitForFinished();

    if (process.exitCode() == 0) {
        LOG_DEBUG("Successfully updated DConf settings!\n");
    } else {
        LOG_ERROR("Error updating DConf settings: %s\n", process.readAllStandardError().toStdString().c_str());
    }
}

void setupGnomeIME() {
    QString sources = getGnomeInputSources();
    QStringList sourcesList;

    // It comes in an array of tuples like this:
    // [('xkb', 'us'), ('ibus', 'OpenBangla')]
    // So we need to do some preprocessing.
    auto string = sources.replace("[", "").replace("]", "");
    auto index = string.indexOf("),");

    while(index != -1) {
        auto sub = string.mid(0, index + 1).trimmed();
        sourcesList.append(sub);
        string.remove(0, index + 2);
        index = string.indexOf("),");
    }

    if(!string.isEmpty()) {
        sourcesList.append(string.trimmed());
    }

    for (const auto &source : sourcesList) {
        LOG_DEBUG("Source: %s\n", source.toStdString().c_str());
    }

    QString firstSource = sourcesList.first();

    // Check if OpenBangla exists in the sources
    bool found = sourcesList.contains("('ibus', 'OpenBangla')");

    if (found) {
        LOG_DEBUG("OpenBangla found in sources\n");
    } else {
        LOG_DEBUG("OpenBangla not found in sources\n");
        // Add OpenBangla to the sources
        sourcesList.append("('ibus', 'OpenBangla')");
        auto sourcesString = QString("[%1]").arg(sourcesList.join(", "));
        writeDConfSetting("/org/gnome/desktop/input-sources/sources", sourcesString);
        writeDConfSetting("/org/gnome/desktop/input-sources/mru-sources", QString("[%1]").arg(firstSource));
        LOG_DEBUG("Added OpenBangla to sources\n");
    }
}


void setupInputSources() {
    auto de = detectDesktopEnvironment();

    if(de == DesktopEnvironment::GNOME) {
        setupGnomeIME();
    } else {
        LOG_DEBUG("Desktop Environment not supported for input source setup\n");
    }
}
