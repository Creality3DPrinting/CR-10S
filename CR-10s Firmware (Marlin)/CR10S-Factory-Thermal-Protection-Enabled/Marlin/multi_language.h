#ifndef MULTI_LANGUAGE_H
#define MULTI_LANGUAGE_H

static unsigned char currentfontnameindex = 0; // 0:en, 1:cn, 2:kana
static unsigned char tall_font_correction = 0;

#define FONT_EEPROM_OFFSET 99
static void set_select_font_index0_to_eeprom () {
  eeprom_write_byte((unsigned char*)FONT_EEPROM_OFFSET, (unsigned char)0);
}
static void set_select_font_index1_to_eeprom () {
  eeprom_write_byte((unsigned char*)FONT_EEPROM_OFFSET, (unsigned char)1);
}
static void set_select_font_index2_to_eeprom () {
  eeprom_write_byte((unsigned char*)FONT_EEPROM_OFFSET, (unsigned char)2);
}

static void(* reboot) (void) = 0;

static void lcd_select_lang() {
  currentfontnameindex = eeprom_read_byte((unsigned char*)FONT_EEPROM_OFFSET);
  if (currentfontnameindex == 1) {
    tall_font_correction = 1;
  }
}

static void set_fontname(char font_nr) {
  #if ENABLED(USE_BIG_EDIT_FONT)
  if (font_nr == FONT_MENU_EDIT) {
    u8g.setFont(FONT_MENU_EDIT_NAME);
  }
  else {
    switch (currentfontnameindex) {
      case 0:
        u8g.setFont(FONT_MENU_NAME_EN);
        break;
      case 1:
        u8g.setFont(FONT_MENU_NAME_CN);
        break;
      case 2:
        u8g.setFont(FONT_MENU_NAME_KANA);
        break;
      default:
        u8g.setFont(FONT_MENU_NAME_EN);
        break;
    }
  }
  #else
  switch (currentfontnameindex) {
    case 0:
      u8g.setFont(FONT_MENU_NAME_EN);
      break;
    case 1:
      u8g.setFont(FONT_MENU_NAME_CN);
      break;
    case 2:
      u8g.setFont(FONT_MENU_NAME_KANA);
      break;
    default:
      u8g.setFont(FONT_MENU_NAME_EN);
      break;
  }
  #endif
}

/* MSG_SD_INSERTED */
static const char* msg_sd_inserted() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_SD_INSERTED);
    case 1:
      return PSTR(CN_MSG_SD_INSERTED);
    case 2:
      return PSTR(KANA_MSG_SD_INSERTED);    
  }
  return PSTR(MSG_SD_INSERTED);
}

/* MSG_SD_REMOVED */
static const char* msg_sd_removed() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_SD_REMOVED);
    case 1:
      return PSTR(CN_MSG_SD_REMOVED);
    case 2:
      return PSTR(KANA_MSG_SD_REMOVED);    
  }
  return PSTR(MSG_SD_REMOVED);
}

/* MSG_MAIN */
static const char* msg_main() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MAIN);
    case 1:
      return PSTR(CN_MSG_MAIN);
    case 2:
      return PSTR(KANA_MSG_MAIN);    
  }
  return PSTR(MSG_MAIN);
}
/* MSG_AUTOSTART */
static const char* msg_autostart() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_AUTOSTART);
    case 1:
      return PSTR(CN_MSG_AUTOSTART);
    case 2:
      return PSTR(KANA_MSG_AUTOSTART);    
  }
  return PSTR(MSG_AUTOSTART);
}
/* MSG_DISABLE_STEPPERS */
static const char* msg_disable_steppers() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_DISABLE_STEPPERS);
    case 1:
      return PSTR(CN_MSG_DISABLE_STEPPERS);
    case 2:
      return PSTR(KANA_MSG_DISABLE_STEPPERS);    
  }
  return PSTR(MSG_DISABLE_STEPPERS);
}
/* MSG_AUTO_HOME */
static const char* msg_auto_home() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_AUTO_HOME);
    case 1:
      return PSTR(CN_MSG_AUTO_HOME);
    case 2:
      return PSTR(KANA_MSG_AUTO_HOME);    
  }
  return PSTR(MSG_AUTO_HOME);
}
/* MSG_SET_HOME_OFFSETS */
static const char* msg_set_home_offsets() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_SET_HOME_OFFSETS);
    case 1:
      return PSTR(CN_MSG_SET_HOME_OFFSETS);
    case 2:
      return PSTR(KANA_MSG_SET_HOME_OFFSETS);    
  }
  return PSTR(MSG_SET_HOME_OFFSETS);
}
/* MSG_SET_ORIGIN */
static const char* msg_set_origin() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_SET_ORIGIN);
    case 1:
      return PSTR(CN_MSG_SET_ORIGIN);
    case 2:
      return PSTR(KANA_MSG_SET_ORIGIN);    
  }
  return PSTR(MSG_SET_ORIGIN);
}
/* MSG_PREHEAT_PLA */
static const char* msg_preheat_pla() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PREHEAT_PLA);
    case 1:
      return PSTR(CN_MSG_PREHEAT_PLA);
    case 2:
      return PSTR(KANA_MSG_PREHEAT_PLA);    
  }
  return PSTR(MSG_PREHEAT_PLA);
}
/* MSG_PREHEAT_PLA_N */
static const char* msg_preheat_pla_n() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PREHEAT_PLA_N);
    case 1:
      return PSTR(CN_MSG_PREHEAT_PLA_N);
    case 2:
      return PSTR(KANA_MSG_PREHEAT_PLA_N);    
  }
  return PSTR(MSG_PREHEAT_PLA_N);
}
/* MSG_PREHEAT_PLA_ALL */
static const char* msg_preheat_pla_all() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PREHEAT_PLA_ALL);
    case 1:
      return PSTR(CN_MSG_PREHEAT_PLA_ALL);
    case 2:
      return PSTR(KANA_MSG_PREHEAT_PLA_ALL);    
  }
  return PSTR(MSG_PREHEAT_PLA_ALL);
}
/* MSG_PREHEAT_PLA_BEDONLY */
static const char* msg_preheat_pla_bedonly() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PREHEAT_PLA_BEDONLY);
    case 1:
      return PSTR(CN_MSG_PREHEAT_PLA_BEDONLY);
    case 2:
      return PSTR(KANA_MSG_PREHEAT_PLA_BEDONLY);    
  }
  return PSTR(MSG_PREHEAT_PLA_BEDONLY);
}
/* MSG_PREHEAT_PLA_SETTINGS */
static const char* msg_preheat_pla_settings() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PREHEAT_PLA_SETTINGS);
    case 1:
      return PSTR(CN_MSG_PREHEAT_PLA_SETTINGS);
    case 2:
      return PSTR(KANA_MSG_PREHEAT_PLA_SETTINGS);    
  }
  return PSTR(MSG_PREHEAT_PLA_SETTINGS);
}
/* MSG_PREHEAT_ABS */
static const char* msg_preheat_abs() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PREHEAT_ABS);
    case 1:
      return PSTR(CN_MSG_PREHEAT_ABS);
    case 2:
      return PSTR(KANA_MSG_PREHEAT_ABS);    
  }
  return PSTR(MSG_PREHEAT_ABS);
}
/* MSG_PREHEAT_ABS_N */
static const char* msg_preheat_abs_n() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PREHEAT_ABS_N);
    case 1:
      return PSTR(CN_MSG_PREHEAT_ABS_N);
    case 2:
      return PSTR(KANA_MSG_PREHEAT_ABS_N);    
  }
  return PSTR(MSG_PREHEAT_ABS_N);
}
/* MSG_PREHEAT_ABS_ALL */
static const char* msg_preheat_abs_all() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PREHEAT_ABS_ALL);
    case 1:
      return PSTR(CN_MSG_PREHEAT_ABS_ALL);
    case 2:
      return PSTR(KANA_MSG_PREHEAT_ABS_ALL);    
  }
  return PSTR(MSG_PREHEAT_ABS_ALL);
}
/* MSG_PREHEAT_ABS_BEDONLY */
static const char* msg_preheat_abs_bedonly() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PREHEAT_ABS_BEDONLY);
    case 1:
      return PSTR(CN_MSG_PREHEAT_ABS_BEDONLY);
    case 2:
      return PSTR(KANA_MSG_PREHEAT_ABS_BEDONLY);    
  }
  return PSTR(MSG_PREHEAT_ABS_BEDONLY);
}
/* MSG_PREHEAT_ABS_SETTINGS */
static const char* msg_preheat_abs_settings() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PREHEAT_ABS_SETTINGS);
    case 1:
      return PSTR(CN_MSG_PREHEAT_ABS_SETTINGS);
    case 2:
      return PSTR(KANA_MSG_PREHEAT_ABS_SETTINGS);    
  }
  return PSTR(MSG_PREHEAT_ABS_SETTINGS);
}
/* MSG_NOZZLE_H1 */
static const char* msg_nozzle_h1() {
 switch (currentfontnameindex) {
   case 0:
     return PSTR(MSG_NOZZLE MSG_H1);
   case 1:
     return PSTR(MSG_NOZZLE MSG_H1);
   case 2:
     return PSTR(MSG_NOZZLE MSG_H1);    
 }
  return PSTR(MSG_NOZZLE MSG_H1);
}
/* MSG_NOZZLE_H2 */
static const char* msg_nozzle_h2() {
 switch (currentfontnameindex) {
   case 0:
     return PSTR(MSG_NOZZLE MSG_H2);
   case 1:
     return PSTR(MSG_NOZZLE MSG_H2);
   case 2:
     return PSTR(MSG_NOZZLE MSG_H2);    
 }
  return PSTR(MSG_NOZZLE MSG_H2);
}
/* MSG_NOZZLE_H3 */
static const char* msg_nozzle_h3() {
 switch (currentfontnameindex) {
   case 0:
     return PSTR(MSG_NOZZLE MSG_H3);
   case 1:
     return PSTR(MSG_NOZZLE MSG_H3);
   case 2:
     return PSTR(MSG_NOZZLE MSG_H3);    
 }
  return PSTR(MSG_NOZZLE MSG_H3);
}
/* MSG_NOZZLE_H4 */
static const char* msg_nozzle_h4() {
 switch (currentfontnameindex) {
   case 0:
     return PSTR(MSG_NOZZLE MSG_H4);
   case 1:
     return PSTR(MSG_NOZZLE MSG_H4);
   case 2:
     return PSTR(MSG_NOZZLE MSG_H4);    
 }
  return PSTR(MSG_NOZZLE MSG_H4);
}
/* MSG_PREHEAT_PLA_N_H1 */
static const char* msg_preheat_pla_n_h1() {
 switch (currentfontnameindex) {
   case 0:
     return PSTR(MSG_PREHEAT_PLA_N MSG_H1);
   case 1:
     return PSTR(MSG_PREHEAT_PLA_N MSG_H1);
   case 2:
     return PSTR(MSG_PREHEAT_PLA_N MSG_H1);    
 }
  return PSTR(MSG_PREHEAT_PLA_N MSG_H1);
}
/* MSG_PREHEAT_PLA_N_H2 */
static const char* msg_preheat_pla_n_h2() {
 switch (currentfontnameindex) {
   case 0:
     return PSTR(MSG_PREHEAT_PLA_N MSG_H2);
   case 1:
     return PSTR(MSG_PREHEAT_PLA_N MSG_H2);
   case 2:
     return PSTR(MSG_PREHEAT_PLA_N MSG_H2);    
 }
  return PSTR(MSG_PREHEAT_PLA_N MSG_H2);
}
/* MSG_PREHEAT_PLA_N_H3 */
static const char* msg_preheat_pla_n_h3() {
 switch (currentfontnameindex) {
   case 0:
     return PSTR(MSG_PREHEAT_PLA_N MSG_H3);
   case 1:
     return PSTR(MSG_PREHEAT_PLA_N MSG_H3);
   case 2:
     return PSTR(MSG_PREHEAT_PLA_N MSG_H3);    
 }
  return PSTR(MSG_PREHEAT_PLA_N MSG_H3);
}
/* MSG_PREHEAT_PLA_N_H4 */
static const char* msg_preheat_pla_n_h4() {
 switch (currentfontnameindex) {
   case 0:
     return PSTR(MSG_PREHEAT_PLA_N MSG_H4);
   case 1:
     return PSTR(MSG_PREHEAT_PLA_N MSG_H4);
   case 2:
     return PSTR(MSG_PREHEAT_PLA_N MSG_H4);    
 }
  return PSTR(MSG_PREHEAT_PLA_N MSG_H4);
}
/* MSG_PREHEAT_ABS_N_H1 */
static const char* msg_preheat_abs_n_h1() {
 switch (currentfontnameindex) {
   case 0:
     return PSTR(MSG_PREHEAT_ABS_N MSG_H1);
   case 1:
     return PSTR(MSG_PREHEAT_ABS_N MSG_H1);
   case 2:
     return PSTR(MSG_PREHEAT_ABS_N MSG_H1);    
 }
  return PSTR(MSG_PREHEAT_ABS_N MSG_H1);
}
/* MSG_PREHEAT_ABS_N_H2 */
static const char* msg_preheat_abs_n_h2() {
 switch (currentfontnameindex) {
   case 0:
     return PSTR(MSG_PREHEAT_ABS_N MSG_H2);
   case 1:
     return PSTR(MSG_PREHEAT_ABS_N MSG_H2);
   case 2:
     return PSTR(MSG_PREHEAT_ABS_N MSG_H2);    
 }
  return PSTR(MSG_PREHEAT_ABS_N MSG_H2);
}
/* MSG_PREHEAT_ABS_N_H3 */
static const char* msg_preheat_abs_n_h3() {
 switch (currentfontnameindex) {
   case 0:
     return PSTR(MSG_PREHEAT_ABS_N MSG_H3);
   case 1:
     return PSTR(MSG_PREHEAT_ABS_N MSG_H3);
   case 2:
     return PSTR(MSG_PREHEAT_ABS_N MSG_H3);    
 }
  return PSTR(MSG_PREHEAT_ABS_N MSG_H3);
}
/* MSG_PREHEAT_ABS_N_H4 */
static const char* msg_preheat_abs_n_h4() {
 switch (currentfontnameindex) {
   case 0:
     return PSTR(MSG_PREHEAT_ABS_N MSG_H4);
   case 1:
     return PSTR(MSG_PREHEAT_ABS_N MSG_H4);
   case 2:
     return PSTR(MSG_PREHEAT_ABS_N MSG_H4);    
 }
  return PSTR(MSG_PREHEAT_ABS_N MSG_H4);
}
/* MSG_COOLDOWN */
static const char* msg_cooldown() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_COOLDOWN);
    case 1:
      return PSTR(CN_MSG_COOLDOWN);
    case 2:
      return PSTR(KANA_MSG_COOLDOWN);    
  }
  return PSTR(MSG_COOLDOWN);
}
/* MSG_SWITCH_PS_ON */
static const char* msg_switch_ps_on() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_SWITCH_PS_ON);
    case 1:
      return PSTR(CN_MSG_SWITCH_PS_ON);
    case 2:
      return PSTR(KANA_MSG_SWITCH_PS_ON);    
  }
  return PSTR(MSG_SWITCH_PS_ON);
}
/* MSG_SWITCH_PS_OFF */
static const char* msg_switch_ps_off() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_SWITCH_PS_OFF);
    case 1:
      return PSTR(CN_MSG_SWITCH_PS_OFF);
    case 2:
      return PSTR(KANA_MSG_SWITCH_PS_OFF);    
  }
  return PSTR(MSG_SWITCH_PS_OFF);
}
/* MSG_EXTRUDE */
static const char* msg_extrude() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_EXTRUDE);
    case 1:
      return PSTR(CN_MSG_EXTRUDE);
    case 2:
      return PSTR(KANA_MSG_EXTRUDE);    
  }
  return PSTR(MSG_EXTRUDE);
}
/* MSG_RETRACT */
static const char* msg_retract() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_RETRACT);
    case 1:
      return PSTR(CN_MSG_RETRACT);
    case 2:
      return PSTR(KANA_MSG_RETRACT);    
  }
  return PSTR(MSG_RETRACT);
}
/* MSG_MOVE_AXIS */
static const char* msg_move_axis() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOVE_AXIS);
    case 1:
      return PSTR(CN_MSG_MOVE_AXIS);
    case 2:
      return PSTR(KANA_MSG_MOVE_AXIS);    
  }
  return PSTR(MSG_MOVE_AXIS);
}
/* MSG_LEVEL_BED */
static const char* msg_level_bed() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_LEVEL_BED);
    case 1:
      return PSTR(CN_MSG_LEVEL_BED);
    case 2:
      return PSTR(MSG_LEVEL_BED);    
  }
  return PSTR(MSG_LEVEL_BED);
}
/* MSG_MOVE_X */
static const char* msg_move_x() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOVE_X);
    case 1:
      return PSTR(CN_MSG_MOVE_X);
    case 2:
      return PSTR(KANA_MSG_MOVE_X);    
  }
  return PSTR(MSG_MOVE_X);
}
/* MSG_MOVE_Y */
static const char* msg_move_y() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOVE_Y);
    case 1:
      return PSTR(CN_MSG_MOVE_Y);
    case 2:
      return PSTR(KANA_MSG_MOVE_Y);    
  }
  return PSTR(MSG_MOVE_Y);
}
/* MSG_MOVE_Z */
static const char* msg_move_z() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOVE_Z);
    case 1:
      return PSTR(CN_MSG_MOVE_Z);
    case 2:
      return PSTR(KANA_MSG_MOVE_Z);    
  }
  return PSTR(MSG_MOVE_Z);
}
/* MSG_MOVE_E */
static const char* msg_move_e() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOVE_E);
    case 1:
      return PSTR(CN_MSG_MOVE_E);
    case 2:
      return PSTR(KANA_MSG_MOVE_E);    
  }
  return PSTR(MSG_MOVE_E);
}
/* MSG_MOVE_E_MOVE_E1 */
static const char* msg_move_e_move_e1() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOVE_E MSG_MOVE_E1);
    case 1:
      return PSTR(CN_MSG_MOVE_E MSG_MOVE_E1);
    case 2:
      return PSTR(KANA_MSG_MOVE_E MSG_MOVE_E1);    
  }
  return PSTR(MSG_MOVE_E MSG_MOVE_E1);
}
/* MSG_MOVE_E_MOVE_E2 */
static const char* msg_move_e_move_e2() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOVE_E MSG_MOVE_E2);
    case 1:
      return PSTR(CN_MSG_MOVE_E MSG_MOVE_E2);
    case 2:
      return PSTR(KANA_MSG_MOVE_E MSG_MOVE_E2);    
  }
  return PSTR(MSG_MOVE_E MSG_MOVE_E2);
}
/* MSG_MOVE_E_MOVE_E3 */
static const char* msg_move_e_move_e3() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOVE_E MSG_MOVE_E3);
    case 1:
      return PSTR(CN_MSG_MOVE_E MSG_MOVE_E3);
    case 2:
      return PSTR(KANA_MSG_MOVE_E MSG_MOVE_E3);    
  }
  return PSTR(MSG_MOVE_E MSG_MOVE_E3);
}
/* MSG_MOVE_E_MOVE_E4 */
static const char* msg_move_e_move_e4() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOVE_E MSG_MOVE_E4);
    case 1:
      return PSTR(CN_MSG_MOVE_E MSG_MOVE_E4);
    case 2:
      return PSTR(KANA_MSG_MOVE_E MSG_MOVE_E4);    
  }
  return PSTR(MSG_MOVE_E MSG_MOVE_E4);
}
/* MSG_MOVE_01MM */
static const char* msg_move_01mm() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOVE_01MM);
    case 1:
      return PSTR(CN_MSG_MOVE_01MM);
    case 2:
      return PSTR(KANA_MSG_MOVE_01MM);    
  }
  return PSTR(MSG_MOVE_01MM);
}
/* MSG_MOVE_1MM */
static const char* msg_move_1mm() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOVE_1MM);
    case 1:
      return PSTR(CN_MSG_MOVE_1MM);
    case 2:
      return PSTR(KANA_MSG_MOVE_1MM);    
  }
  return PSTR(MSG_MOVE_1MM);
}
/* MSG_MOVE_10MM */
static const char* msg_move_10mm() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOVE_10MM);
    case 1:
      return PSTR(CN_MSG_MOVE_10MM);
    case 2:
      return PSTR(KANA_MSG_MOVE_10MM);    
  }
  return PSTR(MSG_MOVE_10MM);
}
/* MSG_SPEED */
static const char* msg_speed() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_SPEED);
    case 1:
      return PSTR(CN_MSG_SPEED);
    case 2:
      return PSTR(KANA_MSG_SPEED);    
  }
  return PSTR(MSG_SPEED);
}
/* MSG_NOZZLE */
static const char* msg_nozzle() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_NOZZLE);
    case 1:
      return PSTR(CN_MSG_NOZZLE);
    case 2:
      return PSTR(KANA_MSG_NOZZLE);    
  }
  return PSTR(MSG_NOZZLE);
}
/* MSG_NOZZLE_N1 */
static const char* msg_nozzle_n1() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_NOZZLE MSG_N1);
    case 1:
      return PSTR(CN_MSG_NOZZLE MSG_N1);
    case 2:
      return PSTR(KANA_MSG_NOZZLE MSG_N1);    
  }
  return PSTR(MSG_NOZZLE MSG_N1);
}
/* MSG_NOZZLE_N2 */
static const char* msg_nozzle_n2() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_NOZZLE MSG_N2);
    case 1:
      return PSTR(CN_MSG_NOZZLE MSG_N2);
    case 2:
      return PSTR(KANA_MSG_NOZZLE MSG_N2);    
  }
  return PSTR(MSG_NOZZLE MSG_N2);
}
/* MSG_NOZZLE_N3 */
static const char* msg_nozzle_n3() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_NOZZLE MSG_N3);
    case 1:
      return PSTR(CN_MSG_NOZZLE MSG_N3);
    case 2:
      return PSTR(KANA_MSG_NOZZLE MSG_N3);    
  }
  return PSTR(MSG_NOZZLE MSG_N3);
}
/* MSG_NOZZLE_N4 */
static const char* msg_nozzle_n4() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_NOZZLE MSG_N4);
    case 1:
      return PSTR(CN_MSG_NOZZLE MSG_N4);
    case 2:
      return PSTR(KANA_MSG_NOZZLE MSG_N4);    
  }
  return PSTR(MSG_NOZZLE MSG_N4);
}
/* MSG_BED */
static const char* msg_bed() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_BED);
    case 1:
      return PSTR(CN_MSG_BED);
    case 2:
      return PSTR(KANA_MSG_BED);    
  }
  return PSTR(MSG_BED);
}
/* MSG_FAN_SPEED */
static const char* msg_fan_speed() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FAN_SPEED);
    case 1:
      return PSTR(CN_MSG_FAN_SPEED);
    case 2:
      return PSTR(KANA_MSG_FAN_SPEED);    
  }
  return PSTR(MSG_FAN_SPEED);
}
/* MSG_FLOW */
static const char* msg_flow() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FLOW);
    case 1:
      return PSTR(CN_MSG_FLOW);
    case 2:
      return PSTR(KANA_MSG_FLOW);    
  }
  return PSTR(MSG_FLOW);
}
/* MSG_FLOW_N1 */
static const char* msg_flow_n1() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FLOW MSG_N1);
    case 1:
      return PSTR(CN_MSG_FLOW MSG_N1);
    case 2:
      return PSTR(KANA_MSG_FLOW MSG_N1);    
  }
  return PSTR(MSG_FLOW MSG_N1);
}
/* MSG_FLOW_N2 */
static const char* msg_flow_n2() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FLOW MSG_N2);
    case 1:
      return PSTR(CN_MSG_FLOW MSG_N2);
    case 2:
      return PSTR(KANA_MSG_FLOW MSG_N2);    
  }
  return PSTR(MSG_FLOW MSG_N2);
}
/* MSG_FLOW_N3 */
static const char* msg_flow_n3() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FLOW MSG_N3);
    case 1:
      return PSTR(CN_MSG_FLOW MSG_N3);
    case 2:
      return PSTR(KANA_MSG_FLOW MSG_N3);    
  }
  return PSTR(MSG_FLOW MSG_N3);
}
/* MSG_FLOW_N4 */
static const char* msg_flow_n4() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FLOW MSG_N4);
    case 1:
      return PSTR(CN_MSG_FLOW MSG_N4);
    case 2:
      return PSTR(KANA_MSG_FLOW MSG_N4);    
  }
  return PSTR(MSG_FLOW MSG_N4);
}
/* MSG_CONTROL */
static const char* msg_control() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_CONTROL);
    case 1:
      return PSTR(CN_MSG_CONTROL);
    case 2:
      return PSTR(KANA_MSG_CONTROL);    
  }
  return PSTR(MSG_CONTROL);
}
/* MSG_MIN */
static const char* msg_min() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MIN);
    case 1:
      return PSTR(CN_MSG_MIN);
    case 2:
      return PSTR(KANA_MSG_MIN);    
  }
  return PSTR(MSG_MIN);
}
/* MSG_MAX */
static const char* msg_max() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MAX);
    case 1:
      return PSTR(CN_MSG_MAX);
    case 2:
      return PSTR(KANA_MSG_MAX);    
  }
  return PSTR(MSG_MAX);
}
/* MSG_FACTOR */
static const char* msg_factor() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FACTOR);
    case 1:
      return PSTR(CN_MSG_FACTOR);
    case 2:
      return PSTR(KANA_MSG_FACTOR);    
  }
  return PSTR(MSG_FACTOR);
}
/* MSG_AUTOTEMP */
static const char* msg_autotemp() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_AUTOTEMP);
    case 1:
      return PSTR(CN_MSG_AUTOTEMP);
    case 2:
      return PSTR(KANA_MSG_AUTOTEMP);    
  }
  return PSTR(MSG_AUTOTEMP);
}
/* MSG_ON */
static const char* msg_on() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ON);
    case 1:
      return PSTR(CN_MSG_ON);
    case 2:
      return PSTR(KANA_MSG_ON);    
  }
  return PSTR(MSG_ON);
}
/* MSG_OFF */
static const char* msg_off() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_OFF);
    case 1:
      return PSTR(CN_MSG_OFF);
    case 2:
      return PSTR(KANA_MSG_OFF);    
  }
  return PSTR(MSG_OFF);
}
/* MSG_PID_P */
static const char* msg_pid_p() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PID_P);
    case 1:
      return PSTR(CN_MSG_PID_P);
    case 2:
      return PSTR(KANA_MSG_PID_P);    
  }
  return PSTR(MSG_PID_P);
}
/* MSG_PID_I */
static const char* msg_pid_i() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PID_I);
    case 1:
      return PSTR(CN_MSG_PID_I);
    case 2:
      return PSTR(KANA_MSG_PID_I);    
  }
  return PSTR(MSG_PID_I);
}
/* MSG_PID_D */
static const char* msg_pid_d() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PID_D);
    case 1:
      return PSTR(CN_MSG_PID_D);
    case 2:
      return PSTR(KANA_MSG_PID_D);    
  }
  return PSTR(MSG_PID_D);
}
/* MSG_PID_C */
static const char* msg_pid_c() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PID_C);
    case 1:
      return PSTR(CN_MSG_PID_C);
    case 2:
      return PSTR(KANA_MSG_PID_C);    
  }
  return PSTR(MSG_PID_C);
}
/* MSG_E1 */
static const char* msg_e1() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_E1);
    case 1:
      return PSTR(MSG_E1);
    case 2:
      return PSTR(MSG_E1);    
  }
  return PSTR(MSG_E1);
}
/* MSG_E2 */
static const char* msg_e2() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_E2);
    case 1:
      return PSTR(MSG_E2);
    case 2:
      return PSTR(MSG_E2);    
  }
  return PSTR(MSG_E2);
}
/* MSG_E3 */
static const char* msg_e3() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_E3);
    case 1:
      return PSTR(MSG_E3);
    case 2:
      return PSTR(MSG_E3);    
  }
  return PSTR(MSG_E3);
}
/* MSG_E4 */
static const char* msg_e4() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_E4);
    case 1:
      return PSTR(MSG_E4);
    case 2:
      return PSTR(MSG_E4);    
  }
  return PSTR(MSG_E4);
}
/* MSG_ACC */
static const char* msg_acc() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ACC);
    case 1:
      return PSTR(CN_MSG_ACC);
    case 2:
      return PSTR(KANA_MSG_ACC);    
  }
  return PSTR(MSG_ACC);
}
/* MSG_VXY_JERK */
static const char* msg_vxy_jerk() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_VXY_JERK);
    case 1:
      return PSTR(CN_MSG_VXY_JERK);
    case 2:
      return PSTR(KANA_MSG_VXY_JERK);    
  }
  return PSTR(MSG_VXY_JERK);
}
/* MSG_VZ_JERK */
static const char* msg_vz_jerk() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_VZ_JERK);
    case 1:
      return PSTR(CN_MSG_VZ_JERK);
    case 2:
      return PSTR(KANA_MSG_VZ_JERK);    
  }
  return PSTR(MSG_VZ_JERK);
}
/* MSG_VE_JERK */
static const char* msg_ve_jerk() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_VE_JERK);
    case 1:
      return PSTR(CN_MSG_VE_JERK);
    case 2:
      return PSTR(KANA_MSG_VE_JERK);    
  }
  return PSTR(MSG_VE_JERK);
}
/* MSG_VMAX */
static const char* msg_vmax() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_VMAX);
    case 1:
      return PSTR(CN_MSG_VMAX);
    case 2:
      return PSTR(KANA_MSG_VMAX);    
  }
  return PSTR(MSG_VMAX);
}
/* MSG_VMAX_X */
static const char* msg_vmax_x() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_VMAX MSG_X);
    case 1:
      return PSTR(CN_MSG_VMAX CN_MSG_X);
    case 2:
      return PSTR(KANA_MSG_VMAX KANA_MSG_X);    
  }
  return PSTR(MSG_VMAX MSG_X);
}
/* MSG_VMAX_Y */
static const char* msg_vmax_y() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_VMAX MSG_Y);
    case 1:
      return PSTR(CN_MSG_VMAX CN_MSG_Y);
    case 2:
      return PSTR(KANA_MSG_VMAX KANA_MSG_Y);    
  }
  return PSTR(MSG_VMAX MSG_Y);
}
/* MSG_VMAX_Z */
static const char* msg_vmax_z() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_VMAX MSG_Z);
    case 1:
      return PSTR(CN_MSG_VMAX CN_MSG_Z);
    case 2:
      return PSTR(KANA_MSG_VMAX KANA_MSG_Z);    
  }
  return PSTR(MSG_VMAX MSG_Z);
}
/* MSG_VMAX_E */
static const char* msg_vmax_e() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_VMAX MSG_E);
    case 1:
      return PSTR(CN_MSG_VMAX CN_MSG_E);
    case 2:
      return PSTR(KANA_MSG_VMAX KANA_MSG_E);    
  }
  return PSTR(MSG_VMAX MSG_E);
}
/* MSG_VMIN */
static const char* msg_vmin() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_VMIN);
    case 1:
      return PSTR(CN_MSG_VMIN);
    case 2:
      return PSTR(KANA_MSG_VMIN);    
  }
  return PSTR(MSG_VMIN);
}
/* MSG_VTRAV_MIN */
static const char* msg_vtrav_min() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_VTRAV_MIN);
    case 1:
      return PSTR(CN_MSG_VTRAV_MIN);
    case 2:
      return PSTR(KANA_MSG_VTRAV_MIN);    
  }
  return PSTR(MSG_VTRAV_MIN);
}
/* MSG_AMAX */
static const char* msg_amax() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_AMAX);
    case 1:
      return PSTR(CN_MSG_AMAX);
    case 2:
      return PSTR(KANA_MSG_AMAX);    
  }
  return PSTR(MSG_AMAX);
}
/* MSG_AMAX_X */
static const char* msg_amax_x() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_AMAX MSG_X);
    case 1:
      return PSTR(CN_MSG_AMAX CN_MSG_X);
    case 2:
      return PSTR(KANA_MSG_AMAX KANA_MSG_X);    
  }
  return PSTR(MSG_AMAX MSG_X);
}
/* MSG_AMAX_Y */
static const char* msg_amax_y() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_AMAX MSG_Y);
    case 1:
      return PSTR(CN_MSG_AMAX CN_MSG_Y);
    case 2:
      return PSTR(KANA_MSG_AMAX KANA_MSG_Y);    
  }
  return PSTR(MSG_AMAX MSG_Y);
}
/* MSG_AMAX_Z */
static const char* msg_amax_z() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_AMAX MSG_Z);
    case 1:
      return PSTR(CN_MSG_AMAX CN_MSG_Z);
    case 2:
      return PSTR(KANA_MSG_AMAX KANA_MSG_Z);    
  }
  return PSTR(MSG_AMAX MSG_Z);
}
/* MSG_AMAX_E */
static const char* msg_amax_e() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_AMAX MSG_E);
    case 1:
      return PSTR(CN_MSG_AMAX CN_MSG_E);
    case 2:
      return PSTR(KANA_MSG_AMAX KANA_MSG_E);    
  }
  return PSTR(MSG_AMAX MSG_E);
}
/* MSG_A_RETRACT */
static const char* msg_a_retract() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_A_RETRACT);
    case 1:
      return PSTR(CN_MSG_A_RETRACT);
    case 2:
      return PSTR(KANA_MSG_A_RETRACT);    
  }
  return PSTR(MSG_A_RETRACT);
}
/* MSG_A_TRAVEL */
static const char* msg_a_travel() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_A_TRAVEL);
    case 1:
      return PSTR(CN_MSG_A_TRAVEL);
    case 2:
      return PSTR(MSG_A_TRAVEL);    
  }
  return PSTR(MSG_A_TRAVEL);
}
/* MSG_XSTEPS */
static const char* msg_xsteps() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_XSTEPS);
    case 1:
      return PSTR(CN_MSG_XSTEPS);
    case 2:
      return PSTR(KANA_MSG_XSTEPS);    
  }
  return PSTR(MSG_XSTEPS);
}
/* MSG_YSTEPS */
static const char* msg_ysteps() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_YSTEPS);
    case 1:
      return PSTR(CN_MSG_YSTEPS);
    case 2:
      return PSTR(KANA_MSG_YSTEPS);    
  }
  return PSTR(MSG_YSTEPS);
}
/* MSG_ZSTEPS */
static const char* msg_zsteps() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ZSTEPS);
    case 1:
      return PSTR(CN_MSG_ZSTEPS);
    case 2:
      return PSTR(KANA_MSG_ZSTEPS);    
  }
  return PSTR(MSG_ZSTEPS);
}
/* MSG_ESTEPS */
static const char* msg_esteps() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ESTEPS);
    case 1:
      return PSTR(CN_MSG_ESTEPS);
    case 2:
      return PSTR(KANA_MSG_ESTEPS);    
  }
  return PSTR(MSG_ESTEPS);
}
/* MSG_TEMPERATURE */
static const char* msg_temperature() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_TEMPERATURE);
    case 1:
      return PSTR(CN_MSG_TEMPERATURE);
    case 2:
      return PSTR(KANA_MSG_TEMPERATURE);    
  }
  return PSTR(MSG_TEMPERATURE);
}
/* MSG_MOTION */
static const char* msg_motion() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_MOTION);
    case 1:
      return PSTR(CN_MSG_MOTION);
    case 2:
      return PSTR(KANA_MSG_MOTION);    
  }
  return PSTR(MSG_MOTION);
}
/* MSG_VOLUMETRIC */
static const char* msg_volumetric() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_VOLUMETRIC);
    case 1:
      return PSTR(CN_MSG_VOLUMETRIC);
    case 2:
      return PSTR(KANA_MSG_VOLUMETRIC);    
  }
  return PSTR(MSG_VOLUMETRIC);
}
/* MSG_VOLUMETRIC_ENABLED */
static const char* msg_volumetric_enabled() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_VOLUMETRIC_ENABLED);
    case 1:
      return PSTR(CN_MSG_VOLUMETRIC_ENABLED);
    case 2:
      return PSTR(KANA_MSG_VOLUMETRIC_ENABLED);    
  }
  return PSTR(MSG_VOLUMETRIC_ENABLED);
}
/* MSG_FILAMENT_DIAM */
static const char* msg_filament_diam() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FILAMENT_DIAM);
    case 1:
      return PSTR(CN_MSG_FILAMENT_DIAM);
    case 2:
      return PSTR(KANA_MSG_FILAMENT_DIAM);    
  }
  return PSTR(MSG_FILAMENT_DIAM);
}
/* MSG_FILAMENT_DIAM_DIAM_E1 */
static const char* MSG_FILAMENT_DIAM_DIAM_E1() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FILAMENT_DIAM MSG_DIAM_E1);
    case 1:
      return PSTR(CN_MSG_FILAMENT_DIAM MSG_DIAM_E1);
    case 2:
      return PSTR(KANA_MSG_FILAMENT_DIAM MSG_DIAM_E1);    
  }
  return PSTR(MSG_FILAMENT_DIAM MSG_DIAM_E1);
}
/* MSG_FILAMENT_DIAM_DIAM_E2 */
static const char* MSG_FILAMENT_DIAM_DIAM_E2() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FILAMENT_DIAM MSG_DIAM_E2);
    case 1:
      return PSTR(CN_MSG_FILAMENT_DIAM MSG_DIAM_E2);
    case 2:
      return PSTR(KANA_MSG_FILAMENT_DIAM MSG_DIAM_E2);    
  }
  return PSTR(MSG_FILAMENT_DIAM MSG_DIAM_E2);
}
/* MSG_FILAMENT_DIAM_DIAM_E3 */
static const char* MSG_FILAMENT_DIAM_DIAM_E3() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FILAMENT_DIAM MSG_DIAM_E3);
    case 1:
      return PSTR(CN_MSG_FILAMENT_DIAM MSG_DIAM_E3);
    case 2:
      return PSTR(KANA_MSG_FILAMENT_DIAM MSG_DIAM_E3);    
  }
  return PSTR(MSG_FILAMENT_DIAM MSG_DIAM_E3);
}
/* MSG_FILAMENT_DIAM_DIAM_E4 */
static const char* MSG_FILAMENT_DIAM_DIAM_E4() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FILAMENT_DIAM MSG_DIAM_E4);
    case 1:
      return PSTR(CN_MSG_FILAMENT_DIAM MSG_DIAM_E4);
    case 2:
      return PSTR(KANA_MSG_FILAMENT_DIAM MSG_DIAM_E4);    
  }
  return PSTR(MSG_FILAMENT_DIAM MSG_DIAM_E4);
}
/* MSG_CONTRAST */
static const char* msg_contrast() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_CONTRAST);
    case 1:
      return PSTR(CN_MSG_CONTRAST);
    case 2:
      return PSTR(KANA_MSG_CONTRAST);    
  }
  return PSTR(MSG_CONTRAST);
}
/* MSG_STORE_EPROM */
static const char* msg_store_eprom() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_STORE_EPROM);
    case 1:
      return PSTR(CN_MSG_STORE_EPROM);
    case 2:
      return PSTR(KANA_MSG_STORE_EPROM);    
  }
  return PSTR(MSG_STORE_EPROM);
}
/* MSG_LOAD_EPROM */
static const char* msg_load_eprom() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_LOAD_EPROM);
    case 1:
      return PSTR(CN_MSG_LOAD_EPROM);
    case 2:
      return PSTR(KANA_MSG_LOAD_EPROM);    
  }
  return PSTR(MSG_LOAD_EPROM);
}
/* MSG_RESTORE_FAILSAFE */
static const char* msg_restore_failsafe() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_RESTORE_FAILSAFE);
    case 1:
      return PSTR(CN_MSG_RESTORE_FAILSAFE);
    case 2:
      return PSTR(KANA_MSG_RESTORE_FAILSAFE);    
  }
  return PSTR(MSG_RESTORE_FAILSAFE);
}
/* MSG_SELECT_LANGUAGE */
static const char* msg_select_language() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_SELECT_LANGUAGE);
    case 1:
      return PSTR(CN_MSG_SELECT_LANGUAGE);
    case 2:
      return PSTR(KANA_MSG_SELECT_LANGUAGE);    
  }
  return PSTR(MSG_SELECT_LANGUAGE);
}
/* MSG_ENGLISH_LANGUAGE */
static const char* msg_english_lang() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ENGLISH_LANGUAGE);
    case 1:
      return PSTR(CN_MSG_ENGLISH_LANGUAGE);
    case 2:
      return PSTR(KANA_MSG_ENGLISH_LANGUAGE);    
  }
  return PSTR(MSG_ENGLISH_LANGUAGE);
}
/* MSG_CHINESE_LANGUAGE */
static const char* msg_chinese_lang() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_CHINESE_LANGUAGE);
    case 1:
      return PSTR(CN_MSG_CHINESE_LANGUAGE);
    case 2:
      return PSTR(KANA_MSG_CHINESE_LANGUAGE);    
  }
  return PSTR(MSG_CHINESE_LANGUAGE);
}
/* MSG_JAPANESE_LANGUAGE */
static const char* msg_japanese_lang() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_JAPANESE_LANGUAGE);
    case 1:
      return PSTR(CN_MSG_JAPANESE_LANGUAGE);
    case 2:
      return PSTR(KANA_MSG_JAPANESE_LANGUAGE);    
  }
  return PSTR(MSG_JAPANESE_LANGUAGE);
}
/* MSG_REFRESH */
static const char* msg_refresh() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_REFRESH);
    case 1:
      return PSTR(CN_MSG_REFRESH);
    case 2:
      return PSTR(KANA_MSG_REFRESH);    
  }
  return PSTR(MSG_REFRESH);
}
/* MSG_WATCH */
static const char* msg_watch() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_WATCH);
    case 1:
      return PSTR(CN_MSG_WATCH);
    case 2:
      return PSTR(KANA_MSG_WATCH);    
  }
  return PSTR(MSG_WATCH);
}
/* MSG_PREPARE */
static const char* msg_prepare() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PREPARE);
    case 1:
      return PSTR(CN_MSG_PREPARE);
    case 2:
      return PSTR(KANA_MSG_PREPARE);    
  }
  return PSTR(MSG_PREPARE);
}
/* MSG_TUNE */
static const char* msg_tune() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_TUNE);
    case 1:
      return PSTR(CN_MSG_TUNE);
    case 2:
      return PSTR(KANA_MSG_TUNE);    
  }
  return PSTR(MSG_TUNE);
}
/* MSG_PAUSE_PRINT */
static const char* msg_pause_print() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PAUSE_PRINT);
    case 1:
      return PSTR(CN_MSG_PAUSE_PRINT);
    case 2:
      return PSTR(KANA_MSG_PAUSE_PRINT);    
  }
  return PSTR(MSG_PAUSE_PRINT);
}
/* MSG_RESUME_PRINT */
static const char* msg_resume_print() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_RESUME_PRINT);
    case 1:
      return PSTR(CN_MSG_RESUME_PRINT);
    case 2:
      return PSTR(KANA_MSG_RESUME_PRINT);    
  }
  return PSTR(MSG_RESUME_PRINT);
}
/* MSG_STOP_PRINT */
static const char* msg_stop_print() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_STOP_PRINT);
    case 1:
      return PSTR(CN_MSG_STOP_PRINT);
    case 2:
      return PSTR(KANA_MSG_STOP_PRINT);    
  }
  return PSTR(MSG_STOP_PRINT);
}
/* MSG_CARD_MENU */
static const char* msg_card_menu() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_CARD_MENU);
    case 1:
      return PSTR(CN_MSG_CARD_MENU);
    case 2:
      return PSTR(KANA_MSG_CARD_MENU);    
  }
  return PSTR(MSG_CARD_MENU);
}
/* MSG_NO_CARD */
static const char* msg_no_card() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_NO_CARD);
    case 1:
      return PSTR(CN_MSG_NO_CARD);
    case 2:
      return PSTR(KANA_MSG_NO_CARD);    
  }
  return PSTR(MSG_NO_CARD);
}
/* MSG_DWELL */
static const char* msg_dwell() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_DWELL);
    case 1:
      return PSTR(CN_MSG_DWELL);
    case 2:
      return PSTR(KANA_MSG_DWELL);    
  }
  return PSTR(MSG_DWELL);
}
/* MSG_USERWAIT */
static const char* msg_userwait() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_USERWAIT);
    case 1:
      return PSTR(CN_MSG_USERWAIT);
    case 2:
      return PSTR(KANA_MSG_USERWAIT);    
  }
  return PSTR(MSG_USERWAIT);
}
/* MSG_RESUMING */
static const char* msg_resuming() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_RESUMING);
    case 1:
      return PSTR(CN_MSG_RESUMING);
    case 2:
      return PSTR(KANA_MSG_RESUMING);    
  }
  return PSTR(MSG_RESUMING);
}
/* MSG_PRINT_ABORTED */
static const char* msg_print_aborted() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_PRINT_ABORTED);
    case 1:
      return PSTR(CN_MSG_PRINT_ABORTED);
    case 2:
      return PSTR(KANA_MSG_PRINT_ABORTED);    
  }
  return PSTR(MSG_PRINT_ABORTED);
}
/* MSG_NO_MOVE */
static const char* msg_no_move() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_NO_MOVE);
    case 1:
      return PSTR(CN_MSG_NO_MOVE);
    case 2:
      return PSTR(KANA_MSG_NO_MOVE);    
  }
  return PSTR(MSG_NO_MOVE);
}
/* MSG_KILLED */
static const char* msg_killed() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_KILLED);
    case 1:
      return PSTR(CN_MSG_KILLED);
    case 2:
      return PSTR(KANA_MSG_KILLED);    
  }
  return PSTR(MSG_KILLED);
}
/* MSG_STOPPED */
static const char* msg_stopped() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_STOPPED);
    case 1:
      return PSTR(CN_MSG_STOPPED);
    case 2:
      return PSTR(KANA_MSG_STOPPED);    
  }
  return PSTR(MSG_STOPPED);
}
/* MSG_CONTROL_RETRACT */
static const char* msg_control_retract() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_CONTROL_RETRACT);
    case 1:
      return PSTR(CN_MSG_CONTROL_RETRACT);
    case 2:
      return PSTR(KANA_MSG_CONTROL_RETRACT);    
  }
  return PSTR(MSG_CONTROL_RETRACT);
}
/* MSG_CONTROL_RETRACT_SWAP */
static const char* msg_control_retract_swap() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_CONTROL_RETRACT_SWAP);
    case 1:
      return PSTR(CN_MSG_CONTROL_RETRACT_SWAP);
    case 2:
      return PSTR(KANA_MSG_CONTROL_RETRACT_SWAP);    
  }
  return PSTR(MSG_CONTROL_RETRACT_SWAP);
}
/* MSG_CONTROL_RETRACTF */
static const char* msg_control_retractf() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_CONTROL_RETRACTF);
    case 1:
      return PSTR(CN_MSG_CONTROL_RETRACTF);
    case 2:
      return PSTR(KANA_MSG_CONTROL_RETRACTF);    
  }
  return PSTR(MSG_CONTROL_RETRACTF);
}
/* MSG_CONTROL_RETRACT_ZLIFT */
static const char* msg_control_retract_zlift() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_CONTROL_RETRACT_ZLIFT);
    case 1:
      return PSTR(CN_MSG_CONTROL_RETRACT_ZLIFT);
    case 2:
      return PSTR(KANA_MSG_CONTROL_RETRACT_ZLIFT);    
  }
  return PSTR(MSG_CONTROL_RETRACT_ZLIFT);
}
/* MSG_CONTROL_RETRACT_RECOVER */
static const char* msg_control_retract_recover() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_CONTROL_RETRACT_RECOVER);
    case 1:
      return PSTR(CN_MSG_CONTROL_RETRACT_RECOVER);
    case 2:
      return PSTR(KANA_MSG_CONTROL_RETRACT_RECOVER);    
  }
  return PSTR(MSG_CONTROL_RETRACT_RECOVER);
}
/* MSG_CONTROL_RETRACT_RECOVER_SWAP */
static const char* msg_control_retract_recover_swap() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_CONTROL_RETRACT_RECOVER_SWAP);
    case 1:
      return PSTR(CN_MSG_CONTROL_RETRACT_RECOVER_SWAP);
    case 2:
      return PSTR(KANA_MSG_CONTROL_RETRACT_RECOVER_SWAP);    
  }
  return PSTR(MSG_CONTROL_RETRACT_RECOVER_SWAP);
}
/* MSG_CONTROL_RETRACT_RECOVERF */
static const char* msg_control_retract_recoverf() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_CONTROL_RETRACT_RECOVERF);
    case 1:
      return PSTR(CN_MSG_CONTROL_RETRACT_RECOVERF);
    case 2:
      return PSTR(KANA_MSG_CONTROL_RETRACT_RECOVERF);    
  }
  return PSTR(MSG_CONTROL_RETRACT_RECOVERF);
}
/* MSG_AUTORETRACT */
static const char* msg_autoretract() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_AUTORETRACT);
    case 1:
      return PSTR(CN_MSG_AUTORETRACT);
    case 2:
      return PSTR(KANA_MSG_AUTORETRACT);    
  }
  return PSTR(MSG_AUTORETRACT);
}
/* MSG_FILAMENTCHANGE */
static const char* msg_filamentchange() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_FILAMENTCHANGE);
    case 1:
      return PSTR(CN_MSG_FILAMENTCHANGE);
    case 2:
      return PSTR(KANA_MSG_FILAMENTCHANGE);    
  }
  return PSTR(MSG_FILAMENTCHANGE);
}
/* MSG_LOAD */
static const char* msg_load() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_LOAD);
    case 1:
      return PSTR(CN_MSG_LOAD);
    case 2:
      return PSTR(MSG_LOAD);    
  }
  return PSTR(MSG_LOAD);
}
/* MSG_UNLOAD */
static const char* msg_unload() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_UNLOAD);
    case 1:
      return PSTR(CN_MSG_UNLOAD);
    case 2:
      return PSTR(MSG_UNLOAD);    
  }
  return PSTR(MSG_UNLOAD);
}
/* MSG_INIT_SDCARD */
static const char* msg_init_sdcard() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_INIT_SDCARD);
    case 1:
      return PSTR(CN_MSG_INIT_SDCARD);
    case 2:
      return PSTR(KANA_MSG_INIT_SDCARD);    
  }
  return PSTR(MSG_INIT_SDCARD);
}
/* MSG_CNG_SDCARD */
static const char* msg_cng_sdcard() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_CNG_SDCARD);
    case 1:
      return PSTR(CN_MSG_CNG_SDCARD);
    case 2:
      return PSTR(KANA_MSG_CNG_SDCARD);    
  }
  return PSTR(MSG_CNG_SDCARD);
}
/* MSG_ZPROBE_OUT */
static const char* msg_zprobe_out() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ZPROBE_OUT);
    case 1:
      return PSTR(CN_MSG_ZPROBE_OUT);
    case 2:
      return PSTR(KANA_MSG_ZPROBE_OUT);    
  }
  return PSTR(MSG_ZPROBE_OUT);
}
/* MSG_POSITION_UNKNOWN */
static const char* msg_position_unknown() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_POSITION_UNKNOWN);
    case 1:
      return PSTR(CN_MSG_POSITION_UNKNOWN);
    case 2:
      return PSTR(KANA_MSG_POSITION_UNKNOWN);    
  }
  return PSTR(MSG_POSITION_UNKNOWN);
}
/* MSG_LED */
static const char* msg_led() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_LED);
    case 1:
      return PSTR(CN_MSG_LED);
    case 2:
      return PSTR(MSG_LED);    
  }
  return PSTR(MSG_LED);
}
/* MSG_TIAOPING */
static const char* msg_tiaoping() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_TIAOPING);
    case 1:
      return PSTR(CN_MSG_TIAOPING);
    case 2:
      return PSTR(MSG_TIAOPING);    
  }
  return PSTR(MSG_TIAOPING);
}
/* MSG_LED_ON */
static const char* msg_led_on() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_LED_ON);
    case 1:
      return PSTR(CN_MSG_LED_ON);
    case 2:
      return PSTR(MSG_LED_ON);    
  }
  return PSTR(MSG_LED_ON);
}
/* MSG_LED_OFF */
static const char* msg_led_off() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_LED_OFF);
    case 1:
      return PSTR(CN_MSG_LED_OFF);
    case 2:
      return PSTR(MSG_LED_OFF);    
  }
  return PSTR(MSG_LED_OFF);
}
/* MSG_NEXT */
static const char* msg_next() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_NEXT);
    case 1:
      return PSTR(CN_MSG_NEXT);
    case 2:
      return PSTR(MSG_NEXT);    
  }
  return PSTR(MSG_NEXT);
}
/* MSG_ZPROBE_ZOFFSET */
static const char* msg_zprobe_zoffset() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ZPROBE_ZOFFSET);
    case 1:
      return PSTR(CN_MSG_ZPROBE_ZOFFSET);
    case 2:
      return PSTR(KANA_MSG_ZPROBE_ZOFFSET);    
  }
  return PSTR(MSG_ZPROBE_ZOFFSET);
}
/* MSG_BABYSTEP_X */
static const char* msg_babystep_x() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_BABYSTEP_X);
    case 1:
      return PSTR(CN_MSG_BABYSTEP_X);
    case 2:
      return PSTR(KANA_MSG_BABYSTEP_X);    
  }
  return PSTR(MSG_BABYSTEP_X);
}
/* MSG_BABYSTEP_Y */
static const char* msg_babystep_y() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_BABYSTEP_Y);
    case 1:
      return PSTR(CN_MSG_BABYSTEP_Y);
    case 2:
      return PSTR(KANA_MSG_BABYSTEP_Y);    
  }
  return PSTR(MSG_BABYSTEP_Y);
}
/* MSG_BABYSTEP_Z */
static const char* msg_babystep_z() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_BABYSTEP_Z);
    case 1:
      return PSTR(CN_MSG_BABYSTEP_Z);
    case 2:
      return PSTR(KANA_MSG_BABYSTEP_Z);    
  }
  return PSTR(MSG_BABYSTEP_Z);
}
/* MSG_ENDSTOP_ABORT */
static const char* msg_endstop_abort() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ENDSTOP_ABORT);
    case 1:
      return PSTR(CN_MSG_ENDSTOP_ABORT);
    case 2:
      return PSTR(KANA_MSG_ENDSTOP_ABORT);    
  }
  return PSTR(MSG_ENDSTOP_ABORT);
}
/* MSG_HEATING_FAILED_LCD */
static const char* msg_heating_failed_lcd() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_HEATING_FAILED_LCD);
    case 1:
      return PSTR(CN_MSG_HEATING_FAILED_LCD);
    case 2:
      return PSTR(MSG_HEATING_FAILED_LCD);    
  }
  return PSTR(MSG_HEATING_FAILED_LCD);
}
/* MSG_ERR_REDUNDANT_TEMP */
static const char* msg_err_redundant_temp() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ERR_REDUNDANT_TEMP);
    case 1:
      return PSTR(CN_MSG_ERR_REDUNDANT_TEMP);
    case 2:
      return PSTR(MSG_ERR_REDUNDANT_TEMP);    
  }
  return PSTR(MSG_ERR_REDUNDANT_TEMP);
}
/* MSG_THERMAL_RUNAWAY */
static const char* msg_thermal_runaway() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_THERMAL_RUNAWAY);
    case 1:
      return PSTR(CN_MSG_THERMAL_RUNAWAY);
    case 2:
      return PSTR(MSG_THERMAL_RUNAWAY);    
  }
  return PSTR(MSG_THERMAL_RUNAWAY);
}
/* MSG_ERR_MAXTEMP */
static const char* msg_err_maxtemp() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ERR_MAXTEMP);
    case 1:
      return PSTR(CN_MSG_ERR_MAXTEMP);
    case 2:
      return PSTR(MSG_ERR_MAXTEMP);    
  }
  return PSTR(MSG_ERR_MAXTEMP);
}
/* MSG_ERR_MINTEMP */
static const char* msg_err_mintemp() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ERR_MINTEMP);
    case 1:
      return PSTR(CN_MSG_ERR_MINTEMP);
    case 2:
      return PSTR(MSG_ERR_MINTEMP);    
  }
  return PSTR(MSG_ERR_MINTEMP);
}
/* MSG_ERR_MAXTEMP_BED */
static const char* msg_err_maxtemp_bed() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ERR_MAXTEMP_BED);
    case 1:
      return PSTR(CN_MSG_ERR_MAXTEMP_BED);
    case 2:
      return PSTR(MSG_ERR_MAXTEMP_BED);    
  }
  return PSTR(MSG_ERR_MAXTEMP_BED);
}
/* MSG_ERR_MINTEMP_BED */
static const char* msg_err_mintemp_bed() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_ERR_MINTEMP_BED);
    case 1:
      return PSTR(MSG_ERR_MINTEMP_BED);
    case 2:
      return PSTR(MSG_ERR_MINTEMP_BED);    
  }
  return PSTR(MSG_ERR_MINTEMP_BED);
}
/* MSG_END_HOUR */
static const char* msg_end_hour() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_END_HOUR);
    case 1:
      return PSTR(CN_MSG_END_HOUR);
    case 2:
      return PSTR(KANA_MSG_END_HOUR);    
  }
  return PSTR(MSG_END_HOUR);
}
/* MSG_END_MINUTE */
static const char* msg_end_minute() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_END_MINUTE);
    case 1:
      return PSTR(CN_MSG_END_MINUTE);
    case 2:
      return PSTR(KANA_MSG_END_MINUTE);    
  }
  return PSTR(MSG_END_MINUTE);
}
/* MSG_HEATING */
static const char* msg_heating() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_HEATING);
    case 1:
      return PSTR(MSG_HEATING);
    case 2:
      return PSTR(MSG_HEATING);    
  }
  return PSTR(MSG_HEATING);
}
/* MSG_HEATING_COMPLETE */
static const char* msg_heating_complete() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_HEATING_COMPLETE);
    case 1:
      return PSTR(MSG_HEATING_COMPLETE);
    case 2:
      return PSTR(MSG_HEATING_COMPLETE);    
  }
  return PSTR(MSG_HEATING_COMPLETE);
}
/* MSG_BED_HEATING */
static const char* msg_bed_heating() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_BED_HEATING);
    case 1:
      return PSTR(MSG_BED_HEATING);
    case 2:
      return PSTR(MSG_BED_HEATING);    
  }
  return PSTR(MSG_BED_HEATING);
}
/* MSG_BED_DONE */
static const char* msg_bed_done() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_BED_DONE);
    case 1:
      return PSTR(MSG_BED_DONE);
    case 2:
      return PSTR(MSG_BED_DONE);    
  }
  return PSTR(MSG_BED_DONE);
}
/* MSG_DELTA_CALIBRATE */
static const char* msg_delta_calibrate() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_DELTA_CALIBRATE);
    case 1:
      return PSTR(MSG_DELTA_CALIBRATE);
    case 2:
      return PSTR(MSG_DELTA_CALIBRATE);    
  }
  return PSTR(MSG_DELTA_CALIBRATE);
}
/* MSG_DELTA_CALIBRATE_X */
static const char* msg_delta_calibrate_x() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_DELTA_CALIBRATE_X);
    case 1:
      return PSTR(MSG_DELTA_CALIBRATE_X);
    case 2:
      return PSTR(MSG_DELTA_CALIBRATE_X);    
  }
  return PSTR(MSG_DELTA_CALIBRATE_X);
}
/* MSG_DELTA_CALIBRATE_Y */
static const char* msg_delta_calibrate_y() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_DELTA_CALIBRATE_Y);
    case 1:
      return PSTR(MSG_DELTA_CALIBRATE_Y);
    case 2:
      return PSTR(MSG_DELTA_CALIBRATE_Y);    
  }
  return PSTR(MSG_DELTA_CALIBRATE_Y);
}
/* MSG_DELTA_CALIBRATE_Z */
static const char* msg_delta_calibrate_z() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_DELTA_CALIBRATE_Z);
    case 1:
      return PSTR(MSG_DELTA_CALIBRATE_Z);
    case 2:
      return PSTR(MSG_DELTA_CALIBRATE_Z);    
  }
  return PSTR(MSG_DELTA_CALIBRATE_Z);
}
/* MSG_DELTA_CALIBRATE_CENTER */
static const char* msg_delta_calibrate_center() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_DELTA_CALIBRATE_CENTER);
    case 1:
      return PSTR(MSG_DELTA_CALIBRATE_CENTER);
    case 2:
      return PSTR(MSG_DELTA_CALIBRATE_CENTER);    
  }
  return PSTR(MSG_DELTA_CALIBRATE_CENTER);
}

/* MSG_RESUME_PRINT_OK */
static const char* msg_resume_print_ok() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_RESUME_PRINT_OK);
    case 1:
      return PSTR(CN_MSG_RESUME_PRINT_OK);
    case 2:
      return PSTR(KANA_MSG_RESUME_PRINT_OK);    
  }
  return PSTR(MSG_RESUME_PRINT_OK);
}
/* MSG_RESUME_PRINT_Y */
static const char* msg_resume_print_y() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_RESUME_PRINT_Y);
    case 1:
      return PSTR(CN_MSG_RESUME_PRINT_Y);
    case 2:
      return PSTR(KANA_MSG_RESUME_PRINT_Y);    
  }
  return PSTR(MSG_RESUME_PRINT_Y);
}
/* MSG_RESUME_PRINT_N */
static const char* msg_resume_print_n() {
  switch (currentfontnameindex) {
    case 0:
      return PSTR(MSG_RESUME_PRINT_N);
    case 1:
      return PSTR(CN_MSG_RESUME_PRINT_N);
    case 2:
      return PSTR(KANA_MSG_RESUME_PRINT_N);    
  }
  return PSTR(MSG_RESUME_PRINT_N);
}

#endif //__LANGUAGE_H
