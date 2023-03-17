#include <shButton.h>
#include <EEPROM.h>
#include <FastLED.h> // https://github.com/FastLED/FastLED

// ==== управляющий уровень реле =====================
const uint8_t control_level = HIGH; // если реле управляются низким уровнем, установите LOW вместо HIGH

// ==== индекс в EEPROM для хранения настроек ========
uint8_t i_data = 0x0a;

// ==== пины реле ====================================
const uint8_t RELAY_1_PIN = 0;
const uint8_t RELAY_2_PIN = 1;
const uint8_t RELAY_3_PIN = 2;
const uint8_t RELAY_4_PIN = 3;
const uint8_t RELAY_5_PIN = 4;
const uint8_t RELAY_6_PIN = 5;
const uint8_t RELAY_7_PIN = 6;
const uint8_t RELAY_8_PIN = 7;

// ==== пины кнопок ==================================
const uint8_t BTN_1_PIN = 9;
const uint8_t BTN_2_PIN = 10;
const uint8_t BTN_3_PIN = 11;
const uint8_t BTN_4_PIN = 12;
const uint8_t BTN_5_PIN = 13;
const uint8_t BTN_6_PIN = 14;   // A0;
const uint8_t BTN_7_PIN = 15;   // A1;
const uint8_t BTN_8_PIN = 16;   // A2;
const uint8_t BTN_OFF_PIN = 17; // A3;

// ==== пин управления адресными светодиодами ========
const uint8_t LEDS_DATA_PIN = 23;

// ===================================================
// shButton btn_off(BTN_OFF_PIN);

shButton btns[] =
    {
        (shButton){BTN_1_PIN},
        (shButton){BTN_2_PIN},
        (shButton){BTN_3_PIN},
        (shButton){BTN_4_PIN},
        (shButton){BTN_5_PIN},
        (shButton){BTN_6_PIN},
        (shButton){BTN_7_PIN},
        (shButton){BTN_8_PIN}};

uint8_t relay_pin[] = {RELAY_1_PIN, RELAY_2_PIN, RELAY_3_PIN, RELAY_4_PIN,
                       RELAY_5_PIN, RELAY_6_PIN, RELAY_7_PIN, RELAY_8_PIN};

CRGB leds[8]; // массив адресных светодиодов-индикаторов каналов

// ===================================================
bool readData(uint8_t _index)
{
  return ((bool)EEPROM.read(i_data + _index));
}

void writeData(uint8_t _index, bool _data)
{
  EEPROM.update(i_data + _index, (byte)_data);
}

void setLeds()
{
  for (uint8_t i = 0; i < 8; i++)
  {
    CRGB color = CRGB::Black;
    if (readData(i))
    {
      color = (digitalRead(relay_pin[i])) ? CRGB::Green : CRGB::Red;
    }
    leds[i] = color;
  }
  FastLED.show();
}

void setup()
{
  for (uint8_t i = 0; i < 8; i++)
  {
    digitalWrite(relay_pin[i], !control_level);
    pinMode(relay_pin[i], OUTPUT);
    btns[i].setVirtualClickOn();
  }

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

  // управление светодиодами
  static uint32_t timer = 0;
  if (millis() - timer >= 100)
  {
    timer = millis();
    setLeds();
  }
}