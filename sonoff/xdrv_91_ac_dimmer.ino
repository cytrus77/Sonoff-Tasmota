/*
  xdrv_91_ac_dimmer.ino - PWM sonoff led support for Sonoff-Tasmota

  Copyright (C) 2019  Theo Arends

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**************
 *  RobotDyn
 *  Dimmer Library
 *  **************
 *
 *  The following sketch is meant to to define dimming value through potentiometer,
 *  The potentiometer values are changing in range from 0 to 1023
 *  potentiometer values are converted through the map function to values from 0 to 100% and saved in dimmer.setPower(outVal);
 *  Serial.begin is used to display dimming values
 *
 *
 *  ---------------------- OUTPUT & INPUT Pin table ---------------------
 *  +---------------+-------------------------+-------------------------+
 *  |   Board       | INPUT Pin               | OUTPUT Pin              |
 *  |               | Zero-Cross              |                         |
 *  +---------------+-------------------------+-------------------------+
 *  | Lenardo       | D7 (NOT CHANGABLE)      | D0-D6, D8-D13           |
 *  +---------------+-------------------------+-------------------------+
 *  | Mega          | D2 (NOT CHANGABLE)      | D0-D1, D3-D70           |
 *  +---------------+-------------------------+-------------------------+
 *  | Uno           | D2 (NOT CHANGABLE)      | D0-D1, D3-D20           |
 *  +---------------+-------------------------+-------------------------+
 *  | ESP8266       | D1(IO5),    D2(IO4),    | D0(IO16),   D1(IO5),    |
 *  |               | D5(IO14),   D6(IO12),   | D2(IO4),    D5(IO14),   |
 *  |               | D7(IO13),   D8(IO15),   | D6(IO12),   D7(IO13),   |
 *  |               |                         | D8(IO15)                |
 *  +---------------+-------------------------+-------------------------+
 *  | ESP32         | 4(GPI36),   6(GPI34),   | 8(GPO32),   9(GP033),   |
 *  |               | 5(GPI39),   7(GPI35),   | 10(GPIO25), 11(GPIO26), |
 *  |               | 8(GPO32),   9(GP033),   | 12(GPIO27), 13(GPIO14), |
 *  |               | 10(GPI025), 11(GPIO26), | 14(GPIO12), 16(GPIO13), |
 *  |               | 12(GPIO27), 13(GPIO14), | 23(GPIO15), 24(GPIO2),  |
 *  |               | 14(GPIO12), 16(GPIO13), | 25(GPIO0),  26(GPIO4),  |
 *  |               | 21(GPIO7),  23(GPIO15), | 27(GPIO16), 28(GPIO17), |
 *  |               | 24(GPIO2),  25(GPIO0),  | 29(GPIO5),  30(GPIO18), |
 *  |               | 26(GPIO4),  27(GPIO16), | 31(GPIO19), 33(GPIO21), |
 *  |               | 28(GPIO17), 29(GPIO5),  | 34(GPIO3),  35(GPIO1),  |
 *  |               | 30(GPIO18), 31(GPIO19), | 36(GPIO22), 37(GPIO23), |
 *  |               | 33(GPIO21), 35(GPIO1),  |                         |
 *  |               | 36(GPIO22), 37(GPIO23), |                         |
 *  +---------------+-------------------------+-------------------------+
 *  | Arduino M0    | D7 (NOT CHANGABLE)      | D0-D6, D8-D13           |
 *  | Arduino Zero  |                         |                         |
 *  +---------------+-------------------------+-------------------------+
 *  | Arduino Due   | D0-D53                  | D0-D53                  |
 *  +---------------+-------------------------+-------------------------+
 *  | STM32         | PA0-PA15,PB0-PB15       | PA0-PA15,PB0-PB15       |
 *  | Black Pill    | PC13-PC15               | PC13-PC15               |
 *  | BluePill      |                         |                         |
 *  | Etc...        |                         |                         |
 *  +---------------+-------------------------+-------------------------+
 */

//#define XDRV_91              91

#ifdef NOCOMPILE

#include <RBDdimmer.h>//

#define outputPin  12
#define zerocross  5 // for boards with CHANGEBLE input pins

enum AcDimmerCommands {
  CMND_DIMMER, CMND_LED, CMND_LEDTABLE, CMND_FADE,
  CMND_ROTATION, CMND_SCHEME, CMND_WAKEUP, CMND_WAKEUPDURATION,
  CMND_WHITE, CMND_WIDTH, CMND_CHANNEL, CMND_UNDOCA };
const char kAcDimmerCommands[] PROGMEM =
  D_CMND_DIMMER "|" D_CMND_LED "|" D_CMND_LEDTABLE "|" D_CMND_FADE "|"
  D_CMND_ROTATION "|" D_CMND_SCHEME "|" D_CMND_WAKEUP "|" D_CMND_WAKEUPDURATION "|"
  D_CMND_WHITE "|" D_CMND_WIDTH "|" D_CMND_CHANNEL "|UNDOCA" ;

uint8_t ledTable[] = {
  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
  1,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,  3,  3,  4,  4,
  4,  4,  4,  5,  5,  5,  5,  6,  6,  6,  6,  7,  7,  7,  7,  8,
  8,  8,  9,  9,  9, 10, 10, 10, 11, 11, 12, 12, 12, 13, 13, 14,
 14, 15, 15, 15, 16, 16, 17, 17, 18, 18, 19, 19, 20, 20, 21, 22,
 22, 23, 23, 24, 25, 25, 26, 26, 27, 28, 28, 29, 30, 30, 31, 32,
 33, 33, 34, 35, 36, 36, 37, 38, 39, 40, 40, 41, 42, 43, 44, 45,
 46, 46, 47, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60,
 61, 62, 63, 64, 65, 67, 68, 69, 70, 71, 72, 73, 75, 76, 77, 78,
 80, 81, 82, 83, 85, 86, 87, 89, 90, 91, 93, 94, 95, 97, 98, 99,
101,102,104,105,107,108,110,111,113,114,116,117,119,121,122,124,
125,127,129,130,132,134,135,137,139,141,142,144,146,148,150,151,
153,155,157,159,161,163,165,166,168,170,172,174,176,178,180,182,
184,186,189,191,193,195,197,199,201,204,206,208,210,212,215,217,
219,221,224,226,228,231,233,235,238,240,243,245,248,250,253,255 };

uint8_t light_wheel = 0;
uint8_t light_subtype = 0;
uint8_t light_device = 0;
uint8_t light_power = 0;
uint8_t light_old_power = 1;
uint8_t light_update = 1;
uint8_t light_wakeup_active = 0;
uint8_t light_wakeup_dimmer = 0;
uint16_t light_wakeup_counter = 0;

unsigned long strip_timer_counter = 0;    // Bars and Gradient

/*********************************************************************************************\
 * Sonoff B1 and AiAcDimmer inspired by OpenAcDimmer https://github.com/icamgo/noduino-sdk
\*********************************************************************************************/

extern "C" {
  void os_delay_us(unsigned int);
}

/********************************************************************************************/

void AcDimmerInit(void)
{
  uint8_t max_scheme = LS_MAX -1;

  light_device = devices_present;
  light_subtype = light_type &7;        // Always 0 - 7

  if (LST_SINGLE == light_subtype) {
    Settings.light_color[0] = 255;      // One channel only supports Dimmer but needs max color
  }
  if (light_type < LT_PWM6) {           // PWM
    for (uint8_t i = 0; i < light_type; i++) {
      Settings.pwm_value[i] = 0;        // Disable direct PWM control
      if (pin[GPIO_PWM1 +i] < 99) {
        pinMode(pin[GPIO_PWM1 +i], OUTPUT);
      }
    }
    if (SONOFF_LED == my_module_type) { // Fix Sonoff Led instabilities
      if (!my_module.io[4]) {
        pinMode(4, OUTPUT);             // Stop floating outputs
        digitalWrite(4, LOW);
      }
      if (!my_module.io[5]) {
        pinMode(5, OUTPUT);             // Stop floating outputs
        digitalWrite(5, LOW);
      }
      if (!my_module.io[14]) {
        pinMode(14, OUTPUT);            // Stop floating outputs
        digitalWrite(14, LOW);
      }
    }
    if (pin[GPIO_ARIRFRCV] < 99) {
      if (pin[GPIO_LED4] < 99) {
        digitalWrite(pin[GPIO_LED4], bitRead(led_inverted, 3));  // Turn off RF
      }
    }
  }
  else {
    light_pdi_pin = pin[GPIO_DI];
    light_pdcki_pin = pin[GPIO_DCKI];

    pinMode(light_pdi_pin, OUTPUT);
    pinMode(light_pdcki_pin, OUTPUT);
    digitalWrite(light_pdi_pin, LOW);
    digitalWrite(light_pdcki_pin, LOW);
  }

  if (light_subtype < LST_RGB) {
    max_scheme = LS_POWER;
  }
  if ((LS_WAKEUP == Settings.light_scheme) || (Settings.light_scheme > max_scheme)) {
    Settings.light_scheme = LS_POWER;
  }
}

void AcDimmerSetDimmer(uint8_t myDimmer)
{
  float temp;

  if (LT_PWM1 == light_type) {
    Settings.light_color[0] = 255;    // One PWM channel only supports Dimmer but needs max color
  }
  float dimmer = 100 / (float)myDimmer;
  for (uint8_t i = 0; i < light_subtype; i++) {
    if (Settings.flag.light_signal) {
      temp = (float)light_signal_color[i] / dimmer;
    } else {
      temp = (float)Settings.light_color[i] / dimmer;
    }
    light_current_color[i] = (uint8_t)temp;
  }
}

void AcDimmerPowerOn(void)
{
  if (Settings.light_dimmer && !(light_power)) {
    ExecuteCommandPower(light_device, POWER_ON, SRC_LIGHT);
  }
}

void AcDimmerState(uint8_t append)
{
  char scommand[33];

  if (append) {
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s,"), mqtt_data);
  } else {
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("{"));
  }
  GetPowerDevice(scommand, light_device, sizeof(scommand), Settings.flag.device_index_enable);
  snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s\"%s\":\"%s\",\"" D_CMND_DIMMER "\":%d"),
    mqtt_data, scommand, GetStateText(light_power), Settings.light_dimmer);
  if ((LST_COLDWARM == light_subtype) || (LST_RGBWC == light_subtype)) {
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s,\"" D_CMND_COLORTEMPERATURE "\":%d"), mqtt_data, AcDimmerGetColorTemp());
  }
  if (append) {
    if (light_subtype >= LST_RGB) {
      snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s,\"" D_CMND_SCHEME "\":%d"), mqtt_data, Settings.light_scheme);
    }
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s,\"" D_CMND_FADE "\":\"%s\",\"" D_CMND_SPEED "\":%d,\"" D_CMND_LEDTABLE "\":\"%s\""),
      mqtt_data, GetStateText(Settings.light_fade), Settings.light_speed, GetStateText(Settings.light_correction));
  } else {
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s}"), mqtt_data);
  }
}

void AcDimmerPreparePower(void)
{
  if (Settings.light_dimmer && !(light_power)) {
    if (!Settings.flag.not_power_linked) {
      ExecuteCommandPower(light_device, POWER_ON_NO_STATE, SRC_LIGHT);
    }
  }
  else if (!Settings.light_dimmer && light_power) {
    ExecuteCommandPower(light_device, POWER_OFF_NO_STATE, SRC_LIGHT);
  }
#ifdef USE_DOMOTICZ
  DomoticzUpdatePowerState(light_device);
#endif  // USE_DOMOTICZ
  if (Settings.flag3.hass_tele_on_power) {
    mqtt_data[0] = '\0';
    MqttShowState();
    MqttPublishPrefixTopic_P(TELE, PSTR(D_RSLT_STATE), MQTT_TELE_RETAIN);
  }

  AcDimmerState(0);
}

void AcDimmerSetPower(void)
{
//  light_power = XdrvMailbox.index;
  light_old_power = light_power;
  light_power = bitRead(XdrvMailbox.index, light_device -1);
  if (light_wakeup_active) {
    light_wakeup_active--;
  }
  if (light_power && !light_old_power) {
    light_update = 1;
  }
  AcDimmerAnimate();
}

void AcDimmerAnimate(void)
{
  uint8_t cur_col[5];
  uint16_t light_still_on = 0;

  strip_timer_counter++;
  if (!light_power) {                   // Power Off
    sleep = Settings.sleep;
    strip_timer_counter = 0;
    for (uint8_t i = 0; i < light_subtype; i++) {
      light_still_on += light_new_color[i];
    }
    if (light_still_on && Settings.light_fade && (Settings.light_scheme < LS_MAX)) {
      uint8_t speed = Settings.light_speed;
      if (speed > 6) {
        speed = 6;
      }
      for (uint8_t i = 0; i < light_subtype; i++) {
        if (light_new_color[i] > 0) {
          light_new_color[i] -= (light_new_color[i] >> speed) +1;
        }
      }
    } else {
      for (uint8_t i = 0; i < light_subtype; i++) {
        light_new_color[i] = 0;
      }
    }
  }
  else {
    sleep = 0;
    switch (Settings.light_scheme) {
      case LS_POWER:
        AcDimmerSetDimmer(Settings.light_dimmer);
        AcDimmerFade();
        break;
      case LS_WAKEUP:
        if (2 == light_wakeup_active) {
          light_wakeup_active = 1;
          for (uint8_t i = 0; i < light_subtype; i++) {
            light_new_color[i] = 0;
          }
          light_wakeup_counter = 0;
          light_wakeup_dimmer = 0;
        }
        light_wakeup_counter++;
        if (light_wakeup_counter > ((Settings.light_wakeup * STATES) / Settings.light_dimmer)) {
          light_wakeup_counter = 0;
          light_wakeup_dimmer++;
          if (light_wakeup_dimmer <= Settings.light_dimmer) {
            AcDimmerSetDimmer(light_wakeup_dimmer);
            for (uint8_t i = 0; i < light_subtype; i++) {
              light_new_color[i] = light_current_color[i];
            }
          } else {
            snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("{\"" D_CMND_WAKEUP "\":\"" D_JSON_DONE "\"}"));
            MqttPublishPrefixTopic_P(TELE, PSTR(D_CMND_WAKEUP));
            light_wakeup_active = 0;
            Settings.light_scheme = LS_POWER;
          }
        }
        break;
    }
  }

  if ((Settings.light_scheme < LS_MAX) || !light_power) {
    if (memcmp(light_last_color, light_new_color, light_subtype)) {
        light_update = 1;
    }
    if (light_update) {
      light_update = 0;
      for (uint8_t i = 0; i < light_subtype; i++) {
        light_last_color[i] = light_new_color[i];
        cur_col[i] = light_last_color[i]*Settings.rgbwwTable[i]/255;
        cur_col[i] = (Settings.light_correction) ? ledTable[cur_col[i]] : cur_col[i];
      }

      // color remapping
      uint8_t orig_col[5];
      memcpy(orig_col, cur_col, sizeof(orig_col));
      for (uint8_t i = 0; i < 5; i++) {
        cur_col[i] = orig_col[light_color_remap[i]];
      }

      for (uint8_t i = 0; i < light_subtype; i++) {
        if (light_type < LT_PWM6) {
          if (pin[GPIO_PWM1 +i] < 99) {
            if (cur_col[i] > 0xFC) {
              cur_col[i] = 0xFC;   // Fix unwanted blinking and PWM watchdog errors for values close to pwm_range (H801, Arilux and BN-SZ01)
            }
            uint16_t curcol = cur_col[i] * (Settings.pwm_range / 255);
            curcol = map(curcol, 8, 1008, 0, 1023);
//            AddLog_P2(LOG_LEVEL_DEBUG, PSTR(D_LOG_APPLICATION "Cur_Col%d %d, CurCol %d"), i, cur_col[i], curcol);
            PWM_VALUES_TAB[i][3] = 1;
            PWM_VALUES_TAB[i][0] = pin[GPIO_PWM1 +i];
            PWM_VALUES_TAB[i][1] = (bitRead(pwm_inverted, i) ? Settings.pwm_range - curcol : curcol);
          }
        }
      }

      char *tmp_data = XdrvMailbox.data;
      uint16_t tmp_data_len = XdrvMailbox.data_len;

      XdrvMailbox.data = (char*)cur_col;
      XdrvMailbox.data_len = sizeof(cur_col);
      if (XdrvCall(FUNC_SET_CHANNELS)) {
        // Serviced
      }
      XdrvMailbox.data = tmp_data;
      XdrvMailbox.data_len = tmp_data_len;
    }
  }
}

bool AcDimmerCommand(void)
{
  char command [CMDSZ];
  bool serviced = true;
  bool coldim = false;
  bool valid_entry = false;
  char scolor[25];
  char option = (1 == XdrvMailbox.data_len) ? XdrvMailbox.data[0] : '\0';

  int command_code = GetCommandCode(command, sizeof(command), XdrvMailbox.topic, kAcDimmerCommands);
  if (-1 == command_code) {
    serviced = false;  // Unknown command
  }
  else if (CMND_WAKEUP == command_code) {
    if ((XdrvMailbox.payload >= 0) && (XdrvMailbox.payload <= 100)) {
      Settings.light_dimmer = XdrvMailbox.payload;
    }
    light_wakeup_active = 3;
    Settings.light_scheme = LS_WAKEUP;
    AcDimmerPowerOn();
    snprintf_P(mqtt_data, sizeof(mqtt_data), S_JSON_COMMAND_SVALUE, command, D_JSON_STARTED);
  }
  else if (CMND_DIMMER == command_code) {
    if ('+' == option) {
      XdrvMailbox.payload = (Settings.light_dimmer > 89) ? 100 : Settings.light_dimmer + 10;
    }
    else if ('-' == option) {
      XdrvMailbox.payload = (Settings.light_dimmer < 11) ? 1 : Settings.light_dimmer - 10;
    }
    if ((XdrvMailbox.payload >= 0) && (XdrvMailbox.payload <= 100)) {
      Settings.light_dimmer = XdrvMailbox.payload;
      light_update = 1;
      coldim = true;
    } else {
      snprintf_P(mqtt_data, sizeof(mqtt_data), S_JSON_COMMAND_NVALUE, command, Settings.light_dimmer);
    }
  }
  else if (CMND_LEDTABLE == command_code) {
    if ((XdrvMailbox.payload >= 0) && (XdrvMailbox.payload <= 2)) {
      switch (XdrvMailbox.payload) {
      case 0: // Off
      case 1: // On
        Settings.light_correction = XdrvMailbox.payload;
        break;
      case 2: // Toggle
        Settings.light_correction ^= 1;
        break;
      }
      light_update = 1;
    }
    snprintf_P(mqtt_data, sizeof(mqtt_data), S_JSON_COMMAND_SVALUE, command, GetStateText(Settings.light_correction));
  }
  else if (CMND_FADE == command_code) {
    switch (XdrvMailbox.payload) {
    case 0: // Off
    case 1: // On
      Settings.light_fade = XdrvMailbox.payload;
      break;
    case 2: // Toggle
      Settings.light_fade ^= 1;
      break;
    }
    snprintf_P(mqtt_data, sizeof(mqtt_data), S_JSON_COMMAND_SVALUE, command, GetStateText(Settings.light_fade));
  }
  else if (CMND_WAKEUPDURATION == command_code) {
    if ((XdrvMailbox.payload > 0) && (XdrvMailbox.payload < 3001)) {
      Settings.light_wakeup = XdrvMailbox.payload;
      light_wakeup_active = 0;
    }
    snprintf_P(mqtt_data, sizeof(mqtt_data), S_JSON_COMMAND_NVALUE, command, Settings.light_wakeup);
  }
  else if (CMND_UNDOCA == command_code) {  // Theos legacy status
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s,%d,%d,%d,%d,%d"),
      scolor, Settings.light_fade, Settings.light_correction, Settings.light_scheme, Settings.light_speed, Settings.light_width);
    MqttPublishPrefixTopic_P(STAT, XdrvMailbox.topic);
    mqtt_data[0] = '\0';
  }
  else {
    serviced = false;  // Unknown command
  }

  if (coldim) {
    AcDimmerPreparePower();
  }

  return serviced;
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

bool Xdrv91(uint8_t function)
{
  bool result = false;

  if (light_type) {
    switch (function) {
      case FUNC_PRE_INIT:
        AcDimmerInit();
        break;
      case FUNC_EVERY_50_MSECOND:
        AcDimmerAnimate();
        break;
      case FUNC_COMMAND:
        result = AcDimmerCommand();
        break;
      case FUNC_SET_POWER:
        AcDimmerSetPower();
        break;
    }
  }
  return result;
}

#endif
