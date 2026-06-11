# SPDX-FileCopyrightText: 2022 Volker Krause <vkrause@kde.org>
# SPDX-License-Identifier: BSD-3-Clause

gradle_add_aar(kcalendarcore_aar BUILDFILE ${CMAKE_CURRENT_SOURCE_DIR}/build.gradle NAME KF6CalendarCore)
gradle_install_aar(kcalendarcore_aar DESTINATION ${KDE_INSTALL_JARDIR})

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/KF6CalendarCore-android-dependencies.xml
    ${CMAKE_BINARY_DIR}/lib/KF6CalendarCore_${CMAKE_ANDROID_ARCH_ABI}-android-dependencies.xml
)
install(
    FILES
        KF6CalendarCore-android-dependencies.xml
    DESTINATION ${KDE_INSTALL_LIBDIR}
    RENAME KF6CalendarCore_${CMAKE_ANDROID_ARCH_ABI}-android-dependencies.xml
)
