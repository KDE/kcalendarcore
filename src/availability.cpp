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
