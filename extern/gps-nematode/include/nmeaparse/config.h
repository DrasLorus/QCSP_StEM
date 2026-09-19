#pragma once

#ifndef NMEA_NOEXCEPT
#if defined (__GNUC__) && !defined (__clang__)
#define NMEA_NOEXCEPT _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW
#elif defined (__clang__)
#define NMEA_NOEXCEPT _NOEXCEPT
#endif
#endif // !NMEA_NOEXCEPT
