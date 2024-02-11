KNOWN_APPS = \
    $$PWD/depecher.ini \
    $$PWD/filecase.ini \
    $$PWD/jolla-email.ini \
    $$PWD/sailfish-browser.ini \
    $$PWD/harbour-telegrame.ini \
    $$PWD/microtube.ini \
    $$PWD/openrepos-books.ini \
    $$PWD/harbour-storeman.ini \
    $$PWD/harbour-meecast.ini \
    $$PWD/harbour-amazfish.ini

OTHER_FILES += \
    $$PWD/generator.py \
    $$PWD/README.md

known_apps.input = KNOWN_APPS
known_apps.output = $$OUT_PWD/mknown_apps.hpp
known_apps.commands = python3 $$PWD/generator.py ./
known_apps.CONFIG += no_link target_predeps combine

QMAKE_EXTRA_COMPILERS += known_apps
