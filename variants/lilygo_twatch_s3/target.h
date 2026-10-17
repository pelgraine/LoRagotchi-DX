#pragma once

#include "variant.h"

#define RADIOLIB_STATIC_ONLY 1
#include <RadioLib.h>
#include <helpers/radiolib/RadioLibWrappers.h>
#if defined(TWATCH_SX1280)
  #include <helpers/radiolib/CustomSX1280Wrapper.h>   // 2.4 GHz watch revision
#else
  #include <helpers/radiolib/CustomSX1262Wrapper.h>
#endif
#include <TWatchS3Board.h>
#include <helpers/AutoDiscoverRTCClock.h>
#include <helpers/SensorManager.h>

#ifdef DISPLAY_CLASS
  #include <TWatchS3Display.h>
  #include <TWatchInput.h>
#endif

extern TWatchS3Board board;
extern WRAPPER_CLASS radio_driver;
extern AutoDiscoverRTCClock rtc_clock;
extern SensorManager sensors;

#ifdef DISPLAY_CLASS
  extern DISPLAY_CLASS display;
  // Side PWR key + touch screen, presented to the UI as its single button.
  extern TWatchInput user_btn;
#endif

bool radio_init();
mesh::LocalIdentity radio_new_identity();
