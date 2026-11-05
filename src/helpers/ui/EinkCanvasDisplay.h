#pragma once

// LoRaGotchi on a small e-ink panel. The pet UI is laid out for a 128x64 OLED, so this
// driver gives it exactly that: a 128x64 one-bit off-screen picture, using the same built-in
// font as the OLED. endFrame() then scales the whole picture by 1.9 (nearest neighbour) to
// 244x122, centred on the 250x122 panel, and sends it as a partial refresh.

#include <SPI.h>
#include <Adafruit_GFX.h>

#define ENABLE_GxEPD2_GFX 0

#include <GxEPD2_BW.h>

#include "DisplayDriver.h"

#ifndef DISPLAY_ROTATION
  #define DISPLAY_ROTATION 1
#endif

#ifndef EINK_FULL_REFRESH_MS
  #define EINK_FULL_REFRESH_MS 300000UL   // full refresh every 5 minutes while the picture changes
#endif

class EinkCanvasDisplay : public DisplayDriver {
  static constexpr int CANVAS_W = 128;
  static constexpr int CANVAS_H = 64;
  static constexpr int SCALED_W = 244;
  static constexpr int SCALED_H = 122;
  static constexpr int OFFSET_X = 3;    // (250 - 244) / 2

  GxEPD2_BW<EINK_DISPLAY_MODEL, EINK_DISPLAY_MODEL::HEIGHT> display;
  GFXcanvas1 canvas;
  GFXcanvas1 fish_mask;   // dithered style only: which canvas pixels belong to a fish
  uint8_t _col_map[SCALED_W];
  uint8_t _row_map[SCALED_H];
  bool _init = false;
  bool _isOn = false;
  bool _full_refresh_pending = false;
  uint16_t _color = 0;
  uint16_t _bkg = 1;
  uint32_t _last_hash = 0;
  uint32_t _last_full_ms = 0;
  bool _dithered = false;
  bool _mark_fish = false;
  int _water_top = 0;
  int _water_bottom = 0;

  uint32_t frameHash() const;
  void scaleToPanel();
  bool ditheredPixel(int cx, int cy, int px, int py);

public:
  EinkCanvasDisplay() : DisplayDriver(CANVAS_W, CANVAS_H),
      display(EINK_DISPLAY_MODEL(PIN_DISPLAY_CS, PIN_DISPLAY_DC, PIN_DISPLAY_RST, PIN_DISPLAY_BUSY)),
      canvas(CANVAS_W, CANVAS_H), fish_mask(CANVAS_W, CANVAS_H) {}

  bool begin();

  bool isOn() override { return _isOn; }
  bool isEink() override { return true; }
  void forceFullRefresh() override { _full_refresh_pending = true; }
  void turnOn() override;
  void turnOff() override;
  void clear() override;
  void startFrame(ColorVal bkg = UIColor::window_bkg) override;
  void setTextSize(int sz) override;
  void setColor(ColorVal c) override;
  void setCursor(int x, int y) override;
  void print(const char* str) override;
  void fillRect(int x, int y, int w, int h) override;
  void drawRect(int x, int y, int w, int h) override;
  void drawXbm(int x, int y, const uint8_t* bits, int w, int h) override;
  uint16_t getTextWidth(const char* str) override;
  void endFrame() override;

  // The two looks offered on this panel: plain mono, or dithered mono with grey fish
  // bodies and water that darkens towards the tank floor.
  void setDithered(bool on) { _dithered = on; }
  void setFishShading(bool on) { _mark_fish = on; }
  void setWaterRows(int top, int bottom) { _water_top = top; _water_bottom = bottom; }
};

// Everything drawn while one of these exists counts as part of a fish.
class EinkFishShading {
  EinkCanvasDisplay& _display;
public:
  explicit EinkFishShading(EinkCanvasDisplay& display) : _display(display) { _display.setFishShading(true); }
  ~EinkFishShading() { _display.setFishShading(false); }
};
