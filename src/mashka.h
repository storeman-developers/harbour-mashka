#ifndef MASHKA_H
#define MASHKA_H

#include <QObject>

class QSettings;

class Mashka : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 totalDeletedData READ totalDeletedData NOTIFY totalDeletedDataChanged)
    Q_PROPERTY(bool advancedOptionsEnabled READ advancedOptionsEnabled WRITE setAdvancedOptionsEnabled NOTIFY advancedOptionsEnabledChanged)
    Q_PROPERTY(bool processConfigEnabled READ processConfigEnabled WRITE setProcessConfigEnabled NOTIFY processConfigEnabledChanged)
    Q_PROPERTY(bool deleteAllDataAllowed READ deleteAllDataAllowed WRITE setDeleteAllDataAllowed NOTIFY deleteAllDataAllowedChanged)

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

    qint64 totalDeletedData() const;
    Q_INVOKABLE void addDeletedData(const qint64 &size);
    Q_INVOKABLE void resetDeletedData();

    bool advancedOptionsEnabled() const;
    void setAdvancedOptionsEnabled(bool value);

    bool processConfigEnabled() const;
    void setProcessConfigEnabled(bool value);

    bool deleteAllDataAllowed() const;
    void setDeleteAllDataAllowed(bool value);

signals:
    void totalDeletedDataChanged();
    void advancedOptionsEnabledChanged();
    void processConfigEnabledChanged();
    void deleteAllDataAllowedChanged();

public slots:

private:
    QSettings *m_settings;
};

#endif // MASHKA_H
