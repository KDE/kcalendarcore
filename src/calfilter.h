/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2001, 2003, 2004 Cornelius Schumacher <schumacher@kde.org>
  SPDX-FileCopyrightText: 2003-2004 Reinhold Kainhofer <reinhold@kainhofer.com>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/
/*
  @file
  This file is part of the API for handling calendar data and
  defines the CalFilter class.
*/

#ifndef KCALCORE_CALFILTER_H
#define KCALCORE_CALFILTER_H

#include "event.h"
#include "journal.h"
#include "kcalendarcore_export.h"
#include "todo.h"

namespace KCalendarCore
{
/*!
  \class KCalendarCore::CalFilter
  \inmodule KCalendarCore
  \inheaderfile KCalendarCore/CalFilter
  \brief
  Provides a filter for calendars.

  This class provides a means for filtering calendar incidences by
  a list of email addresses, a list of categories, or other Criteria.

  The following Criteria are available:
  \list
    \li remove recurring Incidences
    \li keep Incidences with a matching category (see setCategoryList())
    \li remove completed To-dos (see setCompletedTimeSpan())
    \li remove inactive To-dos
    \li remove To-dos without a matching attendee (see setEmailList())
  \endlist
*/
class KCALENDARCORE_EXPORT CalFilter
{
public:
    /*!
      \enum KCalendarCore::CalFilter::Criteria
      \brief Filtering Criteria.

      \value HideRecurring
      Remove incidences that recur.
      \value HideCompletedTodos
      Remove completed to-dos.
      \value ShowCategories
      Show incidences with at least one matching category.
      \value HideInactiveTodos
      Remove to-dos that haven't started yet.
      \value HideNoMatchingAttendeeTodos
      Remove to-dos without a matching attendee.
    */
    enum Criteria {
        HideRecurring = 1,
        HideCompletedTodos = 2,
        ShowCategories = 4,
        HideInactiveTodos = 8,
        HideNoMatchingAttendeeTodos = 16,
    };

    /*!
      Constructs an empty filter -- a filter without a name or criteria.
    */
    CalFilter();

    /*!
      Constructs a filter with \a name.

      \a name is the name of this filter.

    */
    explicit CalFilter(const QString &name);

    /*!
      Destroys this filter.
    */
    ~CalFilter();

    /*!
      Sets the filter name.

      \a name is the name of this filter.

      \sa name().
    */
    void setName(const QString &name);

    /*!
      Returns the filter name.
      \sa setName().
    */
    Q_REQUIRED_RESULT QString name() const;

    /*!
      Sets the criteria which must be fulfilled for an Incidence to pass
      the filter.

      \a criteria is a combination of CalFilter::Criteria.

      \sa criteria().
    */
    void setCriteria(int criteria);

    /*!
      Returns the inclusive filter criteria.
      \sa setCriteria().
    */
    Q_REQUIRED_RESULT int criteria() const;

    /*!
      Applies the filter to a list of Events. All events not matching the
      filter criteria are removed from the list.

      \a eventList is a list of Events to filter.

    */
    void apply(Event::List *eventList) const;

    /*!
      Applies the filter to a list of To-dos. All to-dos not matching the
      filter criteria are removed from the list.

      \a todoList is a list of To-dos to filter.

    */
    void apply(Todo::List *todoList) const;

    /*!
      Applies the filter to a list of Journals. All journals not matching the
      filter criteria are removed from the list.

      \a journalList is a list of Journals to filter.

    */
    void apply(Journal::List *journalList) const;

    /*!
      Applies the filter criteria to the specified Incidence.

      \a incidence is the Incidence to filter.

      Returns true if the Incidence passes the criteria; false otherwise.
    */
    Q_REQUIRED_RESULT bool filterIncidence(const Incidence::Ptr &incidence) const;

    /*!
      Enables or disables the filter.

      \a enabled is true if the filter is to be enabled; false otherwise.

      \sa isEnabled().
    */
    void setEnabled(bool enabled);

    /*!
      Returns whether the filter is enabled or not.
      \sa setEnabled().
    */
    Q_REQUIRED_RESULT bool isEnabled() const;

    /*!
      Sets the list of categories to be considered when filtering incidences
      according to the ShowCategories criteria.

      \a categoryList is a QStringList of categories.

      \sa categoryList().
    */
    void setCategoryList(const QStringList &categoryList);

    /*!
      Returns the category list for this filter.
      \sa setCategoryList().
    */
    Q_REQUIRED_RESULT QStringList categoryList() const;

    /*!
      Sets the list of email addresses to be considered when filtering
      incidences according to the HideNoMatchingAttendeeTodos criteria.

      \a emailList is a QStringList of email addresses.

      \sa emailList().
    */
    void setEmailList(const QStringList &emailList);

    /*!
      Returns the email list for this filter.
      \sa setEmailList().
    */
    Q_REQUIRED_RESULT QStringList emailList() const;

    /*!
      Sets the number of days for the HideCompletedTodos criteria.
      If a to-do has been completed within the recent \a timespan days,
      then that to-do will be removed during filtering. If a time span is
      not specified in the filter, then all completed to-dos will be removed
      if the HideCompletedTodos criteria is set.

      \a timespan is an integer representing a time span in days.

      \sa completedTimeSpan().
     */
    void setCompletedTimeSpan(int timespan);

    /*!
      Returns the completed time span for this filter.
      \sa setCompletedTimeSpan()
     */
    Q_REQUIRED_RESULT int completedTimeSpan() const;

    /*!
      Compares this with \a filter for equality.

      \a filter the CalFilter to compare.

      Returns true if \a filter is equal to this object, or false if they are different.
    */
    bool operator==(const CalFilter &filter) const;

private:
    //@cond PRIVATE
    Q_DISABLE_COPY(CalFilter)
    class Private;
    Private *const d;
    //@endcond
};

}

#endif
