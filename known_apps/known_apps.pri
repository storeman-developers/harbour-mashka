KNOWN_APPS = \
    $$PWD/depecher.ini \
    $$PWD/filecase.ini \
    $$PWD/jolla-email.ini \
    $$PWD/sailfish-browser.ini

OTHER_FILES += \
    $$PWD/generator.py \
    $$PWD/README.md

known_apps.input = KNOWN_APPS
known_apps.output = $$OUT_PWD/mknown_apps.hpp
known_apps.commands = python3 $$PWD/generator.py ./
known_apps.CONFIG += no_link target_predeps combine

QMAKE_EXTRA_COMPILERS += known_apps
