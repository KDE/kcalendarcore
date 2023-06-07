#include "availability.h"
#include "incidence_p.h"

#include "kcalendarcore_debug.h"

using namespace KCalendarCore;

Availability::Availability()
    : Incidence()
{
}

Incidence::IncidenceType Availability::type() const
{
    return TypeAvailability;
}

QByteArray Availability::typeStr() const
{
    return QByteArrayLiteral("Availability");
}

Availability::~Availability() = default;

Availability *Availability::clone() const
{
    // TODO fix this return new Availability(*this);
    return NULL;
}

bool Availability::hasStartDate() const
{
    return IncidenceBase::dtStart().isValid();
}

QDateTime Availability::dtStart() const
{
    if (!hasStartDate()) {
        return QDateTime();
    }

    return IncidenceBase::dtStart();
}

QDateTime Availability::dateTime(DateTimeRole role) const
{
    // TODO see what to add here
    return QDateTime();
}

void Availability::setDateTime(const QDateTime &dateTime, DateTimeRole role)
{
    // TODO see what to add here
}

QLatin1String Availability::mimeType() const
{
    return Availability::availabilityMimeType();
}

QLatin1String Availability::availabilityMimeType()
{
    return QLatin1String("application/x-vnd.akonadi.calendar.availability");
}

void Availability::virtual_hook(VirtualHook id, void *data)
{
    Q_UNUSED(id);
    Q_UNUSED(data);
}

QLatin1String Availability::iconName(const QDateTime &recurrenceId) const
{
    // TODO see what to add here
    return QLatin1String("availability ?");
}

// TODO what does this mean?
bool Availability::supportsGroupwareCommunication() const
{
    return true;
}
