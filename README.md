# Mashka

Search for and delete unused data leaved after removed Sailfish OS applications.

## Translations

You can help to localize harbour-mashka to your language using [Transifex](https://www.transifex.com/mentaljam/harbour-mashka) or [Qt Linguist](http://doc.qt.io/qt-5/qtlinguist-index.html).

Note that harbour-mashka translations are **ID based** Qt ts files. So if you want to compile a translation file for testing you should run the lrelease command with the "-idbased" option, for example:

    lrelease -idbased harbour-mashka.ts

If you want to test your translation before publishing you should compile it and place produced qm file to (root access is required)

    /usr/share/harbour-mashka/translations

The application tries to load translation files automatically basing on your system locale settings. Also you can run application with selected locale from terminal. For example for Russian language the command is

    LANG=ru harbour-mashka

## Acknowledgements

To [gregguh](https://github.com/gregguh) for the launcher icon.

To the community for the translations and feedback.
