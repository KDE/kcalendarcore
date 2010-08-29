/*
  This file is part of the kcalcore library.

  Copyright (C) 2004 Dirk Mueller <mueller@kde.org>

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
#ifndef KCALCORE_EXPORT_H
#define KCALCORE_EXPORT_H

#include <kdemacros.h>

#ifndef KCALCORE_EXPORT
# if defined(KDEPIM_STATIC_LIBS)
   /* No export/import for static libraries */
#  define KCALCORE_EXPORT
# elif defined(MAKE_KCALCORE_LIB)
   /* We are building this library */
#  define KCALCORE_EXPORT KDE_EXPORT
# else
   /* We are using this library */
#  define KCALCORE_EXPORT KDE_IMPORT
# endif
#endif

# ifndef KCALCORE_EXPORT_DEPRECATED
#  if !defined( WANT_DEPRECATED_KCALCORE_API )
#    define KCALCORE_EXPORT_DEPRECATED KDE_DEPRECATED KCALCORE_EXPORT
#  else
#    define KCALCORE_EXPORT_DEPRECATED KCALCORE_EXPORT
#  endif
# endif

#ifdef COMPILING_TESTS
#ifndef KCALCORE_TEST_EXPORT
# if defined(KDEPIM_STATIC_LIBS)
   /* No export/import for static libraries */
#  define KCALCORE_TEST_EXPORT
# elif defined(MAKE_KCALCORE_TEST_LIB)
   /* We are building this library */
#  define KCALCORE_TEST_EXPORT KDE_EXPORT
# else
   /* We are using this library */
#  define KCALCORE_TEST_EXPORT KDE_IMPORT
# endif
#endif
#endif /* COMPILING_TESTS */

/**
  @namespace KCalCore

  @brief
  Contains all the KCalCore library global classes, objects, and functions.
*/

#endif
