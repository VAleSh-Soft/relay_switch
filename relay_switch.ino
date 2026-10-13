#include <shButton.h>
#include <EEPROM.h>
#include <FastLED.h> // https://github.com/FastLED/FastLED
#include "header_file.h"

// ===================================================

// считывание данных из EEPROM
bool readData(uint8_t _index)
{
  return ((bool)EEPROM.read(i_data + _index));
}

// запись данных в EEPROM
void writeData(uint8_t _index, bool _data)
{
  EEPROM.update(i_data + _index, (byte)_data);
}

// управление светодиодами
void setLeds()
{
  for (uint8_t i = 0; i < 8; i++)
  {
    CRGB color = CRGB::Black;
    if (readData(i))
    {
      color = (digitalRead(relay_pin[i]) == control_level) ? CRGB::Green : CRGB::Red;
    }
    leds[i] = color;
  }
  FastLED.show();
}

void setup()
{
  // настройка кнопок и реле
  for (uint8_t i = 0; i < 8; i++)
  {
    digitalWrite(relay_pin[i], !control_level);
    pinMode(relay_pin[i], OUTPUT);
    btns[i].setVirtualClickOn();
    btns[i].setLongClickMode(LCM_ONLYONCE);
    // отключение канала по удержанию кнопки в течение 2 секунд
    btns[i].setTimeoutOfLongClick(2000);
    // т.к. двойной клик не используется, уменьшаем его интервал, чтобы ускорить реакцию на одиночный клик
    btns[i].setTimeoutOfDblClick(100);
#ifdef USE_TOUCH_BUTTON
    // для сенсорных кнопок установить нулевой антидребезг
    btns[i].setTimeoutOfDebounce(0);
#endif
  }

  // настройка светодиодов
  FastLED.addLeds<WS2812B, LEDS_DATA_PIN, GRB>(leds, 8);
  FastLED.setBrightness(5);
  setLeds();
}

void loop()
{
  for (uint8_t i = 0; i < 8; i++)
  {
    switch (btns[i].getButtonState())
    {
    // переключить реле при коротком клике на кнопку канала
    case BTN_ONECLICK:
      uint8_t data;
      data = (readData(i)) ? !digitalRead(relay_pin[i]) : !control_level;
      digitalWrite(relay_pin[i], data);
      break;
    // отключить или включить управление каналом при длинном клике на кнопку канала
    case BTN_LONGCLICK:
      bool pin_enabled;
      pin_enabled = !readData(i);
      writeData(i, pin_enabled);
      if (!pin_enabled)
      {
        digitalWrite(relay_pin[i], !control_level);
      }
      break;
    }
  }

  // управление светодиодами - один раз в 100 мс
  static uint32_t timer = 0;
  if (millis() - timer >= 100)
  {
    timer = millis();
    setLeds();
  }
}