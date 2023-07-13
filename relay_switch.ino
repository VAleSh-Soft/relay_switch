#include <shButton.h>
#include <EEPROM.h>
#include <FastLED.h> // https://github.com/FastLED/FastLED

// ==== управляющий уровень реле =====================
const uint8_t control_level = HIGH; // если реле управляются низким уровнем, установите LOW вместо HIGH

// ==== тип кнопки ===================================
#define USE_TOUCH_BUTTON // расскомментируйте, если используете сенсорные кнопки

// ==== индекс в EEPROM для хранения настроек ========
uint8_t i_data = 0x0a;

#if defined(__AVR_ATmega328P__) || defined(__AVR_ATmega328PB__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega168__) || defined(__AVR_ATmega168P__) || defined(__AVR_ATmega8__)
// ==== пины кнопок ==================================
const uint8_t BTN_1_PIN = 9;
const uint8_t BTN_2_PIN = 8;
const uint8_t BTN_3_PIN = 7;
const uint8_t BTN_4_PIN = 6;
const uint8_t BTN_5_PIN = 5;
const uint8_t BTN_6_PIN = 4;
const uint8_t BTN_7_PIN = 3;
const uint8_t BTN_8_PIN = 2;
// ==== пины реле ====================================
const uint8_t RELAY_1_PIN = 10;
const uint8_t RELAY_2_PIN = 11;
const uint8_t RELAY_3_PIN = 12;
const uint8_t RELAY_4_PIN = 13;
const uint8_t RELAY_5_PIN = 14; // A0
const uint8_t RELAY_6_PIN = 15; // A1
const uint8_t RELAY_7_PIN = 16; // A2
const uint8_t RELAY_8_PIN = 17; // A3
// ==== пин управления адресными светодиодами ========
const uint8_t LEDS_DATA_PIN = 18 // A4;
#elif defined(__AVR_ATtiny48__) || defined(__AVR_ATtiny88__)
// ==== пины кнопок ==================================
const uint8_t BTN_1_PIN = 10;
const uint8_t BTN_2_PIN = 9;
const uint8_t BTN_3_PIN = 8;
const uint8_t BTN_4_PIN = 7;
const uint8_t BTN_5_PIN = 6;
const uint8_t BTN_6_PIN = 5;
const uint8_t BTN_7_PIN = 4;
const uint8_t BTN_8_PIN = 3;
// ==== пины реле ====================================
const uint8_t RELAY_1_PIN = 22; // A3
const uint8_t RELAY_2_PIN = 21; // A2
const uint8_t RELAY_3_PIN = 20; // A1
const uint8_t RELAY_4_PIN = 19; // A0
const uint8_t RELAY_5_PIN = 18; // A7
const uint8_t RELAY_6_PIN = 17; // A6
const uint8_t RELAY_7_PIN = 16;
const uint8_t RELAY_8_PIN = 15;
// ==== пин управления адресными светодиодами ========
const uint8_t LEDS_DATA_PIN = 13;
#endif

// ==== массив кнопок ================================
#ifdef USE_TOUCH_BUTTON
uint8_t input_type = PULL_DOWN;
#else
uint8_t input_type = PULL_UP;
#endif

shButton btns[] =
    {
        (shButton){BTN_1_PIN, input_type},
        (shButton){BTN_2_PIN, input_type},
        (shButton){BTN_3_PIN, input_type},
        (shButton){BTN_4_PIN, input_type},
        (shButton){BTN_5_PIN, input_type},
        (shButton){BTN_6_PIN, input_type},
        (shButton){BTN_7_PIN, input_type},
        (shButton){BTN_8_PIN, input_type}};

// ==== массив пинов реле ============================
uint8_t relay_pin[] = {RELAY_1_PIN, RELAY_2_PIN, RELAY_3_PIN, RELAY_4_PIN,
                       RELAY_5_PIN, RELAY_6_PIN, RELAY_7_PIN, RELAY_8_PIN};

CRGB leds[8]; // массив адресных светодиодов - индикаторов каналов

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