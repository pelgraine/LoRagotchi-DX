#include "TWatchS3Display.h"
#include "variant.h"

// RGB565 defaults; the LoRaGotchi UI replaces these with its own theme.
ColorVal UIColor::window_bkg = 0x0000;
ColorVal UIColor::title_bkg = 0x001F;
ColorVal UIColor::title_txt = 0xFFFF;
ColorVal UIColor::primary_txt = 0xFFFF;
ColorVal UIColor::secondary_txt = 0xBDF7;
ColorVal UIColor::warning_txt = 0xFD20;
ColorVal UIColor::popup_bkg = 0x001F;
ColorVal UIColor::popup_txt = 0xFFFF;
ColorVal UIColor::corp_blue = 0x001A;

// Scale from the 128x64 layout to the panel width, and the band's position.
static const float BAND_ZOOM = (float)LCD_HOR_SIZE / TWatchS3Display::LOGICAL_WIDTH;  // 1.875
static const int BAND_HEIGHT = (int)(TWatchS3Display::LOGICAL_HEIGHT * BAND_ZOOM);   // 120
static const int BAND_TOP = (LCD_VER_SIZE - BAND_HEIGHT) / 2;                         // 60

bool TWatchS3Display::begin() {
  _lcd.init();
  _lcd.setRotation(1);
  _lcd.setColorDepth(16);
  _lcd.fillScreen((uint16_t)0x0000);
  _lcd.setBrightness(_brightness);

  _frame.setColorDepth(16);
  _frame.setPsram(false);
  _buffered = _frame.createSprite(LOGICAL_WIDTH, LOGICAL_HEIGHT) != nullptr;
  _frame.setTextSize(1);
  _isOn = true;
  _strips_valid = false;
  return _buffered;
}

void TWatchS3Display::turnOn() {
  if (!_isOn) {
    _lcd.wakeup();
    _lcd.setBrightness(_brightness);
    _isOn = true;
    _strips_valid = false;
  }
}

void TWatchS3Display::turnOff() {
  if (_isOn) {
    _lcd.sleep();   // backlight to 0 and panel sleep
    _isOn = false;
  }
}

void TWatchS3Display::setBrightness(uint8_t brightness) {
  _brightness = brightness;
  if (_isOn) _lcd.setBrightness(brightness);   // never light a sleeping panel
}

void TWatchS3Display::clear() {
  if (_buffered) _frame.fillScreen((uint16_t)0x0000);
  _bkg = 0x0000;
}

void TWatchS3Display::startFrame(ColorVal bkg) {
  _bkg = (uint16_t)bkg;
  if (_buffered) {
    _frame.fillScreen(_bkg);
    _frame.setCursor(0, 0);
  }
  // Same as SSD1306Display: each frame starts in the primary text colour.
  _color = (uint16_t)UIColor::primary_txt;
  _frame.setTextColor(_color);
}

void TWatchS3Display::setTextSize(int sz) {
  _frame.setTextSize(sz);
}

void TWatchS3Display::setColor(ColorVal c) {
  _color = (uint16_t)c;
  _frame.setTextColor(_color);
}

void TWatchS3Display::setCursor(int x, int y) {
  _frame.setCursor(x, y);
}

void TWatchS3Display::print(const char* str) {
  if (_buffered) _frame.print(str);
}

void TWatchS3Display::fillRect(int x, int y, int w, int h) {
  if (_buffered) _frame.fillRect(x, y, w, h, _color);
}

void TWatchS3Display::drawRect(int x, int y, int w, int h) {
  if (_buffered) _frame.drawRect(x, y, w, h, _color);
}

void TWatchS3Display::drawXbm(int x, int y, const uint8_t* bits, int w, int h) {
  // The shared icons are MSB-first (as drawn by SSD1306Display via Adafruit
  // drawBitmap); LovyanGFX's drawBitmap uses the same order, drawXBitmap does not.
  if (_buffered) _frame.drawBitmap(x, y, bits, w, h, _color);
}

uint16_t TWatchS3Display::getTextWidth(const char* str) {
  return _frame.textWidth(str);
}

void TWatchS3Display::endFrame() {
  if (!_isOn || !_buffered) return;
  _lcd.startWrite();
  if (!_strips_valid || _strip_color != _bkg) {
    _lcd.fillRect(0, 0, LCD_HOR_SIZE, BAND_TOP, _bkg);
    _lcd.fillRect(0, BAND_TOP + BAND_HEIGHT, LCD_HOR_SIZE, LCD_VER_SIZE - BAND_TOP - BAND_HEIGHT, _bkg);
    _strip_color = _bkg;
    _strips_valid = true;
  }
  _frame.pushRotateZoom(&_lcd, LCD_HOR_SIZE / 2.0f, BAND_TOP + BAND_HEIGHT / 2.0f, 0, BAND_ZOOM, BAND_ZOOM);
  _lcd.endWrite();
}

bool TWatchS3Display::getTouch(int* x, int* y) {
  lgfx::touch_point_t point;
  if (_lcd.getTouch(&point) == 0) return false;
  *x = point.x;
  *y = point.y;
  return true;
}
