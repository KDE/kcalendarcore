/*
  This file is part of the kcalcore library.

  Copyright (c) 2019 Volker Krause <vkrause@kde.org>

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

#ifndef KCALCORE_RECURRENCEHELPER_H
#define KCALCORE_RECURRENCEHELPER_H

#include <algorithm>

namespace KCalCore {

template <typename T>
inline void sortAndRemoveDuplicates(T &container)
{
    std::sort(container.begin(), container.end());
    container.erase(std::unique(container.begin(), container.end()), container.end());
}

template <typename T>
inline void inplaceSetDifference(T &set1, const T &set2)
{
    auto beginIt = set1.begin();
    for (const auto &elem : set2) {
        const auto it = std::lower_bound(beginIt, set1.end(), elem);
        if (it != set1.end() && *it == elem) {
            beginIt = set1.erase(it);
        }
    }
}

template <typename Container, typename Value>
inline void setInsert(Container &c, const Value &v)
{
    const auto it = std::lower_bound(c.begin(), c.end(), v);
    if (it == c.end() || *it != v) {
        c.insert(it, v);
    }
}

}

#endif
