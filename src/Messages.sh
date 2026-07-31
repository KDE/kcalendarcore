#!/bin/sh
# SPDX-FileCopyrightText: 2026 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

$EXTRACT_TR_STRINGS `find . -name \*.cpp -o -name \*.h` -o $podir/kcalendarcore6_qt.pot
