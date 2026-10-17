#pragma once

#include <Arduino.h>
#include <helpers/ui/MomentaryButton.h>   // BUTTON_EVENT_* constants
#include "TWatchS3Board.h"
#include "TWatchS3Display.h"

// TWatchInput -- stands in for the single user button that LoRaGotchi's UI
// expects, so the existing click / double-click / long-press controls work
// unchanged on the watch:
//
//   side PWR key   press < 1s           -> click (also wakes the display)
//   touch screen   tap                  -> click
//                  two quick taps       -> double-click
//                  three quick taps     -> triple-click
//                  hold for 1s          -> long-press
//
// The PWR key is not a GPIO: it is wired to the AXP2101 PWRON pin, so presses
// are read from the PMU's latched IRQ status over I2C (polling approach ported
// from Meck's PMUButton). The PMU cannot report long or multi presses (holding
// the key 6s is a hardware power-off), so those gestures come from the touch
// screen only.
//
// Touches are ignored while the display is off, so a sleeve or wrist brushing
// the glass cannot wake the watch; press the side key to wake it.
//
// Touch gestures follow MomentaryButton's timing: a tap is reported once the
// multi-click window has passed, and a long-press fires while still held.
class TWatchInput {
  TWatchS3Board& _board;
  TWatchS3Display& _display;

  // PWR key
  static const unsigned long PWR_POLL_MS = 30;
  unsigned long _last_pwr_poll = 0;
  bool _pwr_down = false;

  // Touch
  static const unsigned long TOUCH_POLL_MS = 20;
  static const unsigned long TOUCH_RELEASE_MS = 60;   // finger must be gone this long
  static const unsigned long LONG_PRESS_MS = 1000;     // matches user_btn on other boards
  static const unsigned long MULTI_CLICK_MS = 280;     // matches MomentaryButton
  unsigned long _last_touch_poll = 0;
  unsigned long _last_touch_seen = 0;
  bool _touch_down = false;
  unsigned long _down_at = 0;
  uint8_t _click_count = 0;
  unsigned long _last_click = 0;
  bool _pending_click = false;

  void resetTouch() {
    _touch_down = false;
    _down_at = 0;
    _click_count = 0;
    _last_click = 0;
    _pending_click = false;
  }

  int checkPwrKey() {
    if (millis() - _last_pwr_poll < PWR_POLL_MS) return BUTTON_EVENT_NONE;
    _last_pwr_poll = millis();

    XPowersAXP2101* pmu = _board.getPMU();
    if (pmu == NULL) return BUTTON_EVENT_NONE;

    pmu->getIrqStatus();   // latch the status registers into the driver
    // Apply the edges before the click, so a poll that catches press, release
    // and short-press together still leaves _pwr_down false.
    if (pmu->isPekeyNegativeIrq()) _pwr_down = true;   // PWRON is active low: press
    if (pmu->isPekeyPositiveIrq()) _pwr_down = false;  // release
    int ev = pmu->isPekeyShortPressIrq() ? BUTTON_EVENT_CLICK : BUTTON_EVENT_NONE;
    pmu->clearIrqStatus();
    return ev;
  }

  int checkTouch() {
    if (!_display.isOn()) {
      resetTouch();
      return BUTTON_EVENT_NONE;
    }

    unsigned long now = millis();
    int event = BUTTON_EVENT_NONE;

    if (now - _last_touch_poll >= TOUCH_POLL_MS) {
      _last_touch_poll = now;
      bool touched = _display.isTouched();
      if (touched) {
        _last_touch_seen = now;
        if (!_touch_down) {          // finger down
          _touch_down = true;
          _down_at = now;
        }
      } else if (_touch_down && now - _last_touch_seen >= TOUCH_RELEASE_MS) {
        _touch_down = false;         // finger up
        if (_down_at > 0 && now - _down_at < LONG_PRESS_MS) {
          _click_count++;
          _last_click = now;
          _pending_click = true;
        }
        _down_at = 0;
      }
    }

    if (_down_at > 0 && now - _down_at >= LONG_PRESS_MS) {
      if (_pending_click) {
        // held during multi-click detection: drop the whole gesture
        _click_count = 0;
        _last_click = 0;
        _pending_click = false;
      } else {
        event = BUTTON_EVENT_LONG_PRESS;
      }
      _down_at = 0;   // the release that follows is not a click
    }

    if (_pending_click && !_touch_down && now - _last_click >= MULTI_CLICK_MS) {
      if (_click_count == 1)      event = BUTTON_EVENT_CLICK;
      else if (_click_count == 2) event = BUTTON_EVENT_DOUBLE_CLICK;
      else                        event = BUTTON_EVENT_TRIPLE_CLICK;
      _click_count = 0;
      _last_click = 0;
      _pending_click = false;
    }
    return event;
  }

public:
  TWatchInput(TWatchS3Board& board, TWatchS3Display& display) : _board(board), _display(display) {}

  void begin() {}

  // True while the PWR key is held or a finger is on the screen.
  bool isPressed() const { return _pwr_down || _touch_down; }

  int check() {
    int ev = checkPwrKey();
    if (ev != BUTTON_EVENT_NONE) {
      resetTouch();
      return ev;
    }
    return checkTouch();
  }
};
