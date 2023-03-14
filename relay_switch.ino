#include <shButton.h>
#include "header_file.h"

shButton btn_off(BTN_OFF_PIN);
shButton btn_1(BTN_1_PIN);
shButton btn_2(BTN_2_PIN);
shButton btn_3(BTN_3_PIN);
shButton btn_4(BTN_4_PIN);
shButton btn_5(BTN_5_PIN);
shButton btn_6(BTN_6_PIN);
shButton btn_7(BTN_7_PIN);
shButton btn_8(BTN_8_PIN);

shButton btns[] = {btn_1, btn_2, btn_3, btn_4,
                   btn_5, btn_6, btn_7, btn_8};

uint8_t relay_pin[] = {RELAY_1_PIN, RELAY_2_PIN, RELAY_3_PIN, RELAY_4_PIN,
                       RELAY_5_PIN, RELAY_6_PIN, RELAY_7_PIN, RELAY_8_PIN};

void setup()
{
  for (uint8_t i = 0; i < 8; i++)
  {
    pinMode(relay_pin[i], OUTPUT);
  }
}

void loop()
{
  if (btn_off.getButtonState() == BTN_DOWN)
  {
    for (uint8_t i = 0; i < 8; i++)
    {
      digitalWrite(relay_pin[i], LOW);
    }
  }

  for (uint8_t i = 0; i < 8; i++)
  {
    if (btns[i].getButtonState() == BTN_DOWN)
    {
      digitalWrite(relay_pin[i], digitalRead(relay_pin[i]));
    }
  }
}