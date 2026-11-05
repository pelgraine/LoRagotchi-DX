#include "EinkCanvasDisplay.h"

// One bit per pixel: 1 is paper white, 0 is black. The pet's black-and-white themes use
// the colours 0 and 1, and the driver defaults in GxEPDDisplay.cpp use GxEPD_BLACK (0) and
// GxEPD_WHITE (0xFFFF), so any non-zero colour is white.
static inline uint16_t canvasColor(ColorVal c) { return c ? 1 : 0; }

bool EinkCanvasDisplay::begin() {
  if (canvas.getBuffer() == nullptr || fish_mask.getBuffer() == nullptr) return false;

  for (int x = 0; x < SCALED_W; x++) _col_map[x] = (uint8_t)(x * CANVAS_W / SCALED_W);
  for (int y = 0; y < SCALED_H; y++) _row_map[y] = (uint8_t)(y * CANVAS_H / SCALED_H);

  display.epd2.selectSPI(SPI1, SPISettings(4000000, MSBFIRST, SPI_MODE0));
  SPI1.begin();
  display.init(115200, true, 2, false);
  display.setRotation(DISPLAY_ROTATION);
  display.setFullWindow();
  display.setPartialWindow(0, 0, display.width(), display.height());

  canvas.cp437(true);   // same character set as the OLED driver

  // Clear the panel on the first picture: a full wipe to white, then that picture.
  _full_refresh_pending = true;
  _init = true;
  _isOn = true;
  return true;
}

void EinkCanvasDisplay::turnOn() {
  if (!_init) begin();
  _isOn = true;
}

void EinkCanvasDisplay::turnOff() {
  // The panel keeps its last picture with no power, so the screen timeout freezes the
  // tank instead of blanking it. powerOff() keeps the controller's memory, so the next
  // picture can be a partial refresh with no flash.
  _isOn = false;
  if (_init) display.powerOff();
}

void EinkCanvasDisplay::clear() {
  canvas.fillScreen(1);
  _bkg = 1;
}

void EinkCanvasDisplay::startFrame(ColorVal bkg) {
  _bkg = canvasColor(bkg);
  canvas.fillScreen(_bkg);
  _color = canvasColor(UIColor::primary_txt);
  canvas.setTextColor(_color);
  canvas.setTextSize(1);
  fish_mask.fillScreen(0);
  _water_top = _water_bottom = 0;
}

void EinkCanvasDisplay::setTextSize(int sz) {
  canvas.setTextSize(sz);
}

void EinkCanvasDisplay::setColor(ColorVal c) {
  _color = canvasColor(c);
  canvas.setTextColor(_color);
}

void EinkCanvasDisplay::setCursor(int x, int y) {
  canvas.setCursor(x, y);
}

void EinkCanvasDisplay::print(const char* str) {
  canvas.print(str);
}

void EinkCanvasDisplay::fillRect(int x, int y, int w, int h) {
  canvas.fillRect(x, y, w, h, _color);
  if (_mark_fish) fish_mask.fillRect(x, y, w, h, 1);
}

void EinkCanvasDisplay::drawRect(int x, int y, int w, int h) {
  canvas.drawRect(x, y, w, h, _color);
  if (_mark_fish) fish_mask.drawRect(x, y, w, h, 1);
}

void EinkCanvasDisplay::drawXbm(int x, int y, const uint8_t* bits, int w, int h) {
  canvas.drawBitmap(x, y, bits, w, h, _color);
  if (_mark_fish) fish_mask.drawBitmap(x, y, bits, w, h, 1);
}

uint16_t EinkCanvasDisplay::getTextWidth(const char* str) {
  int16_t x1, y1;
  uint16_t w, h;
  canvas.getTextBounds(str, 0, 0, &x1, &y1, &w, &h);
  return w;
}

uint32_t EinkCanvasDisplay::frameHash() const {
  const uint8_t* bytes = const_cast<GFXcanvas1&>(canvas).getBuffer();
  const size_t size = ((CANVAS_W + 7) / 8) * CANVAS_H;
  uint32_t hash = 2166136261UL;   // FNV-1a
  for (size_t i = 0; i < size; i++) {
    hash ^= bytes[i];
    hash *= 16777619UL;
  }
  if (_dithered) {
    const uint8_t* mask = fish_mask.getBuffer();
    for (size_t i = 0; i < size; i++) {
      hash ^= mask[i];
      hash *= 16777619UL;
    }
    hash ^= (uint32_t)_water_top << 8 | (uint32_t)_water_bottom;
    hash *= 16777619UL;
  }
  return hash ^ (_dithered ? 1 : 0);
}

// True where the foreground colour goes. Solid lines and text stay solid, a fish keeps a
// solid outline around a 50% body, and the bottom half of the water gets light dots that
// double in the deepest quarter. The patterns use panel pixels, so they are finer than the
// picture and stay still.
bool EinkCanvasDisplay::ditheredPixel(int cx, int cy, int px, int py) {
  const bool foreground = canvas.getPixel(cx, cy) != (_bkg != 0);
  const bool fish = fish_mask.getPixel(cx, cy);
  if (foreground) {
    if (!fish) return true;
    const bool interior = cx > 0 && cx < CANVAS_W - 1 && cy > 0 && cy < CANVAS_H - 1 &&
        fish_mask.getPixel(cx - 1, cy) && fish_mask.getPixel(cx + 1, cy) &&
        fish_mask.getPixel(cx, cy - 1) && fish_mask.getPixel(cx, cy + 1) &&
        canvas.getPixel(cx - 1, cy) != (_bkg != 0) && canvas.getPixel(cx + 1, cy) != (_bkg != 0) &&
        canvas.getPixel(cx, cy - 1) != (_bkg != 0) && canvas.getPixel(cx, cy + 1) != (_bkg != 0);
    return interior ? ((px + py) & 1) == 0 : true;
  }
  if (fish || cy < _water_top || cy >= _water_bottom) return false;
  const int depth = (cy - _water_top) * 4 / (_water_bottom - _water_top);
  if (depth < 2) return false;
  if (depth == 2) return (px & 3) == 0 && (py & 3) == 0;
  return (py & 1) == 0 && ((px + (py & 2)) & 3) == 0;
}

void EinkCanvasDisplay::scaleToPanel() {
  const uint8_t* bytes = canvas.getBuffer();
  const int stride = (CANVAS_W + 7) / 8;
  const uint16_t background = _bkg ? GxEPD_WHITE : GxEPD_BLACK;
  display.fillScreen(background);
  if (_dithered) {
    const uint16_t foreground = _bkg ? GxEPD_BLACK : GxEPD_WHITE;
    for (int y = 0; y < SCALED_H; y++) {
      for (int x = 0; x < SCALED_W; x++) {
        if (ditheredPixel(_col_map[x], _row_map[y], OFFSET_X + x, y)) {
          display.drawPixel(OFFSET_X + x, y, foreground);
        }
      }
    }
    return;
  }
  for (int y = 0; y < SCALED_H; y++) {
    const uint8_t* row = bytes + _row_map[y] * stride;
    for (int x = 0; x < SCALED_W; x++) {
      const uint8_t col = _col_map[x];
      const bool white = row[col >> 3] & (0x80 >> (col & 7));
      display.drawPixel(OFFSET_X + x, y, white ? GxEPD_WHITE : GxEPD_BLACK);
    }
  }
}

void EinkCanvasDisplay::endFrame() {
  if (!_isOn) return;

  const uint32_t hash = frameHash();
  if (hash == _last_hash && !_full_refresh_pending) return;   // nothing changed on screen
  _last_hash = hash;

  const uint32_t now = millis();
  if (now - _last_full_ms >= EINK_FULL_REFRESH_MS) _full_refresh_pending = true;

  scaleToPanel();
  if (_full_refresh_pending) {
    // Wipe to solid white with a full-waveform update first, so there is a real
    // black/white transition to force out accumulated ghosting.
    display.clearScreen(0xFF);
    _full_refresh_pending = false;
    _last_full_ms = now;
  }
  // Always push content with a partial update: this writes only the current register,
  // leaving the previous one as the white just wiped to, or as the previous picture.
  // A genuine difference between the two is what drives pixels fully black.
  display.display(true);
}
