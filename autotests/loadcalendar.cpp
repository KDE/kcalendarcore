/*
  This file is part of the kcalcore library.

  SPDX-FileCopyrightText: 2003 Cornelius Schumacher <schumacher@kde.org>

  SPDX-License-Identifier: LGPL-2.0-or-later
*/

#include "filestorage.h"
#include "memorycalendar.h"

#include <QCommandLineParser>
#include <QCoreApplication>
#include <QDebug>
#include <QTimeZone>

using namespace KCalendarCore;

int main(int argc, char **argv)
{
    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringList() << QStringLiteral("verbose"), QStringLiteral("Verbose output")));

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("testincidence"));
    QCoreApplication::setApplicationVersion(QStringLiteral("0.1"));
    parser.process(app);

    MemoryCalendar::Ptr cal(new MemoryCalendar(QTimeZone::utc()));
    FileStorage store(cal, QStringLiteral("cal"));
    if (!store.load()) {
        qWarning() << "Error storing into memory calendar";
        return 1;
    }

    Todo::List todoList;

    // Build dictionary to look up Task object from Todo uid.  Each task is a
    // QListViewItem, and is initially added with the view as the parent.
    todoList = cal->rawTodos();

    if (todoList.isEmpty()) {
        qWarning() << "Error loading calendar";
        return 1;
    }

    qDebug() << (*todoList.begin())->uid();
    QString result = (*todoList.begin())->customProperty(QByteArray("karm"), QByteArray("totalTaskTime"));
    qDebug() << result;
    if (result != QLatin1String("a,b")) {
        qDebug() << "The string a,b was expected, but given was" << result;
        return 1;
    } else {
        qDebug() << "Test passed";
    }
}
