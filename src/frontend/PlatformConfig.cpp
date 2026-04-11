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
#include <QFile>
#include <QDir>
#include <QStandardPaths>
#include <QSettings>

#include "PlatformConfig.h"
#include "Log.h"

#ifdef Q_OS_MACOS
#include "macOS.h"
#endif

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


QString findFcitx5WaylandLauncher() {
    QStringList candidates = {
        "/usr/share/applications/fcitx5-wayland-launcher.desktop",
        "/usr/local/share/applications/fcitx5-wayland-launcher.desktop",
    };

    // Also check XDG data dirs
    QString xdgDataDirs = QProcessEnvironment::systemEnvironment().value("XDG_DATA_DIRS", "/usr/share:/usr/local/share");
    for (const auto &dir : xdgDataDirs.split(':')) {
        QString path = dir + "/applications/fcitx5-wayland-launcher.desktop";
        if (!candidates.contains(path)) {
            candidates.append(path);
        }
    }

    for (const auto &path : candidates) {
        if (QFile::exists(path)) {
            return path;
        }
    }

    return "";
}

void setupKdeVirtualKeyboard() {
    QString launcherPath = findFcitx5WaylandLauncher();
    if (launcherPath.isEmpty()) {
        LOG_DEBUG("Fcitx5 Wayland launcher desktop file not found\n");
        return;
    }

    // Read current virtual keyboard setting
    QProcess readProcess;
    readProcess.start("kreadconfig6", {"--file", "kwinrc", "--group", "Wayland", "--key", "InputMethod"});
    readProcess.waitForFinished();

    QString currentValue = readProcess.readAllStandardOutput().trimmed();

    if (currentValue == launcherPath) {
        LOG_DEBUG("KDE virtual keyboard already set to Fcitx5\n");
        return;
    }

    // Set Fcitx5 as the virtual keyboard
    QProcess writeProcess;
    writeProcess.start("kwriteconfig6", {"--file", "kwinrc", "--group", "Wayland", "--key", "InputMethod", launcherPath});
    writeProcess.waitForFinished();

    if (writeProcess.exitCode() == 0) {
        LOG_DEBUG("Set KDE virtual keyboard to Fcitx5: %s\n", launcherPath.toStdString().c_str());
    } else {
        LOG_ERROR("Failed to set KDE virtual keyboard: %s\n", writeProcess.readAllStandardError().toStdString().c_str());
    }
}

bool addOpenBanglaToFcitx5ViaDBus() {
    // Get current input method group info
    QProcess getProcess;
    getProcess.start("gdbus", {"call", "--session",
        "--dest", "org.fcitx.Fcitx5",
        "--object-path", "/controller",
        "--method", "org.fcitx.Fcitx.Controller1.InputMethodGroupInfo",
        "Default"});
    getProcess.waitForFinished();

    if (getProcess.exitCode() != 0) {
        return false;
    }

    // Output format: ('us', [('keyboard-us', ''), ('openbangla', '')])
    QString output = getProcess.readAllStandardOutput().trimmed();

    if (output.contains("'openbangla'")) {
        LOG_DEBUG("OpenBangla already in Fcitx5 input method group\n");
        return true;
    }

    // Parse the default layout from the output
    // Format: ('layout', [...])
    QString defaultLayout = "us";
    int firstQuote = output.indexOf('\'');
    int secondQuote = output.indexOf('\'', firstQuote + 1);
    if (firstQuote >= 0 && secondQuote > firstQuote) {
        defaultLayout = output.mid(firstQuote + 1, secondQuote - firstQuote - 1);
    }

    // Parse the existing input methods array
    // We need to reconstruct the array with openbangla added
    int arrayStart = output.indexOf('[');
    int arrayEnd = output.lastIndexOf(']');
    QString existingArray;
    if (arrayStart >= 0 && arrayEnd > arrayStart) {
        existingArray = output.mid(arrayStart, arrayEnd - arrayStart + 1);
    }

    // Build new array: insert openbangla before the closing bracket
    QString newArray;
    if (existingArray.isEmpty() || existingArray == "[]") {
        newArray = "[('keyboard-us', ''), ('openbangla', '')]";
    } else {
        // Remove trailing ] and add openbangla
        newArray = existingArray.left(existingArray.length() - 1) + ", ('openbangla', '')]";
    }

    QProcess setProcess;
    setProcess.start("gdbus", {"call", "--session",
        "--dest", "org.fcitx.Fcitx5",
        "--object-path", "/controller",
        "--method", "org.fcitx.Fcitx.Controller1.SetInputMethodGroupInfo",
        "Default", defaultLayout, newArray});
    setProcess.waitForFinished();

    if (setProcess.exitCode() == 0) {
        LOG_DEBUG("Added OpenBangla to Fcitx5 input method group via DBus\n");
        return true;
    }

    LOG_ERROR("Failed to add OpenBangla via DBus: %s\n", setProcess.readAllStandardError().toStdString().c_str());
    return false;
}

void addOpenBanglaToFcitx5Profile() {
    QString profilePath = QDir::homePath() + "/.config/fcitx5/profile";

    // Ensure the directory exists
    QDir().mkpath(QDir::homePath() + "/.config/fcitx5");

    if (QFile::exists(profilePath)) {
        // Read existing profile and check if openbangla is already present
        QSettings profile(profilePath, QSettings::IniFormat);

        // Scan existing groups for openbangla
        int itemCount = 0;
        bool found = false;

        for (const auto &group : profile.childGroups()) {
            if (group.startsWith("Groups/0/Items/")) {
                itemCount++;
                profile.beginGroup(group);
                if (profile.value("Name").toString() == "openbangla") {
                    found = true;
                }
                profile.endGroup();
            }
        }

        if (found) {
            LOG_DEBUG("OpenBangla already in Fcitx5 profile\n");
            return;
        }

        // Add openbangla as the next item
        QString newGroup = QString("Groups/0/Items/%1").arg(itemCount);
        profile.beginGroup(newGroup);
        profile.setValue("Name", "openbangla");
        profile.setValue("Layout", "");
        profile.endGroup();
        profile.sync();

        LOG_DEBUG("Added OpenBangla to existing Fcitx5 profile\n");
    } else {
        // Create a new profile with keyboard-us and openbangla
        QSettings profile(profilePath, QSettings::IniFormat);

        profile.beginGroup("Groups/0");
        profile.setValue("Name", "Default");
        profile.setValue("Default Layout", "us");
        profile.setValue("DefaultIM", "openbangla");
        profile.endGroup();

        profile.beginGroup("Groups/0/Items/0");
        profile.setValue("Name", "keyboard-us");
        profile.setValue("Layout", "");
        profile.endGroup();

        profile.beginGroup("Groups/0/Items/1");
        profile.setValue("Name", "openbangla");
        profile.setValue("Layout", "");
        profile.endGroup();

        profile.beginGroup("GroupOrder");
        profile.setValue("0", "Default");
        profile.endGroup();

        profile.sync();

        LOG_DEBUG("Created new Fcitx5 profile with OpenBangla\n");
    }
}

void setupFcitx5InputMethod() {
    // Try DBus first (works when Fcitx5 is already running)
    if (!addOpenBanglaToFcitx5ViaDBus()) {
        LOG_DEBUG("Fcitx5 DBus not available, writing profile directly\n");
        addOpenBanglaToFcitx5Profile();
    }
}

void setupPantheonIME() {
    // Pantheon (elementary OS) uses iBus directly, not GNOME input sources.
    // Add OpenBangla to iBus preload engines so it appears in Pantheon's
    // Input Method settings panel.
    QString key = "/desktop/ibus/general/preload-engines";

    QProcess readProcess;
    readProcess.start("dconf", {"read", key});
    readProcess.waitForFinished();

    QString engines;
    if (readProcess.exitCode() == 0) {
        engines = readProcess.readAllStandardOutput().trimmed();
    }

    if (engines.contains("'OpenBangla'")) {
        LOG_DEBUG("OpenBangla already in iBus preload engines\n");
        return;
    }

    if (engines.isEmpty() || engines == "@as []") {
        engines = "['OpenBangla']";
    } else {
        // Insert before the closing bracket
        engines = engines.left(engines.length() - 1) + ", 'OpenBangla']";
    }

    writeDConfSetting(key, engines);
    LOG_DEBUG("Added OpenBangla to iBus preload engines for Pantheon\n");
}

void setupKdeIME() {
    setupKdeVirtualKeyboard();
    setupFcitx5InputMethod();
}

void setupInputSources() {
    auto de = detectDesktopEnvironment();

    if(de == DesktopEnvironment::GNOME) {
        setupGnomeIME();
    } else if(de == DesktopEnvironment::Pantheon) {
        setupPantheonIME();
    } else if(de == DesktopEnvironment::KDE) {
        setupKdeIME();
    } else if(de == DesktopEnvironment::macOS) {
        #ifdef Q_OS_MACOS
            bool enabled = macOS::getInputSourceEnabled();
            LOG_DEBUG("OpenBangla Input Source Enabled: %s\n", enabled ? "true" : "false");
        #endif
    } else {
        LOG_DEBUG("Desktop Environment not supported for input source setup\n");
    }
}
