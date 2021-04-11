#include "mashka.h"

#include <QSettings>

static const QString LAUNCHES_KEY{QStringLiteral("Launches")};
static const QString BANNERSEEN_KEY{QStringLiteral("BannerSeen")};
static const QString TOTALDELETED_KEY{QStringLiteral("TotalDeleted")};
static const QString ADVANCEDOPTIONS_KEY{QStringLiteral("AdvancedOptionsEnabled")};
static const QString PROCESSCONFIG_KEY{QStringLiteral("ProcessConfigEnabled")};
static const QString DELETEALLDATA_KEY{QStringLiteral("DeleteAllDataAllowed")};
static const QString HINTS_GROUP{QStringLiteral("Hints")};

static inline auto hint_key(Mashka::Hint hint) {
    return QVariant::fromValue<Mashka::Hint>(hint).toString();
}

Mashka::Mashka(QObject *parent)
    : QObject{parent}
    , m_settings{new QSettings{this}}
{
    const auto current = m_settings->value(LAUNCHES_KEY, 0).toUInt();
    if (current < 10)
    {
        m_settings->setValue(LAUNCHES_KEY, current + 1);
    }
}

bool Mashka::showBanner() const
{
    if (m_settings->value(BANNERSEEN_KEY, false).toBool())
    {
        return false;
    }

    const auto deleted  = this->totalDeletedData();
    const auto launches = m_settings->value(LAUNCHES_KEY, 0).toUInt();

    // 52428800 == 50 MB
    return deleted > 52428800 && launches >= 10;
}

void Mashka::setBannerShowed()
{
    m_settings->setValue(BANNERSEEN_KEY, true);
}

bool Mashka::showHint(Hint hint) const
{
    m_settings->beginGroup(HINTS_GROUP);
    const auto res = !m_settings->value(hint_key(hint), false).toBool();
    m_settings->endGroup();
    return res;
}

void Mashka::setHintShowed(Hint hint)
{
    m_settings->beginGroup(HINTS_GROUP);
    m_settings->setValue(hint_key(hint), true);
    m_settings->endGroup();
}

qint64 Mashka::totalDeletedData() const
{
    return m_settings->value(TOTALDELETED_KEY, 0).toLongLong();
}

void Mashka::addDeletedData(qint64 size)
{
    const auto current = m_settings->value(TOTALDELETED_KEY, 0).toLongLong();
    m_settings->setValue(TOTALDELETED_KEY, current + size);
    emit this->totalDeletedDataChanged();
}

void Mashka::resetDeletedData()
{
    qDebug("Resetting TotalDeleted value");
    m_settings->remove(TOTALDELETED_KEY);
    emit this->totalDeletedDataChanged();
}

bool Mashka::advancedOptionsEnabled() const
{
    return m_settings->value(ADVANCEDOPTIONS_KEY).toBool();
}

void Mashka::setAdvancedOptionsEnabled(bool value)
{
    if (m_settings->value(ADVANCEDOPTIONS_KEY).toBool() != value)
    {
        m_settings->setValue(ADVANCEDOPTIONS_KEY, value);
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
    return m_settings->value(PROCESSCONFIG_KEY).toBool();
}

void Mashka::setProcessConfigEnabled(bool value)
{
    if (m_settings->value(PROCESSCONFIG_KEY).toBool() != value)
    {
        m_settings->setValue(PROCESSCONFIG_KEY, value);
        emit this->processConfigEnabledChanged();
    }
}

bool Mashka::deleteAllDataAllowed() const
{
    return m_settings->value(DELETEALLDATA_KEY).toBool();
}

void Mashka::setDeleteAllDataAllowed(bool value)
{
    if (m_settings->value(DELETEALLDATA_KEY).toBool() != value)
    {
        m_settings->setValue(DELETEALLDATA_KEY, value);
        emit this->deleteAllDataAllowedChanged();
    }
}
