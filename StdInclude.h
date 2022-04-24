#pragma once

#ifdef _MSC_VER
// This is just a dummy to get intellisense for JukeBox stuff
#    include "../../API/JukeboxTypes.h"
#    include "../../API/Jukebox.h"
#    ifndef JUKEBOX_VISUALDEBUG
// We still want to be able to compile VisualDebug configuration
static_assert(false, "This shouldnt be compiled with Visual Studio");
#    endif // JUKEBOX_VISUALDEBUG

#else
#    include "Jukebox.h"
#endif

#ifndef DEBUG
#    ifdef NDEBUG
#        define DEBUG 0
#    else
#        define DEBUG 1
#    endif
#endif

#if DEBUG
#    define CHECKINVARIANT() CheckInvariant(__FUNCTION__)
#    define CHECKINVARIANT_OTHER() iOther.CheckInvariant("iOther")
#    define IF_DEBUG(x) x
#    define ASSERT JBOX_ASSERT
#    define ASSERT_INVARIANT(x) JBOX_ASSERT_MESSAGE(x, iFunctionName)
#    define ASSERT_MSG JBOX_ASSERT_MESSAGE
#else
#    define CHECKINVARIANT()
#    define CHECKINVARIANT_OTHER()
#    define IF_DEBUG(x)
#    define ASSERT(x)
#    define ASSERT_INVARIANT(x)
#    define ASSERT_MSG(x, y)
#endif

#include <cstring>
#include <cstdint>
#include <cmath>
