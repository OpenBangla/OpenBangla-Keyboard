#
# OpenBangla-Keyboard Frontend GUI: additional Qt project file (for Qt Designer).
#
# NOTE: This file is intended to be opened by Qt Designer
#       as a project file (to work with .ui files)
#

TEMPLATE	= app
LANGUAGE	= C++

SOURCES += \
    TopBar.cpp \
    AboutFile.cpp \
    LayoutViewer.cpp \
    SettingsDialog.cpp \
    AboutDialog.cpp \
    AutoCorrectDialog.cpp

HEADERS  += \
    TopBar.h \
    AboutFile.h \
    LayoutViewer.h \
    SettingsDialog.h \
    AboutDialog.h \
    AutoCorrectDialog.h

FORMS    += \
    TopBar.ui \
    AboutFile.ui \
    LayoutViewer.ui \
    SettingsDialog.ui \
    AboutDialog.ui \
    AutoCorrectDialog.ui

RESOURCES += \
    images.qrc
