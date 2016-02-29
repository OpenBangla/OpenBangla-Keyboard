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
    LayoutViewer.cpp

HEADERS  += \
    TopBar.h \
    AboutFile.h \
    LayoutViewer.h

FORMS    += \
    TopBar.ui \
    AboutFile.ui \
    LayoutViewer.ui

RESOURCES += \
    images.qrc
