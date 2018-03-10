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
}

bool Mashka::showBanner() const
{
    if (m_settings->value(QStringLiteral("BannerSeen"), false).toBool())
    {
        return false;
    }

    auto cleared  = this->totalClearedSize();
    auto launches = m_settings->value(QStringLiteral("Launches"), 0).toUInt();

    // 52428800 == 50 MB
    return cleared > 52428800 && launches >= 10;
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

qint64 Mashka::totalClearedSize() const
{
    return m_settings->value(QStringLiteral("TotalCleared"), 0).toLongLong();
}

void Mashka::addClearedSize(const qint64 &size)
{
    QString key(QStringLiteral("TotalCleared"));
    auto current = m_settings->value(key, 0).toLongLong();
    m_settings->setValue(key, current + size);
    emit this->totalClearedSizeChanged();
}
