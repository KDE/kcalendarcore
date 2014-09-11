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

#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kcomponentdata.h>
#include <qdebug.h>

#include <QtCore/QCoreApplication>
#include <QtCore/QCommandLineParser>

using namespace KCalCore;

int main(int argc, char **argv)
{
    QCommandLineParser parser;
    parser.addOption(QCommandLineOption(QStringList() << "verbose" , i18n("Verbose output")));

    KAboutData about(QStringLiteral("testcalendar"),
                     i18n("Test Calendar"), QStringLiteral("0.1"));

    about.setupCommandLine(&parser);
    KAboutData::setApplicationData(about);

    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("testincidence"));
    QCoreApplication::setApplicationVersion("0.1");
    parser.process(app);
    about.processCommandLine(&parser);

    MemoryCalendar::Ptr cal(new MemoryCalendar(KDateTime::UTC));
    FileStorage store(cal, "cal");
    store.load();

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
    if (result != QString("a,b")) {
        qDebug() << "The string a,b was expected, but given was" << result;
        return 1;
    } else {
        qDebug() << "Test passed";
    }
}
