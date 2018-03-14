#include "mashka.h"

#include <QSettings>
#include <QMetaEnum>


Mashka::Mashka(QObject *parent)
    : QObject(parent)
    , m_settings(new QSettings(this))
{
    QString key(QStringLiteral("Launches"));
    auto current = m_settings->value(key, 0).toUInt();
    if (current < 10)
    {
        m_settings->setValue(key, current + 1);
    }
    QString oldkey(QStringLiteral("TotalCleared"));
    if (m_settings->contains(oldkey))
    {
        m_settings->setValue(QStringLiteral("TotalDeleted"), m_settings->value(oldkey));
        m_settings->remove(oldkey);
    }
}

bool Mashka::showBanner() const
{
    if (m_settings->value(QStringLiteral("BannerSeen"), false).toBool())
    {
        return false;
    }

    auto deleted  = this->totalDeletedData();
    auto launches = m_settings->value(QStringLiteral("Launches"), 0).toUInt();

    // 52428800 == 50 MB
    return deleted > 52428800 && launches >= 10;
}

void Mashka::setBannerShowed()
{
    m_settings->setValue(QStringLiteral("BannerSeen"), true);
}

bool Mashka::showHint(const Hint &hint) const
{
    auto me = QMetaEnum::fromType<Hint>();
    auto name = me.valueToKey(hint);
    Q_ASSERT(name);
    return !m_settings->value(QStringLiteral("Hints/").append(name), false).toBool();
}

void Mashka::setHintShowed(const Hint &hint)
{
    auto me = QMetaEnum::fromType<Hint>();
    auto name = me.valueToKey(hint);
    Q_ASSERT(name);
    m_settings->setValue(QStringLiteral("Hints/").append(name), true);
}

qint64 Mashka::totalDeletedData() const
{
    return m_settings->value(QStringLiteral("TotalDeleted"), 0).toLongLong();
}

void Mashka::addDeletedData(const qint64 &size)
{
    QString key(QStringLiteral("TotalDeleted"));
    auto current = m_settings->value(key, 0).toLongLong();
    m_settings->setValue(key, current + size);
    emit this->totalDeletedDataChanged();
}

void Mashka::resetDeletedData()
{
    qDebug("Resetting TotalDeleted value");
    m_settings->remove(QStringLiteral("TotalDeleted"));
    emit this->totalDeletedDataChanged();
}

bool Mashka::advancedOptionsEnabled() const
{
    return m_settings->value(QStringLiteral("AdvancedOptionsEnabled")).toBool();
}

void Mashka::setAdvancedOptionsEnabled(bool value)
{
    QString key(QStringLiteral("AdvancedOptionsEnabled"));
    if (m_settings->value(key).toBool() != value)
    {
        m_settings->setValue(key, value);
        emit this->advancedOptionsEnabledChanged();
        if (!value)
        {
            this->setProcessConfigEnabled(false);
            this->setDeleteAllDataAllowed(false);
        }
    }
}

bool Mashka::processConfigEnabled() const
{
    return m_settings->value(QStringLiteral("ProcessConfigEnabled")).toBool();
}

void Mashka::setProcessConfigEnabled(bool value)
{
    QString key(QStringLiteral("ProcessConfigEnabled"));
    if (m_settings->value(key).toBool() != value)
    {
        m_settings->setValue(key, value);
        emit this->processConfigEnabledChanged();
    }
}

bool Mashka::deleteAllDataAllowed() const
{
    return m_settings->value(QStringLiteral("DeleteAllDataAllowed")).toBool();
}

void Mashka::setDeleteAllDataAllowed(bool value)
{
    QString key(QStringLiteral("DeleteAllDataAllowed"));
    if (m_settings->value(key).toBool() != value)
    {
        m_settings->setValue(key, value);
        emit this->deleteAllDataAllowedChanged();
    }
}
