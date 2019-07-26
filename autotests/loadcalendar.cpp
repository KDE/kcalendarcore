/*
  This file is part of the kcalcore library.

  Copyright (c) 2003 Cornelius Schumacher <schumacher@kde.org>

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Library General Public
  License as published by the Free Software Foundation; either
  version 2 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Library General Public License for more details.

  You should have received a copy of the GNU Library General Public License
  along with this library; see the file COPYING.LIB.  If not, write to
  the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
  Boston, MA 02110-1301, USA.
*/

#include "filestorage.h"
#include "memorycalendar.h"

#include <QDebug>
#include <QCoreApplication>
#include <QCommandLineParser>
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
    QString result = (*todoList.begin())->customProperty(QByteArray("karm"),
                                                         QByteArray("totalTaskTime"));
    qDebug() << result;
    if (result != QStringLiteral("a,b")) {
        qDebug() << "The string a,b was expected, but given was" << result;
        return 1;
    } else {
        qDebug() << "Test passed";
    }
}
