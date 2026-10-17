#pragma once

// Colour display for the LilyGo T-Watch S3: ST7789V 240x240 IPS on SPI3 with a
// PWM backlight (GPIO45), and the FT6336U capacitive touch panel on Wire1
// (SDA 39 / SCL 40, INT 16). Panel and touch configuration ported from the
// Meck T-Watch firmware.
//
// The LoRaGotchi UI is laid out for a 128x64 canvas. It is drawn into a
// 128x64 RGB565 frame buffer, which endFrame() scales by 1.875 (240/128) to a
// full-width 240x120 band in the middle of the square panel, keeping the
// layout's proportions. The strips above and below are filled with the frame
// background colour.

#include <helpers/ui/DisplayDriver.h>

#define LGFX_USE_V1
#include <LovyanGFX.hpp>

class LGFX_TWatchS3 : public lgfx::LGFX_Device {
  lgfx::Panel_ST7789  _panel_instance;
  lgfx::Bus_SPI       _bus_instance;
  lgfx::Light_PWM     _light_instance;
  lgfx::Touch_FT5x06  _touch_instance;

public:
  LGFX_TWatchS3(void) {
    {
      auto cfg = _bus_instance.config();
      cfg.spi_host   = SPI3_HOST;
      cfg.spi_mode   = 0;
      cfg.freq_write = 40000000;
      cfg.freq_read  = 16000000;
      cfg.spi_3wire  = true;
      cfg.use_lock   = true;
      cfg.dma_channel = SPI_DMA_CH_AUTO;
      cfg.pin_sclk = 18;
      cfg.pin_mosi = 13;
      cfg.pin_miso = -1;
      cfg.pin_dc   = 38;
      _bus_instance.config(cfg);
      _panel_instance.setBus(&_bus_instance);
    }
    {
      auto cfg = _panel_instance.config();
      cfg.pin_cs   = 12;
      cfg.pin_rst  = -1;
      cfg.pin_busy = -1;
      // ST7789 GRAM is 240x320; memory_height must be 320 so the 80px rotation
      // offset is applied (otherwise a strip of the panel shows noise).
      cfg.memory_width    = 240;
      cfg.memory_height   = 320;
      cfg.panel_width     = 240;
      cfg.panel_height    = 240;
      cfg.offset_x        = 0;
      cfg.offset_y        = 0;
      cfg.offset_rotation = 1;
      cfg.readable   = false;
      cfg.invert     = true;
      cfg.rgb_order  = false;
      cfg.dlen_16bit = false;
      cfg.bus_shared = false;
      _panel_instance.config(cfg);
    }
    {
      auto cfg = _light_instance.config();
      cfg.pin_bl      = 45;
      cfg.invert      = false;
      cfg.freq        = 44100;
      cfg.pwm_channel = 7;
      _light_instance.config(cfg);
      _panel_instance.setLight(&_light_instance);
    }
    {
      auto cfg = _touch_instance.config();
      cfg.x_min      = 0;
      cfg.x_max      = 239;
      cfg.y_min      = 0;
      cfg.y_max      = 239;
      cfg.pin_int    = 16;
      cfg.pin_rst    = -1;
      cfg.bus_shared = false;
      cfg.offset_rotation = 2;  // touch IC mounted 180deg vs the LCD horizontal axis
      cfg.i2c_port = 1;
      cfg.i2c_addr = 0x38;
      cfg.pin_sda  = 39;
      cfg.pin_scl  = 40;
      cfg.freq     = 400000;
      _touch_instance.config(cfg);
      _panel_instance.setTouch(&_touch_instance);
    }
    setPanel(&_panel_instance);
  }
};

class TWatchS3Display : public DisplayDriver {
  LGFX_TWatchS3 _lcd;
  LGFX_Sprite _frame;
  bool _isOn = false;
  bool _buffered = false;
  uint16_t _color = 0xFFFF;
  uint16_t _bkg = 0x0000;
  bool _strips_valid = false;
  uint16_t _strip_color = 0x0000;
  uint8_t _brightness = 255;

public:
  static const int LOGICAL_WIDTH = 128;
  static const int LOGICAL_HEIGHT = 64;

  TWatchS3Display() : DisplayDriver(LOGICAL_WIDTH, LOGICAL_HEIGHT), _frame(&_lcd) {}

  bool begin();

  bool supportsColor() const override { return true; }
  bool isOn() override { return _isOn; }
  void turnOn() override;
  void turnOff() override;
  void setBrightness(uint8_t brightness) override;
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

  // Touch position in panel pixels (0..239, same orientation as the display).
  // Returns false when no finger is reported.
  bool getTouch(int* x, int* y);
};
