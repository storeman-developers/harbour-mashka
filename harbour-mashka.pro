TARGET = harbour-mashka

CONFIG += sailfishapp

QT += concurrent

SOURCES += \
    src/harbour-mashka.cpp \
    src/mashka.cpp \
    src/mmodel.cpp \
    src/mproxymodel.cpp

HEADERS += \
    src/mashka.h \
    src/mmodel.h \
    src/mproxymodel.h \
    src/mentry.h

# Write version file
VERSION_H = \
"$${LITERAL_HASH}ifndef MASHKA_VERSION" \
"$${LITERAL_HASH}   define MASHKA_VERSION \"$$VERSION\"" \
"$${LITERAL_HASH}endif"
write_file($$$$OUT_PWD/mashka_version.h, VERSION_H)

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG_OUTPUT

DISTFILES += \
    qml/harbour-mashka.qml \
    qml/cover/CoverPage.qml \
    qml/pages/MainPage.qml \
    qml/pages/EntriesPage.qml \
    qml/pages/BannerPage.qml \
    qml/pages/ConfirmationDialog.qml \
    qml/pages/AboutPage.qml \
    qml/pages/SettingsPage.qml \
    qml/components/RescanMenuEntry.qml \
    qml/components/DataLabel.qml \
    qml/components/PlainButton.qml \
    qml/components/SizeLabel.qml \
    qml/components/EntryDelegate.qml \
    qml/components/Hint.qml \
    qml/components/HintLabel.qml \
    qml/components/ListMenuItem.qml \
    icons/harbour-mashka.svg \
    rpm/harbour-mashka.changes \
    rpm/harbour-mashka.spec \
    rpm/harbour-mashka.yaml \
    translations/*.ts \
    .tx/config \
    harbour-mashka.desktop \
    .gitignore \
    LICENSE \
    README.md

SAILFISHAPP_ICONS = 86x86 108x108 128x128

CONFIG += \
    sailfishapp_i18n \
    sailfishapp_i18n_idbased \
    sailfishapp_i18n_unfinished

TRANSLATIONS += \
    translations/harbour-mashka.ts \
    translations/harbour-mashka-es.ts \
    translations/harbour-mashka-hu.ts \
    translations/harbour-mashka-ru.ts \
    translations/harbour-mashka-sv.ts
