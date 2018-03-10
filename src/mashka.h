#ifndef MASHKA_H
#define MASHKA_H

#include <QObject>

class QSettings;

class Mashka : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 totalClearedSize READ totalClearedSize NOTIFY totalClearedSizeChanged)

public:
    enum Hint
    {
        OpenEntriesPage
    };
    Q_ENUM(Hint)

    explicit Mashka(QObject *parent = nullptr);

    Q_INVOKABLE bool showBanner() const;
    Q_INVOKABLE void setBannerShowed();

    Q_INVOKABLE bool showHint(const Hint &hint) const;
    Q_INVOKABLE void setHintShowed(const Hint &hint);

    qint64 totalClearedSize() const;
    Q_INVOKABLE void addClearedSize(const qint64 &size);

signals:
    void totalClearedSizeChanged();

public slots:

private:
    QSettings *m_settings;
};

#endif // MASHKA_H
