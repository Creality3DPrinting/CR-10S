#include "ultralcd.h"
#if ENABLED(ULTRA_LCD)
#include "Marlin.h"
#include "language.h"
#include "cardreader.h"
#include "temperature.h"
#include "stepper.h"
#include "configuration_store.h"

int8_t encoderDiff; // updated from interrupt context and added to encoderPosition every LCD update

bool encoderRateMultiplierEnabled;
int32_t lastEncoderMovementMillis;

int plaPreheatHotendTemp;
int plaPreheatHPBTemp;
int plaPreheatFanSpeed;

int absPreheatHotendTemp;
int absPreheatHPBTemp;
int absPreheatFanSpeed;

#if ENABLED(FILAMENT_LCD_DISPLAY)
  millis_t previous_lcd_status_ms = 0;
#endif

// Function pointer to menu functions.
typedef void (*menuFunc_t)();

uint8_t lcd_status_message_level;
char lcd_status_message[3 * LCD_WIDTH + 1] = WELCOME_MSG; // worst case is kana with up to 3*LCD_WIDTH+1

#if ENABLED(DOGLCD)
  #include "dogm_lcd_implementation.h"
#else
  #include "ultralcd_implementation_hitachi_HD44780.h"
#endif

// The main status screen
static void lcd_status_screen();

#if ENABLED(ULTIPANEL)

  #if HAS_POWER_SWITCH
    extern bool powersupply;
  #endif
  const float manual_feedrate[] = MANUAL_FEEDRATE;
  static void lcd_main_menu();
  static void lcd_resume_ok_menu();
  static void lcd_tune_menu();
  static void lcd_prepare_menu();
  static void lcd_move_menu();
  static void lcd_control_menu();
  static void lcd_select_language_menu();
  static void lcd_control_temperature_menu();
  static void lcd_control_temperature_preheat_pla_settings_menu();
  static void lcd_control_temperature_preheat_abs_settings_menu();
  static void lcd_control_motion_menu();
  static void lcd_control_volumetric_menu();
  //��ʾ�˵�
  static void lcd_filament_change0();
  static void lcd_filament_change1();
  static void led();
  static void led_on();
  static void led_off();
  static void tiaoping();
  static void next();
  static void reseted();

  #if ENABLED(HAS_LCD_CONTRAST)
    static void lcd_set_contrast();
  #endif

  #if ENABLED(FWRETRACT)
    static void lcd_control_retract_menu();
  #endif

  #if ENABLED(DELTA_CALIBRATION_MENU)
    static void lcd_delta_calibrate_menu();
  #endif

  #if ENABLED(MANUAL_BED_LEVELING)
    #include "mesh_bed_leveling.h"
    static void _lcd_level_bed();
    static void _lcd_level_bed_homing();
    static void lcd_level_bed();
  #endif

  /* Different types of actions that can be used in menu items. */
  static void menu_action_back(menuFunc_t data);
  static void menu_action_submenu(menuFunc_t data);
  static void menu_action_gcode(const char* pgcode);
  static void menu_action_function(menuFunc_t data);
  static void menu_action_setting_edit_bool(const char* pstr, bool* ptr);
  static void menu_action_setting_edit_int3(const char* pstr, int* ptr, int minValue, int maxValue);
  static void menu_action_setting_edit_float3(const char* pstr, float* ptr, float minValue, float maxValue);
  static void menu_action_setting_edit_float32(const char* pstr, float* ptr, float minValue, float maxValue);
  static void menu_action_setting_edit_float43(const char* pstr, float* ptr, float minValue, float maxValue);
  static void menu_action_setting_edit_float5(const char* pstr, float* ptr, float minValue, float maxValue);
  static void menu_action_setting_edit_float51(const char* pstr, float* ptr, float minValue, float maxValue);
  static void menu_action_setting_edit_float52(const char* pstr, float* ptr, float minValue, float maxValue);
  static void menu_action_setting_edit_long5(const char* pstr, unsigned long* ptr, unsigned long minValue, unsigned long maxValue);
  static void menu_action_setting_edit_callback_bool(const char* pstr, bool* ptr, menuFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_int3(const char* pstr, int* ptr, int minValue, int maxValue, menuFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_float3(const char* pstr, float* ptr, float minValue, float maxValue, menuFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_float32(const char* pstr, float* ptr, float minValue, float maxValue, menuFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_float43(const char* pstr, float* ptr, float minValue, float maxValue, menuFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_float5(const char* pstr, float* ptr, float minValue, float maxValue, menuFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_float51(const char* pstr, float* ptr, float minValue, float maxValue, menuFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_float52(const char* pstr, float* ptr, float minValue, float maxValue, menuFunc_t callbackFunc);
  static void menu_action_setting_edit_callback_long5(const char* pstr, unsigned long* ptr, unsigned long minValue, unsigned long maxValue, menuFunc_t callbackFunc);

  #if ENABLED(SDSUPPORT)
    static void lcd_sdcard_menu();
    static void menu_action_sdfile(const char* filename, char* longFilename);
    static void menu_action_sddirectory(const char* filename, char* longFilename);
  #endif

  #define ENCODER_FEEDRATE_DEADZONE 10

  #if DISABLED(LCD_I2C_VIKI)
    #ifndef ENCODER_STEPS_PER_MENU_ITEM
      #define ENCODER_STEPS_PER_MENU_ITEM 5
    #endif
    #ifndef ENCODER_PULSES_PER_STEP
      #define ENCODER_PULSES_PER_STEP 1
    #endif
  #else
    #ifndef ENCODER_STEPS_PER_MENU_ITEM
      #define ENCODER_STEPS_PER_MENU_ITEM 2 // VIKI LCD rotary encoder uses a different number of steps per rotation
    #endif
    #ifndef ENCODER_PULSES_PER_STEP
      #define ENCODER_PULSES_PER_STEP 1
    #endif
  #endif


  /* Helper macros for menus */

  /**
   * START_MENU generates the init code for a menu function
   */
  #define START_MENU() do { \
    encoderRateMultiplierEnabled = false; \
    if (encoderPosition > 0x8000) encoderPosition = 0; \
    uint8_t encoderLine = encoderPosition / ENCODER_STEPS_PER_MENU_ITEM; \
    if (encoderLine < currentMenuViewOffset) currentMenuViewOffset = encoderLine; \
    uint8_t _lineNr = currentMenuViewOffset, _menuItemNr; \
    bool wasClicked = LCD_CLICKED, itemSelected; \
    for (uint8_t _drawLineNr = 0; _drawLineNr < LCD_HEIGHT; _drawLineNr++, _lineNr++) { \
      _menuItemNr = 0;

  /**
   * MENU_ITEM generates draw & handler code for a menu item, potentially calling:
   *
   *   lcd_implementation_drawmenu_[type](sel, row, label, arg3...)
   *   menu_action_[type](arg3...)
   *
   * Examples:
   *   MENU_ITEM(back, msg_watch(), lcd_status_screen)
   *     lcd_implementation_drawmenu_back(sel, row, msg_watch(), lcd_status_screen)
   *     menu_action_back(lcd_status_screen)
   *
   *   MENU_ITEM(function, msg_pause_print(), lcd_sdcard_pause)
   *     lcd_implementation_drawmenu_function(sel, row, msg_pause_print(), lcd_sdcard_pause)
   *     menu_action_function(lcd_sdcard_pause)
   *
   *   MENU_ITEM_EDIT(int3, msg_speed(), &feedrate_multiplier, 10, 999)
   *   MENU_ITEM(setting_edit_int3, msg_speed(), msg_speed(), &feedrate_multiplier, 10, 999)
   *     lcd_implementation_drawmenu_setting_edit_int3(sel, row, msg_speed(), msg_speed(), &feedrate_multiplier, 10, 999)
   *     menu_action_setting_edit_int3(msg_speed(), &feedrate_multiplier, 10, 999)
   *
   */
  #define MENU_ITEM(type, label, args...) do { \
    if (_menuItemNr == _lineNr) { \
      itemSelected = encoderLine == _menuItemNr; \
      if (lcdDrawUpdate) \
        lcd_implementation_drawmenu_ ## type(itemSelected, _drawLineNr, label, ## args); \
      if (wasClicked && itemSelected) { \
        lcd_quick_feedback(); \
        menu_action_ ## type(args); \
        return; \
      } \
    } \
    _menuItemNr++; \
  } while(0)

  #if ENABLED(ENCODER_RATE_MULTIPLIER)

    //#define ENCODER_RATE_MULTIPLIER_DEBUG  // If defined, output the encoder steps per second value

    /**
     * MENU_MULTIPLIER_ITEM generates drawing and handling code for a multiplier menu item
     */
    #define MENU_MULTIPLIER_ITEM(type, label, args...) do { \
      if (_menuItemNr == _lineNr) { \
        itemSelected = encoderLine == _menuItemNr; \
        if (lcdDrawUpdate) \
          lcd_implementation_drawmenu_ ## type(itemSelected, _drawLineNr, label, ## args); \
        if (wasClicked && itemSelected) { \
          lcd_quick_feedback(); \
          encoderRateMultiplierEnabled = true; \
          lastEncoderMovementMillis = 0; \
          menu_action_ ## type(args); \
          return; \
        } \
      } \
      _menuItemNr++; \
    } while(0)
  #endif //ENCODER_RATE_MULTIPLIER

  #define MENU_ITEM_DUMMY() do { _menuItemNr++; } while(0)
  #define MENU_ITEM_EDIT(type, label, args...) MENU_ITEM(setting_edit_ ## type, label, label, ## args)
  #define MENU_ITEM_EDIT_CALLBACK(type, label, args...) MENU_ITEM(setting_edit_callback_ ## type, label, label, ## args)
  #if ENABLED(ENCODER_RATE_MULTIPLIER)
    #define MENU_MULTIPLIER_ITEM_EDIT(type, label, args...) MENU_MULTIPLIER_ITEM(setting_edit_ ## type, label, label, ## args)
    #define MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(type, label, args...) MENU_MULTIPLIER_ITEM(setting_edit_callback_ ## type, label, label, ## args)
  #else //!ENCODER_RATE_MULTIPLIER
    #define MENU_MULTIPLIER_ITEM_EDIT(type, label, args...) MENU_ITEM(setting_edit_ ## type, label, label, ## args)
    #define MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(type, label, args...) MENU_ITEM(setting_edit_callback_ ## type, label, label, ## args)
  #endif //!ENCODER_RATE_MULTIPLIER
  #define END_MENU() \
      if (encoderLine >= _menuItemNr) { encoderPosition = _menuItemNr * ENCODER_STEPS_PER_MENU_ITEM - 1; encoderLine = encoderPosition / ENCODER_STEPS_PER_MENU_ITEM; }\
      if (encoderLine >= currentMenuViewOffset + LCD_HEIGHT) { currentMenuViewOffset = encoderLine - LCD_HEIGHT + 1; lcdDrawUpdate = 1; _lineNr = currentMenuViewOffset - 1; _drawLineNr = -1; } \
      } } while(0)

  /** Used variables to keep track of the menu */
  volatile uint8_t buttons;  //the last checked buttons in a bit array.
  #if ENABLED(REPRAPWORLD_KEYPAD)
    volatile uint8_t buttons_reprapworld_keypad; // to store the keypad shift register values
  #endif

  #if ENABLED(LCD_HAS_SLOW_BUTTONS)
    volatile uint8_t slow_buttons; // Bits of the pressed buttons.
  #endif
  uint8_t currentMenuViewOffset;              /* scroll offset in the current menu */
  millis_t next_button_update_ms;
  uint8_t lastEncoderBits;
  uint32_t encoderPosition;
  #if PIN_EXISTS(SD_DETECT)
    uint8_t lcd_sd_status;
  #endif

#endif // ULTIPANEL

menuFunc_t currentMenu = lcd_status_screen; /* function pointer to the currently active menu */
millis_t next_lcd_update_ms;
uint8_t lcd_status_update_delay;
bool ignore_click = false;
bool wait_for_unclick;
uint8_t lcdDrawUpdate = 2;                  /* Set to none-zero when the LCD needs to draw, decreased after every draw. Set to 2 in LCD routines so the LCD gets at least 1 full redraw (first redraw is partial) */

//prevMenu and prevEncoderPosition are used to store the previous menu location when editing settings.
menuFunc_t prevMenu = NULL;
uint16_t prevEncoderPosition;
//Variables used when editing values.
const char* editLabel;
void* editValue;
int32_t minEditValue, maxEditValue;
menuFunc_t callbackFunc;

// lcd auto ho0me
static void lcd_autohome()
{
	//���Z��С��10����,��������10����,�ٹ���.
/*	if(current_position[Z_AXIS]<10)
	{
	//	destination[Z_AXIS]=
		current_position[Z_AXIS]+=10;
		plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS] , current_position[E_AXIS], manual_feedrate[X_AXIS]/60, active_extruder);
	}
*/	
	//��ԭ��
	enqueuecommands_P((PSTR("G28"))); // move all axis home
//	PSTR("G28");
	//�رղ������
//	PSTR("M84");
//	enqueuecommands_P((PSTR("M84"))); // close
	//�ر�X Y�������
	disable_x();
    disable_y();
}

// place-holders for Ki and Kd edits
float raw_Ki, raw_Kd;

/**
 * General function to go directly to a menu
 */
static void lcd_goto_menu(menuFunc_t menu, const bool feedback = false, const uint32_t encoder = 0) {
  if (currentMenu != menu) {
    currentMenu = menu;
    #if ENABLED(NEWPANEL)
      encoderPosition = encoder;
      if (feedback) lcd_quick_feedback();
    #endif
    // For LCD_PROGRESS_BAR re-initialize the custom characters
    #if ENABLED(LCD_PROGRESS_BAR)
      lcd_set_custom_characters(menu == lcd_status_screen);
    #endif
  }
}

/**
 *
 * "Info Screen"
 *
 * This is very display-dependent, so the lcd implementation draws this.
 */

static void lcd_status_screen() {
  encoderRateMultiplierEnabled = false;

  #if ENABLED(LCD_PROGRESS_BAR)
    millis_t ms = millis();
    #if DISABLED(PROGRESS_MSG_ONCE)
      if (ms > progress_bar_ms + PROGRESS_BAR_MSG_TIME + PROGRESS_BAR_BAR_TIME) {
        progress_bar_ms = ms;
      }
    #endif
    #if PROGRESS_MSG_EXPIRE > 0
      // Handle message expire
      if (expire_status_ms > 0) {
        #if ENABLED(SDSUPPORT)
          if (card.isFileOpen()) {
            // Expire the message when printing is active
            if (IS_SD_PRINTING) {
              if (ms >= expire_status_ms) {
                lcd_status_message[0] = '\0';
                expire_status_ms = 0;
              }
            }
            else {
              expire_status_ms += LCD_UPDATE_INTERVAL;
            }
          }
          else {
            expire_status_ms = 0;
          }
        #else
          expire_status_ms = 0;
        #endif //SDSUPPORT
      }
    #endif
  #endif //LCD_PROGRESS_BAR

  lcd_implementation_status_screen();

  #if ENABLED(ULTIPANEL)

    bool current_click = LCD_CLICKED;

    if (ignore_click) {
      if (wait_for_unclick) {
        if (!current_click)
          ignore_click = wait_for_unclick = false;
        else
          current_click = false;
      }
      else if (current_click) {
        lcd_quick_feedback();
        wait_for_unclick = true;
        current_click = false;
      }
    }

    if (current_click) {
      digitalWrite (LED,HIGH);
      lcd_goto_menu(lcd_main_menu, true);
      lcd_implementation_init( // to maybe revive the LCD if static electricity killed it.
        #if ENABLED(LCD_PROGRESS_BAR)
          currentMenu == lcd_status_screen
        #endif
      );
      #if ENABLED(FILAMENT_LCD_DISPLAY)
        previous_lcd_status_ms = millis();  // get status message to show up for a while
      #endif
    }

    #if ENABLED(ULTIPANEL_FEEDMULTIPLY)
      // Dead zone at 100% feedrate
      if ((feedrate_multiplier < 100 && (feedrate_multiplier + int(encoderPosition)) > 100) ||
          (feedrate_multiplier > 100 && (feedrate_multiplier + int(encoderPosition)) < 100)) {
        encoderPosition = 0;
        feedrate_multiplier = 100;
      }
      if (feedrate_multiplier == 100) {
        if (int(encoderPosition) > ENCODER_FEEDRATE_DEADZONE) {
          feedrate_multiplier += int(encoderPosition) - ENCODER_FEEDRATE_DEADZONE;
          encoderPosition = 0;
        }
        else if (int(encoderPosition) < -ENCODER_FEEDRATE_DEADZONE) {
          feedrate_multiplier += int(encoderPosition) + ENCODER_FEEDRATE_DEADZONE;
          encoderPosition = 0;
        }
      }
      else {
        feedrate_multiplier += int(encoderPosition);
        encoderPosition = 0;
      }
    #endif // ULTIPANEL_FEEDMULTIPLY

    feedrate_multiplier = constrain(feedrate_multiplier, 10, 999);

  #endif //ULTIPANEL
}

#if ENABLED(ULTIPANEL)

static void lcd_return_to_status() { lcd_goto_menu(lcd_status_screen); }

#if ENABLED(SDSUPPORT)

int mytemphot=0;
int mytempbed=0;
//float myE0=0;

  static void lcd_sdcard_pause() 
  { 
	uint8_t tmp1_extruder;
//	mytemphot=(int)degHotend(tmp1_extruder);
//	mytempbed=degBed();
	//��ͣ��ӡ
	card.pauseSDPrint();
	//���漷��ͷ���ȴ�Ŀ���¶�
	mytemphot=target_temperature[0];
	mytempbed=target_temperature_bed;
	//����E0������
//	myE0=current_position[E_AXIS];
	//������ͷ���ȴ�����
	setTargetHotend0(0);
	setTargetBed(0);
    
	//��X,Y���ԭ��
//	enquecommand_P(PSTR("G28 X0 Y0")); 
	enqueuecommands_P(PSTR("G28 X0 Y0")); 
	//�رղ������
//	enqueuecommands_P(PSTR("M84")); // close
	//�ر�X Y�������
	disable_x();
    disable_y();
	//  card.pauseSDPrint(); 
  }

  static void lcd_sdcard_resume() 
  {
    if(0==READ(CHECK_MATWEIAL))
    {
      char cmd1[30];
      #if ENABLED(SDSUPPORT) && ENABLED(POWEROFF_SAVE_SD_FILE)
    	if (power_off_commands_count > 0) {
    		sprintf_P(cmd1, PSTR("M190 S%i"), power_off_info.target_temperature_bed);
    		enqueuecommand(cmd1);
    		sprintf_P(cmd1, PSTR("M109 S%i"), power_off_info.target_temperature[0]);
    		enqueuecommand(cmd1);
    		enqueuecommands_P(PSTR("M106 S255"));
        	power_off_type_yes = 1;
    	}
    	else {
    		sprintf_P(cmd1, PSTR("M140 S%i"), mytempbed);
    		enqueuecommand(cmd1);
    		sprintf_P(cmd1, PSTR("M109 S%i"), mytemphot);
    		enqueuecommand(cmd1);
    		enqueuecommands_P(PSTR("G28 X0 Y0"));
    	}
      #else
      sprintf_P(cmd1, PSTR("M140 S%i"), mytempbed);
      enqueuecommand(cmd1);
      sprintf_P(cmd1, PSTR("M109 S%i"), mytemphot);
      enqueuecommand(cmd1);
      enqueuecommands_P(PSTR("G28 X0 Y0"));
      #endif
  
      card.startFileprint();
    }
  }

  static void lcd_sdcard_stop() {
    quickStop();
    card.sdprinting = false;
    card.closefile();
    #if ENABLED(SDSUPPORT) && ENABLED(POWEROFF_SAVE_SD_FILE)
    card.openPowerOffFile(power_off_info.power_off_filename, O_CREAT | O_WRITE | O_TRUNC | O_SYNC);
    power_off_info.valid_head = 0;
    power_off_info.valid_foot = 0;
    if (card.savePowerOffInfo(&power_off_info, sizeof(power_off_info)) == -1){
      SERIAL_PROTOCOLLN("Stop to Write power off file failed.");
    }
    card.closePowerOffFile();
    power_off_commands_count = 0;
    #endif
    autotempShutdown();
    cancel_heatup = true;
    lcd_setstatus(MSG_PRINT_ABORTED, true);
  }
  
  static void lcd_sdcard_resume_return()
{
	lcd_sdcard_resume();
	lcd_return_to_status();
}
  static void lcd_sdcard_stop_return() 
{
	lcd_sdcard_stop();
	lcd_return_to_status();
}
  static void msg_resume_print_ok_title() {
    return;
  }

#endif //SDSUPPORT

/**
 *
 * "Resume yes or no" menu
 *
 */
#if ENABLED(SDSUPPORT) && ENABLED(POWEROFF_SAVE_SD_FILE)
static void lcd_resume_ok_menu() {
  START_MENU();
  MENU_ITEM(function, msg_resume_print_ok(), msg_resume_print_ok_title);
  MENU_ITEM(function, msg_resume_print_y(), lcd_sdcard_resume_return);
  MENU_ITEM(function, msg_resume_print_n(), lcd_sdcard_stop_return);
  END_MENU();
}
#endif

/**
 *
 * "Main" menu
 *
 */

static void lcd_main_menu() {
  START_MENU();
  MENU_ITEM(back, msg_watch(), lcd_status_screen);
  if (movesplanned() || IS_SD_PRINTING) {
    MENU_ITEM(submenu, msg_tune(), lcd_tune_menu);
  }
  else {
    MENU_ITEM(submenu, msg_prepare(), lcd_prepare_menu);//׼���˵�
	  MENU_ITEM(submenu, msg_control(), lcd_control_menu);//���Ʋ˵�
    #if ENABLED(DELTA_CALIBRATION_MENU)
      MENU_ITEM(submenu, msg_delta_calibrate(), lcd_delta_calibrate_menu);
    #endif
    MENU_ITEM(submenu, msg_select_language(), lcd_select_language_menu);
  }


  #if ENABLED(SDSUPPORT)
    if (card.cardOK) {
      if (card.isFileOpen()) {
        #if ENABLED(SDSUPPORT) && ENABLED(POWEROFF_SAVE_SD_FILE)
        if (power_off_commands_count > 0) {
          ;//MENU_ITEM(submenu, msg_resume_print_ok(), lcd_resume_ok_menu);
        }
        else {
        #endif
        if (card.sdprinting)
          MENU_ITEM(function, msg_pause_print(), lcd_sdcard_pause);
        else
          MENU_ITEM(function, msg_resume_print(), lcd_sdcard_resume);
        MENU_ITEM(function, msg_stop_print(), lcd_sdcard_stop);
        #if ENABLED(SDSUPPORT) && ENABLED(POWEROFF_SAVE_SD_FILE)  
        }
        #endif
      }
      else {
        MENU_ITEM(submenu, msg_card_menu(), lcd_sdcard_menu);
        #if !PIN_EXISTS(SD_DETECT)
          MENU_ITEM(gcode, msg_cng_sdcard(), PSTR("M21"));  // SD-card changed by user
        #endif
      }
    }
    else {
      MENU_ITEM(submenu, msg_no_card(), lcd_sdcard_menu);
      #if !PIN_EXISTS(SD_DETECT)
        MENU_ITEM(gcode, msg_init_sdcard(), PSTR("M21")); // Manually initialize the SD-card via user interface
      #endif
    }
  #endif //SDSUPPORT

 // MENU_ITEM(submenu, msg_led(), led);
  
  END_MENU();
}

#if ENABLED(SDSUPPORT) && ENABLED(MENU_ADDAUTOSTART)
  static void lcd_autostart_sd() {
    card.autostart_index = 0;
    card.setroot();
    card.checkautostart(true);
  }
#endif

/**
 * Set the home offset based on the current_position
 */
void lcd_set_home_offsets() {
  // M428 Command
  enqueuecommands_P(PSTR("M428"));
  lcd_return_to_status();
}


#if ENABLED(BABYSTEPPING)

  static void _lcd_babystep(int axis, const char* msg) {
    if (encoderPosition != 0) {
      babystepsTodo[axis] += BABYSTEP_MULTIPLICATOR * (int)encoderPosition;
      encoderPosition = 0;
      lcdDrawUpdate = 1;
    }
    if (lcdDrawUpdate) lcd_implementation_drawedit(msg, "");
    if (LCD_CLICKED) lcd_goto_menu(lcd_tune_menu);
  }
  static void lcd_babystep_x() { _lcd_babystep(X_AXIS, PSTR(MSG_BABYSTEPPING_X)); }
  static void lcd_babystep_y() { _lcd_babystep(Y_AXIS, PSTR(MSG_BABYSTEPPING_Y)); }
  static void lcd_babystep_z() { _lcd_babystep(Z_AXIS, PSTR(MSG_BABYSTEPPING_Z)); }

#endif //BABYSTEPPING

/**
 * Watch temperature callbacks
 */
#if ENABLED(THERMAL_PROTECTION_HOTENDS)
  #if TEMP_SENSOR_0 != 0
    void watch_temp_callback_E0() { start_watching_heater(0); }
  #endif
  #if EXTRUDERS > 1 && TEMP_SENSOR_1 != 0
    void watch_temp_callback_E1() { start_watching_heater(1); }
  #endif // EXTRUDERS > 1
  #if EXTRUDERS > 2 && TEMP_SENSOR_2 != 0
    void watch_temp_callback_E2() { start_watching_heater(2); }
  #endif // EXTRUDERS > 2
  #if EXTRUDERS > 3 && TEMP_SENSOR_3 != 0
    void watch_temp_callback_E3() { start_watching_heater(3); }
  #endif // EXTRUDERS > 3
#else
  #if TEMP_SENSOR_0 != 0
    void watch_temp_callback_E0() {}
  #endif
  #if EXTRUDERS > 1 && TEMP_SENSOR_1 != 0
    void watch_temp_callback_E1() {}
  #endif // EXTRUDERS > 1
  #if EXTRUDERS > 2 && TEMP_SENSOR_2 != 0
    void watch_temp_callback_E2() {}
  #endif // EXTRUDERS > 2
  #if EXTRUDERS > 3 && TEMP_SENSOR_3 != 0
    void watch_temp_callback_E3() {}
  #endif // EXTRUDERS > 3
#endif
/**
 * Items shared between Tune and Temperature menus
 */
static void nozzle_bed_fan_menu_items(uint8_t &encoderLine, uint8_t &_lineNr, uint8_t &_drawLineNr, uint8_t &_menuItemNr, bool &wasClicked, bool &itemSelected) {
  //
  // Nozzle:
  // Nozzle [1-4]:
  //
  #if EXTRUDERS == 1
    #if TEMP_SENSOR_0 != 0
      MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(int3, msg_nozzle(), &target_temperature[0], 0, HEATER_0_MAXTEMP - 15, watch_temp_callback_E0);
    #endif
  #else //EXTRUDERS > 1
    #if TEMP_SENSOR_0 != 0
      MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(int3, msg_nozzle_n1(), &target_temperature[0], 0, HEATER_0_MAXTEMP - 15, watch_temp_callback_E0);
    #endif
    #if TEMP_SENSOR_1 != 0
      MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(int3, msg_nozzle_n2(), &target_temperature[1], 0, HEATER_1_MAXTEMP - 15, watch_temp_callback_E1);
    #endif
    #if EXTRUDERS > 2
      #if TEMP_SENSOR_2 != 0
        MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(int3, msg_nozzle_n3(), &target_temperature[2], 0, HEATER_2_MAXTEMP - 15, watch_temp_callback_E2);
      #endif
      #if EXTRUDERS > 3
        #if TEMP_SENSOR_3 != 0
          MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(int3, msg_nozzle_n4(), &target_temperature[3], 0, HEATER_3_MAXTEMP - 15, watch_temp_callback_E3);
        #endif
      #endif // EXTRUDERS > 3
    #endif // EXTRUDERS > 2
  #endif // EXTRUDERS > 1

  //
  // Bed:
  //
  #if TEMP_SENSOR_BED != 0
    MENU_MULTIPLIER_ITEM_EDIT(int3, msg_bed(), &target_temperature_bed, 0, BED_MAXTEMP - 15);
  #endif

  //
  // Fan Speed:
  //
  MENU_MULTIPLIER_ITEM_EDIT(int3, msg_fan_speed(), &fanSpeed, 0, 255);
}


/**
 *
 * "Tune" submenu
 *
 */
static void lcd_tune_menu() {
  START_MENU();

  //
  // ^ Main
  //
  MENU_ITEM(back, msg_main(), lcd_main_menu);

  //
  // Speed:
  //
  MENU_ITEM_EDIT(int3, msg_speed(), &feedrate_multiplier, 10, 999);

  // Nozzle, Bed, and Fan Control
  //nozzle_bed_fan_menu_items(encoderLine, _lineNr, _drawLineNr, _menuItemNr, wasClicked, itemSelected);
    //��ʾ���޸ļ���ͷ�¶�
	MENU_ITEM_EDIT(int3, msg_nozzle(), &target_temperature[0], 0, HEATER_0_MAXTEMP - 15);
	//��ʾ���޸��ȴ��¶�
	MENU_ITEM_EDIT(int3, msg_bed(), &target_temperature_bed, 0, 150 - 15);
  //
	 // Fan Speed:
  //
  MENU_MULTIPLIER_ITEM_EDIT(int3, msg_fan_speed(), &fanSpeed, 0, 255);

  // Flow:
  // Flow 1:
  // Flow 2:
  // Flow 3:
  // Flow 4:
  //
  #if EXTRUDERS == 1
    MENU_ITEM_EDIT(int3, msg_flow(), &extruder_multiplier[0], 10, 999);
  #else // EXTRUDERS > 1
    MENU_ITEM_EDIT(int3, msg_flow(), &extruder_multiplier[active_extruder], 10, 999);
    MENU_ITEM_EDIT(int3, msg_flow_n1(), &extruder_multiplier[0], 10, 999);
    MENU_ITEM_EDIT(int3, msg_flow_n2(), &extruder_multiplier[1], 10, 999);
    #if EXTRUDERS > 2
      MENU_ITEM_EDIT(int3, msg_flow_n3(), &extruder_multiplier[2], 10, 999);
      #if EXTRUDERS > 3
        MENU_ITEM_EDIT(int3, msg_flow_n4(), &extruder_multiplier[3], 10, 999);
      #endif //EXTRUDERS > 3
    #endif //EXTRUDERS > 2
  #endif //EXTRUDERS > 1

  //
  // Babystep X:
  // Babystep Y:
  // Babystep Z:
  //
  #if ENABLED(BABYSTEPPING)
    #if ENABLED(BABYSTEP_XY)
      MENU_ITEM(submenu, msg_babystep_x(), lcd_babystep_x);
      MENU_ITEM(submenu, msg_babystep_y(), lcd_babystep_y);
    #endif //BABYSTEP_XY
    MENU_ITEM(submenu, msg_babystep_z(), lcd_babystep_z);
  #endif

  //
  // Change filament
  //
  #if ENABLED(FILAMENTCHANGEENABLE)
     MENU_ITEM(gcode, msg_filamentchange(), PSTR("M600"));
  #endif

  END_MENU();
}

void _lcd_preheat(int endnum, const float temph, const float tempb, const int fan) {
  if (temph > 0) setTargetHotend(temph, endnum);
  #if TEMP_SENSOR_BED != 0
    setTargetBed(tempb);
  #endif
  fanSpeed = fan;
  lcd_return_to_status();
}

#if TEMP_SENSOR_0 != 0
  void lcd_preheat_pla0() { _lcd_preheat(0, plaPreheatHotendTemp, plaPreheatHPBTemp, plaPreheatFanSpeed); }
  void lcd_preheat_abs0() { _lcd_preheat(0, absPreheatHotendTemp, absPreheatHPBTemp, absPreheatFanSpeed); }
#endif

#if EXTRUDERS > 1
  void lcd_preheat_pla1() { _lcd_preheat(1, plaPreheatHotendTemp, plaPreheatHPBTemp, plaPreheatFanSpeed); }
  void lcd_preheat_abs1() { _lcd_preheat(1, absPreheatHotendTemp, absPreheatHPBTemp, absPreheatFanSpeed); }
  #if EXTRUDERS > 2
    void lcd_preheat_pla2() { _lcd_preheat(2, plaPreheatHotendTemp, plaPreheatHPBTemp, plaPreheatFanSpeed); }
    void lcd_preheat_abs2() { _lcd_preheat(2, absPreheatHotendTemp, absPreheatHPBTemp, absPreheatFanSpeed); }
    #if EXTRUDERS > 3
      void lcd_preheat_pla3() { _lcd_preheat(3, plaPreheatHotendTemp, plaPreheatHPBTemp, plaPreheatFanSpeed); }
      void lcd_preheat_abs3() { _lcd_preheat(3, absPreheatHotendTemp, absPreheatHPBTemp, absPreheatFanSpeed); }
    #endif
  #endif

  void lcd_preheat_pla0123() {
    setTargetHotend0(plaPreheatHotendTemp);
    setTargetHotend1(plaPreheatHotendTemp);
    setTargetHotend2(plaPreheatHotendTemp);
    _lcd_preheat(3, plaPreheatHotendTemp, plaPreheatHPBTemp, plaPreheatFanSpeed);
  }
  void lcd_preheat_abs0123() {
    setTargetHotend0(absPreheatHotendTemp);
    setTargetHotend1(absPreheatHotendTemp);
    setTargetHotend2(absPreheatHotendTemp);
    _lcd_preheat(3, absPreheatHotendTemp, absPreheatHPBTemp, absPreheatFanSpeed);
  }

#endif // EXTRUDERS > 1

#if TEMP_SENSOR_BED != 0
  void lcd_preheat_pla_bedonly() { _lcd_preheat(0, 0, plaPreheatHPBTemp, plaPreheatFanSpeed); }
  void lcd_preheat_abs_bedonly() { _lcd_preheat(0, 0, absPreheatHPBTemp, absPreheatFanSpeed); }
#endif

#if TEMP_SENSOR_0 != 0 && (TEMP_SENSOR_1 != 0 || TEMP_SENSOR_2 != 0 || TEMP_SENSOR_3 != 0 || TEMP_SENSOR_BED != 0)

  static void lcd_preheat_pla_menu() {
    START_MENU();
    MENU_ITEM(back, msg_prepare(), lcd_prepare_menu);
    #if EXTRUDERS == 1
      MENU_ITEM(function, msg_preheat_pla(), lcd_preheat_pla0);
    #else
      MENU_ITEM(function, msg_preheat_pla_n_h1(), lcd_preheat_pla0);
      MENU_ITEM(function, msg_preheat_pla_n_h2(), lcd_preheat_pla1);
      #if EXTRUDERS > 2
        MENU_ITEM(function, msg_preheat_pla_n_h3(), lcd_preheat_pla2);
        #if EXTRUDERS > 3
          MENU_ITEM(function, msg_preheat_pla_n_h4(), lcd_preheat_pla3);
        #endif
      #endif
      MENU_ITEM(function, msg_preheat_pla_all(), lcd_preheat_pla0123);
    #endif
    #if TEMP_SENSOR_BED != 0
 //     MENU_ITEM(function, msg_preheat_pla_bedonly(), lcd_preheat_pla_bedonly); //PLAԤ���ȴ�
    #endif
    END_MENU();
  }

  static void lcd_preheat_abs_menu() {
    START_MENU();
    MENU_ITEM(back, msg_prepare(), lcd_prepare_menu);
    #if EXTRUDERS == 1
      MENU_ITEM(function, msg_preheat_abs(), lcd_preheat_abs0);
    #else
      MENU_ITEM(function, msg_preheat_abs_n_h1(), lcd_preheat_abs0);
      MENU_ITEM(function, msg_preheat_abs_n_h2(), lcd_preheat_abs1);
      #if EXTRUDERS > 2
        MENU_ITEM(function, msg_preheat_abs_n_h3(), lcd_preheat_abs2);
        #if EXTRUDERS > 3
          MENU_ITEM(function, msg_preheat_abs_n_h4(), lcd_preheat_abs3);
        #endif
      #endif
      MENU_ITEM(function, msg_preheat_abs_all(), lcd_preheat_abs0123);
    #endif
    #if TEMP_SENSOR_BED != 0
  //    MENU_ITEM(function, msg_preheat_abs_bedonly(), lcd_preheat_abs_bedonly);  //ABSԤ���ȴ�
    #endif
    END_MENU();
  }

#endif // TEMP_SENSOR_0 && (TEMP_SENSOR_1 || TEMP_SENSOR_2 || TEMP_SENSOR_3 || TEMP_SENSOR_BED)

void lcd_cooldown() {
  disable_all_heaters();
  fanSpeed = 0;
  lcd_return_to_status();

}

void lcd_english() {
  set_select_font_index0_to_eeprom();
  delay(300);
  reboot();
}

void lcd_chinese() {
  set_select_font_index1_to_eeprom();
  delay(300);
  reboot();
}

void lcd_japanese() {
  set_select_font_index2_to_eeprom();
  delay(300);
  reboot();
}


void lcd_filament_load0(){ 

	//START_MENU();
//	MENU_ITEM(back, "\x9f\xc4 ...", lcd_status_screen); //������һ��˵�prepare
//	MENU_ITEM(back, "\xfc\xfd\xb8\x9e\xfe\xff" , lcd_status_screen);
//	MENU_ITEM(back, "the extruder1..." , lcd_status_screen);
//	END_MENU();

	enqueuecommands_P((PSTR("G92  E0")));
	enqueuecommands_P((PSTR("M109 S205"))); // ����190�
//	WRITE(E0_DIR_PIN, HIGH);
	enqueuecommands_P((PSTR("G1 F150 E150")));	//����
  lcd_return_to_status(); 
}
void lcd_filament_load1(){ 

	START_MENU();
	MENU_ITEM(back, PSTR("Wait for the fila-"), lcd_status_screen); //������һ��˵�prepare
	MENU_ITEM(back, PSTR("ment to comes out of"), lcd_status_screen);
	MENU_ITEM(back, PSTR("the extruder2..."), lcd_status_screen);
	END_MENU();

	enqueuecommands_P((PSTR("M109 S205"))); // ����190�
	WRITE(E1_DIR_PIN, HIGH);
	enqueuecommands_P((PSTR("G1 F200 E100")));	//����
}
//�Զ�����
void lcd_filament_unload0(){
	//START_MENU();
//	MENU_ITEM(back, "Wait for the fila-", lcd_status_screen); //������һ��˵�prepare
//	MENU_ITEM(back, "ment to exit from" , lcd_status_screen);
	//MENU_ITEM(back, "\x9f\xc4 ...", lcd_status_screen); //������һ��˵�prepare
//	MENU_ITEM(back, "\xfc\xfd\x9d\x9e\xfe\xff" , lcd_status_screen);
//	MENU_ITEM(back, "the extruder1..." , lcd_status_screen);
	//END_MENU();

	enqueuecommands_P((PSTR("G92  E0")));
	enqueuecommands_P((PSTR("M109 S210"))); // ����190��
//	WRITE(E0_DIR_PIN, LOW);
  enqueuecommands_P((PSTR("G1 F100 E15")));
	enqueuecommands_P((PSTR("G1 F500 E-100")));	//����
  lcd_return_to_status(); 
}
//�Զ�����
void lcd_filament_unload1(){
	START_MENU();
	MENU_ITEM(back, PSTR("Wait for the fila-"), lcd_status_screen); //������һ��˵�prepare
	MENU_ITEM(back, PSTR("ment to exit from"), lcd_status_screen);
	MENU_ITEM(back, PSTR("the extruder2..."), lcd_status_screen);
	END_MENU();

	enqueuecommands_P((PSTR("M109 S210"))); // ����190�
	WRITE(E1_DIR_PIN, LOW);
	enqueuecommands_P((PSTR("G1 F20000 E150")));	//����
}
/**
 *
 * "Prepare" submenu
 *
 */

static void lcd_prepare_menu() {
  START_MENU();

  //
  //  Main
  //
  MENU_ITEM(back, msg_main(), lcd_main_menu);

  //
  // Auto Home
  //
//  MENU_ITEM(gcode, msg_auto_home(), PSTR("G28"));
    MENU_ITEM(function, msg_auto_home(), lcd_autohome);
  //
  // Set Home Offsets
  // ����ԭ��ƫ����
 // MENU_ITEM(function, msg_set_home_offsets(), lcd_set_home_offsets);
  //MENU_ITEM(gcode, msg_set_origin(), PSTR("G92 X0 Y0 Z0"));

  //
  // Level Bed
  //
  #if ENABLED(AUTO_BED_LEVELING_FEATURE)
    if (axis_known_position[X_AXIS] && axis_known_position[Y_AXIS])
      MENU_ITEM(gcode, msg_level_bed(), PSTR("G29"));
  #elif ENABLED(MANUAL_BED_LEVELING)
    MENU_ITEM(submenu, msg_level_bed(), lcd_level_bed);
  #endif

  //
  // Move Axis
  //
  MENU_ITEM(submenu, msg_move_axis(), lcd_move_menu);

  MENU_ITEM(submenu, msg_tiaoping(), tiaoping);

  //
  // Disable Steppers
  //�رյ��
  MENU_ITEM(gcode, msg_disable_steppers(), PSTR("M84"));

  //�Զ�����
 // MENU_ITEM(submenu, msg_filamentchange(), lcd_filament_change0);
//  MENU_ITEM(submenu, MSG_FILAMENTCHANGE "2", lcd_filament_change1);
  //
  // Preheat PLA
  // Preheat ABS
  //
  #if TEMP_SENSOR_0 != 0
    #if TEMP_SENSOR_1 != 0 || TEMP_SENSOR_2 != 0 || TEMP_SENSOR_3 != 0 || TEMP_SENSOR_BED != 0
//      MENU_ITEM(submenu, msg_preheat_pla(), lcd_preheat_pla_menu);
//      MENU_ITEM(submenu, msg_preheat_abs(), lcd_preheat_abs_menu);
		MENU_ITEM(function, msg_preheat_pla(), lcd_preheat_pla0);
		MENU_ITEM(function, msg_preheat_abs(), lcd_preheat_abs0);
    #else
      MENU_ITEM(function, msg_preheat_pla(), lcd_preheat_pla0);
      MENU_ITEM(function, msg_preheat_abs(), lcd_preheat_abs0);
    #endif
  #endif

  //
  // Cooldown
  //
  MENU_ITEM(function, msg_cooldown(), lcd_cooldown);

  //  
  // Switch power on/off
  
 /* #if HAS_POWER_SWITCH
    if (powersupply)
      MENU_ITEM(gcode, msg_switch_ps_off(), PSTR("M81"));
    else
      MENU_ITEM(gcode, msg_switch_ps_on(), PSTR("M80"));
  #endif */
	

  //
  // Autostart
  //�Զ�����
/*  #if ENABLED(SDSUPPORT) && ENABLED(MENU_ADDAUTOSTART)
    MENU_ITEM(function, msg_autostart(), lcd_autostart_sd);
  #endif
*/
  END_MENU();
}

#if ENABLED(DELTA_CALIBRATION_MENU)

  static void lcd_delta_calibrate_menu() {
    START_MENU();
    MENU_ITEM(back, msg_main(), lcd_main_menu);
    MENU_ITEM(gcode, msg_auto_home(), PSTR("G28"));
    MENU_ITEM(gcode, msg_delta_calibrate_x(), PSTR("G0 F8000 X-77.94 Y-45 Z0"));
    MENU_ITEM(gcode, msg_delta_calibrate_y(), PSTR("G0 F8000 X77.94 Y-45 Z0"));
    MENU_ITEM(gcode, msg_delta_calibrate_z(), PSTR("G0 F8000 X0 Y90 Z0"));
    MENU_ITEM(gcode, msg_delta_calibrate_center(), PSTR("G0 F8000 X0 Y0 Z0"));
    END_MENU();
  }

#endif // DELTA_CALIBRATION_MENU

inline void line_to_current(AxisEnum axis) {
  #if ENABLED(DELTA)
    calculate_delta(current_position);
    plan_buffer_line(delta[X_AXIS], delta[Y_AXIS], delta[Z_AXIS], current_position[E_AXIS], manual_feedrate[axis]/60, active_extruder);
  #else
 //   plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[axis]/60, active_extruder);
	plan_buffer_line(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS], manual_feedrate[axis]/60, active_extruder);
  #endif
}

/**
 *
 * "Prepare" > "Move Axis" submenu
 *
 */

float move_menu_scale;
static void lcd_move_menu_axis();

static void _lcd_move(const char* name, AxisEnum axis, int min, int max) {
  if (encoderPosition != 0) {
    refresh_cmd_timeout();
    current_position[axis] += float((int)encoderPosition) * move_menu_scale;
    if (min_software_endstops && current_position[axis] < min) current_position[axis] = min;
    if (max_software_endstops && current_position[axis] > max) current_position[axis] = max;
    encoderPosition = 0;
    line_to_current(axis);
    lcdDrawUpdate = 1;
  }
  if (lcdDrawUpdate) lcd_implementation_drawedit(name, ftostr31(current_position[axis]));
  if (LCD_CLICKED) lcd_goto_menu(lcd_move_menu_axis);
}
static void lcd_move_x() { _lcd_move(msg_move_x(), X_AXIS, X_MIN_POS, X_MAX_POS); }
static void lcd_move_y() { _lcd_move(msg_move_y(), Y_AXIS, Y_MIN_POS, Y_MAX_POS); }
static void lcd_move_z() { _lcd_move(msg_move_z(), Z_AXIS, Z_MIN_POS, Z_MAX_POS); }
static void lcd_move_e(
  #if EXTRUDERS > 1
    uint8_t e
  #endif
) {
  #if EXTRUDERS > 1
    unsigned short original_active_extruder = active_extruder;
    active_extruder = e;
  #endif
  if (encoderPosition != 0) {
    current_position[E_AXIS] += float((int)encoderPosition) * move_menu_scale;
    encoderPosition = 0;
    line_to_current(E_AXIS);
    lcdDrawUpdate = 1;
  }
  if (lcdDrawUpdate) {
    PGM_P pos_label;
    #if EXTRUDERS == 1
      pos_label = PSTR(MSG_MOVE_E);
    #else
      switch (e) {
        case 0: pos_label = msg_move_e_move_e1(); break;
        case 1: pos_label = msg_move_e_move_e2(); break;
        #if EXTRUDERS > 2
          case 2: pos_label = msg_move_e_move_e3(); break;
          #if EXTRUDERS > 3
            case 3: pos_label = msg_move_e_move_e4(); break;
          #endif //EXTRUDERS > 3
        #endif //EXTRUDERS > 2
      }
    #endif //EXTRUDERS > 1
    lcd_implementation_drawedit(pos_label, ftostr31(current_position[E_AXIS]));
  }
  if (LCD_CLICKED) lcd_goto_menu(lcd_move_menu_axis);
  #if EXTRUDERS > 1
    active_extruder = original_active_extruder;
  #endif
}

#if EXTRUDERS > 1
  static void lcd_move_e0() { lcd_move_e(0); }
  static void lcd_move_e1() { lcd_move_e(1); }
  #if EXTRUDERS > 2
    static void lcd_move_e2() { lcd_move_e(2); }
    #if EXTRUDERS > 3
      static void lcd_move_e3() { lcd_move_e(3); }
    #endif
  #endif
#endif // EXTRUDERS > 1

/**
 *
 * "Prepare" > "Move Xmm" > "Move XYZ" submenu
 *
 */

static void lcd_move_menu_axis() {
  START_MENU();
  MENU_ITEM(back, msg_move_axis(), lcd_move_menu);
  MENU_ITEM(submenu, msg_move_x(), lcd_move_x);
  MENU_ITEM(submenu, msg_move_y(), lcd_move_y);
  if (move_menu_scale < 10.0) {
    MENU_ITEM(submenu, msg_move_z(), lcd_move_z);
    #if EXTRUDERS == 1
      MENU_ITEM(submenu, msg_move_e(), lcd_move_e);
    #else
      MENU_ITEM(submenu, msg_move_e_move_e1(), lcd_move_e0);
      MENU_ITEM(submenu, msg_move_e_move_e2(), lcd_move_e1);
      #if EXTRUDERS > 2
        MENU_ITEM(submenu, msg_move_e_move_e3(), lcd_move_e2);
        #if EXTRUDERS > 3
          MENU_ITEM(submenu, msg_move_e_move_e4(), lcd_move_e3);
        #endif
      #endif
    #endif // EXTRUDERS > 1
  }
  END_MENU();
}

static void lcd_move_menu_10mm() {
  move_menu_scale = 10.0;
  lcd_move_menu_axis();
}
static void lcd_move_menu_1mm() {
  move_menu_scale = 1.0;
  lcd_move_menu_axis();
}
static void lcd_move_menu_01mm() {
  move_menu_scale = 0.1;
  lcd_move_menu_axis();
}

/**
 *
 * "Prepare" > "Move Axis" submenu
 *
 */

static void lcd_move_menu() {
  START_MENU();
  MENU_ITEM(back, msg_prepare(), lcd_prepare_menu);
  MENU_ITEM(submenu, msg_move_10mm(), lcd_move_menu_10mm);
  MENU_ITEM(submenu, msg_move_1mm(), lcd_move_menu_1mm);
  MENU_ITEM(submenu, msg_move_01mm(), lcd_move_menu_01mm);
  //TODO:X,Y,Z,E
  END_MENU();
}


//�Զ�����
static void lcd_filament_change0(){
	START_MENU();
	MENU_ITEM(back, msg_prepare(), lcd_prepare_menu); //������һ��˵�prepare
	MENU_ITEM(function, msg_load(), lcd_filament_load0);
	MENU_ITEM(function, msg_unload(), lcd_filament_unload0);
	END_MENU();
	enqueuecommands_P(PSTR("G82"));
	enqueuecommands_P(PSTR("G92 E0"));	//置零
	enqueuecommands_P(PSTR("T0"));
}

static void lcd_filament_change1(){
	START_MENU();
	MENU_ITEM(back, msg_prepare(), lcd_prepare_menu); //������һ��˵�prepare
	MENU_ITEM(submenu, msg_load(), lcd_filament_load1);
	MENU_ITEM(submenu, msg_unload(), lcd_filament_unload1);
	END_MENU();
	enqueuecommands_P(PSTR("G82"));
	enqueuecommands_P(PSTR("G92 E0"));	//置零
	enqueuecommands_P(PSTR("T1"));
}

void reseted()
{
   Config_ResetDefault();
   lcd_return_to_status(); 
}

static void led(){
  START_MENU();
  MENU_ITEM(back, msg_main(), lcd_main_menu);
  MENU_ITEM(function, msg_led_on(), led_on);
  MENU_ITEM(function, msg_led_off(), led_off);
  END_MENU();
  
  }
void led_on(){
 // START_MENU();
 // MENU_ITEM(back, "ON" , lcd_status_screen);
 // END_MENU();

 digitalWrite (LED,HIGH); 
 lcd_return_to_status(); 
  
  }

void led_off(){
 // START_MENU();
 // MENU_ITEM(back, "OFF" , lcd_status_screen);
 // END_MENU();

digitalWrite (LED,LOW); 
lcd_return_to_status(); 
  
  }
/*if (millis() > led_auto_off)
      {
        lcd_return_to_status();
        lcdDrawUpdate = 2;
    //    return_to_status_ms = ms + LCD_TIMEOUT_TO_STATUS;
    //    led_auto_off = ms + led_off_time ;
       digitalWrite (LED,LOW);
      }*/

  static void tiaoping(){
  START_MENU();
  MENU_ITEM(back, msg_prepare(), lcd_prepare_menu);
  MENU_ITEM(function, msg_next(), next);
//  MENU_ITEM(function, msg_led_off(), led_off);
  END_MENU();
  
  }
  int i=0;
void next(){
 // START_MENU();
 // MENU_ITEM(back, "ON" , lcd_status_screen);
 // END_MENU();
// int i;
 i=i+1;
 if(i==1)
 { 
  // enqueuecommands_P((PSTR("M109 S200")));
 //  enqueuecommands_P((PSTR("G1 F100 E5")));
 //  enqueuecommands_P((PSTR("G1 F20000 E-10")));
   enqueuecommands_P((PSTR("G28 X0 Y0 Z0"))); //全部回原点
   enqueuecommands_P((PSTR("G1 F100 Z3;"))); //Z轴升高3MM
   enqueuecommands_P((PSTR("G90")));//使用绝对坐标
   enqueuecommands_P((PSTR("G1 X30 Y30 F5000")));//X,Y移动到30,30点
   enqueuecommands_P((PSTR("G1 F100 Z-3")));//Z轴下降3mm
 //  lcd_return_to_status();

 }
 if(i==2)
 {
   enqueuecommands_P((PSTR("G1 F200 Z3"))); //Z轴升高3MM
   enqueuecommands_P((PSTR("G1 Y270 F5000")));//X,Y移动到270,30点
   enqueuecommands_P((PSTR("G1 F200 Z-3")));//Z轴下降3mm
 }
 if(i==3)
 {
   enqueuecommands_P((PSTR("G1 F200 Z3"))); //Z轴升高3MM
   enqueuecommands_P((PSTR("G1 X270 F5000")));//X,Y移动到270,270点
   enqueuecommands_P((PSTR("G1 F200 Z-3")));//Z轴下降3mm
 }
 if(i==4)
 {
 enqueuecommands_P((PSTR("G1 F200 Z3"))); //Z轴升高3MM
 enqueuecommands_P((PSTR("G1 Y30 F5000")));//X,Y移动到30,270点
 enqueuecommands_P((PSTR("G1 F200 Z-3")));//Z轴下降3mm
 }
 if(i==5)
 {
 enqueuecommands_P((PSTR("G1 F200 Z3"))); //Z轴升高3MM
 enqueuecommands_P((PSTR("G1 X150 Y150 F5000")));//X,Y移动到100,100点
 enqueuecommands_P((PSTR("G1 F200 Z-3")));//Z轴下降3mm
 }
 if(i==6)
 {
 enqueuecommands_P((PSTR("G1 F200 Z3"))); //Z轴升高3MM
 //enqueuecommands_P((PSTR("G1 X100 Y100 F5000")));//X,Y移动到100,100点
 //enqueuecommands_P((PSTR("G1 F200 Z-3")));//Z轴下降3mm
 i=0;
 lcd_return_to_status();
 }
 
 //lcd_return_to_status(); 
  
  }
/*void led_off(){
 // START_MENU();
 // MENU_ITEM(back, "OFF" , lcd_status_screen);
 // END_MENU();

digitalWrite (LED,LOW); 
lcd_return_to_status(); 
  
  }*/
  
/**
 *
 * "Control" submenu
 *
 */

static void lcd_control_menu() {
  START_MENU();
  MENU_ITEM(back, msg_main(), lcd_main_menu);
  MENU_ITEM(submenu, msg_temperature(), lcd_control_temperature_menu);
  //MENU_ITEM(submenu, msg_motion(), lcd_control_motion_menu);	//�������
//  MENU_ITEM(submenu, msg_volumetric(), lcd_control_volumetric_menu);	//����ֱ��

 /* #if ENABLED(HAS_LCD_CONTRAST)
    //MENU_ITEM_EDIT(int3, msg_contrast(), &lcd_contrast, 0, 63);
    MENU_ITEM(submenu, msg_contrast(), lcd_set_contrast);
  #endif
  #if ENABLED(FWRETRACT)
    MENU_ITEM(submenu, msg_retract(), lcd_control_retract_menu);
  #endif
  #if ENABLED(EEPROM_SETTINGS)
    MENU_ITEM(function, msg_store_eprom(), Config_StoreSettings);
    MENU_ITEM(function, msg_load_eprom(), Config_RetrieveSettings);
  #endif*/
  MENU_ITEM(function, msg_restore_failsafe(), reseted);
  END_MENU();
}

/**
 *
 * "Language" submenu
 *
 */
static void lcd_select_language_menu() {
  START_MENU();
  MENU_ITEM(back, msg_main(), lcd_main_menu);

  MENU_ITEM(function, msg_english_lang(), lcd_english);
  MENU_ITEM(function, msg_chinese_lang(), lcd_chinese);
  //MENU_ITEM(function, msg_japanese_lang(), lcd_japanese);

  END_MENU();
}
/**
 *
 * "Temperature" submenu
 *
 */

#if ENABLED(PIDTEMP)

  // Helpers for editing PID Ki & Kd values
  // grab the PID value out of the temp variable; scale it; then update the PID driver
  void copy_and_scalePID_i(int e) {
    PID_PARAM(Ki, e) = scalePID_i(raw_Ki);
    updatePID();
  }
  void copy_and_scalePID_d(int e) {
    PID_PARAM(Kd, e) = scalePID_d(raw_Kd);
    updatePID();
  }
  #define COPY_AND_SCALE(eindex) \
    void copy_and_scalePID_i_E ## eindex() { copy_and_scalePID_i(eindex); } \
    void copy_and_scalePID_d_E ## eindex() { copy_and_scalePID_d(eindex); }

  COPY_AND_SCALE(0);
  #if ENABLED(PID_PARAMS_PER_EXTRUDER)
    #if EXTRUDERS > 1
      COPY_AND_SCALE(1);
      #if EXTRUDERS > 2
        COPY_AND_SCALE(2);
        #if EXTRUDERS > 3
          COPY_AND_SCALE(3);
        #endif //EXTRUDERS > 3
      #endif //EXTRUDERS > 2
    #endif //EXTRUDERS > 1
  #endif //PID_PARAMS_PER_EXTRUDER

#endif //PIDTEMP

/**
 *
 * "Control" > "Temperature" submenu
 *
 */
static void lcd_control_temperature_menu() {
  START_MENU();

  //
  // ^ Control
  //
  MENU_ITEM(back, msg_control(), lcd_control_menu);

  // Nozzle, Bed, and Fan Control
//  nozzle_bed_fan_menu_items(encoderLine, _lineNr, _drawLineNr, _menuItemNr, wasClicked, itemSelected);
  //��ʾ���޸ļ���ͷ�¶�
	MENU_ITEM_EDIT(int3, msg_nozzle_h1(), &target_temperature[0], 0, HEATER_0_MAXTEMP - 15);
//	MENU_ITEM_EDIT(int3, msg_nozzle_h2(), &target_temperature[1], 0, HEATER_1_MAXTEMP - 15);
	//��ʾ���޸��ȴ��¶�
	MENU_ITEM_EDIT(int3, msg_bed(), &target_temperature_bed, 0, 150 - 15);
	 //
  // Fan Speed:
  //
  MENU_MULTIPLIER_ITEM_EDIT(int3, msg_fan_speed(), &fanSpeed, 0, 255);

  //
  // Autotemp, Min, Max, Fact
  /*
  #if ENABLED(AUTOTEMP) && (TEMP_SENSOR_0 != 0)
    MENU_ITEM_EDIT(bool, msg_autotemp(), &autotemp_enabled);
    MENU_ITEM_EDIT(float3, msg_min(), &autotemp_min, 0, HEATER_0_MAXTEMP - 15);
    MENU_ITEM_EDIT(float3, msg_max(), &autotemp_max, 0, HEATER_0_MAXTEMP - 15);
    MENU_ITEM_EDIT(float32, msg_factor(), &autotemp_factor, 0.0, 1.0);
  #endif
	
  //
  // PID-P, PID-I, PID-D, PID-C
  // PID-P E1, PID-I E1, PID-D E1, PID-C E1
  // PID-P E2, PID-I E2, PID-D E2, PID-C E2
  // PID-P E3, PID-I E3, PID-D E3, PID-C E3
  // PID-P E4, PID-I E4, PID-D E4, PID-C E4
  //
  #if ENABLED(PIDTEMP)

    #define _PID_MENU_ITEMS(ELABEL, eindex) \
      raw_Ki = unscalePID_i(PID_PARAM(Ki, eindex)); \
      raw_Kd = unscalePID_d(PID_PARAM(Kd, eindex)); \
      MENU_ITEM_EDIT(float52, MSG_PID_P ELABEL, &PID_PARAM(Kp, eindex), 1, 9990); \
      MENU_ITEM_EDIT_CALLBACK(float52, MSG_PID_I ELABEL, &raw_Ki, 0.01, 9990, copy_and_scalePID_i_E ## eindex); \
      MENU_ITEM_EDIT_CALLBACK(float52, MSG_PID_D ELABEL, &raw_Kd, 1, 9990, copy_and_scalePID_d_E ## eindex)

    #if ENABLED(PID_ADD_EXTRUSION_RATE)
      #define PID_MENU_ITEMS(ELABEL, eindex) \
        _PID_MENU_ITEMS(ELABEL, eindex); \
        MENU_ITEM_EDIT(float3, MSG_PID_C ELABEL, &PID_PARAM(Kc, eindex), 1, 9990)
    #else
      #define PID_MENU_ITEMS(ELABEL, eindex) _PID_MENU_ITEMS(ELABEL, eindex)
    #endif

    #if ENABLED(PID_PARAMS_PER_EXTRUDER) && EXTRUDERS > 1
      PID_MENU_ITEMS(msg_e1(), 0);
      PID_MENU_ITEMS(msg_e2(), 1);
      #if EXTRUDERS > 2
        PID_MENU_ITEMS(msg_e3(), 2);
        #if EXTRUDERS > 3
          PID_MENU_ITEMS(msg_e4(), 3);
        #endif //EXTRUDERS > 3
      #endif //EXTRUDERS > 2
    #else //!PID_PARAMS_PER_EXTRUDER || EXTRUDERS == 1
      PID_MENU_ITEMS("", 0);
    #endif //!PID_PARAMS_PER_EXTRUDER || EXTRUDERS == 1

  #endif //PIDTEMP
  
  //
  // Preheat PLA conf
  //
  MENU_ITEM(submenu, msg_preheat_pla_settings(), lcd_control_temperature_preheat_pla_settings_menu);

  //
  // Preheat ABS conf
  //
  MENU_ITEM(submenu, msg_preheat_abs_settings(), lcd_control_temperature_preheat_abs_settings_menu);
  */
  END_MENU();
}

/**
 *
 * "Temperature" > "Preheat PLA conf" submenu
 *
 */
static void lcd_control_temperature_preheat_pla_settings_menu() {
  START_MENU();
  MENU_ITEM(back, msg_temperature(), lcd_control_temperature_menu);
  MENU_ITEM_EDIT(int3, msg_fan_speed(), &plaPreheatFanSpeed, 0, 255);
  #if TEMP_SENSOR_0 != 0
    MENU_ITEM_EDIT(int3, msg_nozzle(), &plaPreheatHotendTemp, HEATER_0_MINTEMP, HEATER_0_MAXTEMP - 15);
  #endif
  #if TEMP_SENSOR_BED != 0
    MENU_ITEM_EDIT(int3, msg_bed(), &plaPreheatHPBTemp, BED_MINTEMP, BED_MAXTEMP - 15);
  #endif
  #if ENABLED(EEPROM_SETTINGS)
    MENU_ITEM(function, msg_store_eprom(), Config_StoreSettings);
  #endif
  END_MENU();
}

/**
 *
 * "Temperature" > "Preheat ABS conf" submenu
 *
 */
static void lcd_control_temperature_preheat_abs_settings_menu() {
  START_MENU();
  MENU_ITEM(back, msg_temperature(), lcd_control_temperature_menu);
  MENU_ITEM_EDIT(int3, msg_fan_speed(), &absPreheatFanSpeed, 0, 255);
  #if TEMP_SENSOR_0 != 0
    MENU_ITEM_EDIT(int3, msg_nozzle(), &absPreheatHotendTemp, HEATER_0_MINTEMP, HEATER_0_MAXTEMP - 15);
  #endif
  #if TEMP_SENSOR_BED != 0
    MENU_ITEM_EDIT(int3, msg_bed(), &absPreheatHPBTemp, BED_MINTEMP, BED_MAXTEMP - 15);
  #endif
  #if ENABLED(EEPROM_SETTINGS)
    MENU_ITEM(function, msg_store_eprom(), Config_StoreSettings);
  #endif
  END_MENU();
}

/**
 *
 * "Control" > "Motion" submenu
 *
 */
static void lcd_control_motion_menu() {
  START_MENU();
  MENU_ITEM(back, msg_control(), lcd_control_menu);
  #if ENABLED(AUTO_BED_LEVELING_FEATURE)
    MENU_ITEM_EDIT(float32, msg_zprobe_zoffset(), &zprobe_zoffset, Z_PROBE_OFFSET_RANGE_MIN, Z_PROBE_OFFSET_RANGE_MAX);
  #endif
  MENU_ITEM_EDIT(float5, msg_acc(), &acceleration, 10, 99000);
  MENU_ITEM_EDIT(float3, msg_vxy_jerk(), &max_xy_jerk, 1, 990);
  MENU_ITEM_EDIT(float52, msg_vz_jerk(), &max_z_jerk, 0.1, 990);
  MENU_ITEM_EDIT(float3, msg_ve_jerk(), &max_e_jerk, 1, 990);
  MENU_ITEM_EDIT(float3, msg_vmax_x(), &max_feedrate[X_AXIS], 1, 999);
  MENU_ITEM_EDIT(float3, msg_vmax_y(), &max_feedrate[Y_AXIS], 1, 999);
  MENU_ITEM_EDIT(float3, msg_vmax_z(), &max_feedrate[Z_AXIS], 1, 999);
  MENU_ITEM_EDIT(float3, msg_vmax_e(), &max_feedrate[E_AXIS], 1, 999);
  MENU_ITEM_EDIT(float3, msg_vmin(), &minimumfeedrate, 0, 999);
  MENU_ITEM_EDIT(float3, msg_vtrav_min(), &mintravelfeedrate, 0, 999);
  MENU_ITEM_EDIT_CALLBACK(long5, msg_amax_x(), &max_acceleration_units_per_sq_second[X_AXIS], 100, 99000, reset_acceleration_rates);
  MENU_ITEM_EDIT_CALLBACK(long5, msg_amax_y(), &max_acceleration_units_per_sq_second[Y_AXIS], 100, 99000, reset_acceleration_rates);
  MENU_ITEM_EDIT_CALLBACK(long5, msg_amax_z(), &max_acceleration_units_per_sq_second[Z_AXIS], 10, 99000, reset_acceleration_rates);
  MENU_ITEM_EDIT_CALLBACK(long5, msg_amax_e(), &max_acceleration_units_per_sq_second[E_AXIS], 100, 99000, reset_acceleration_rates);
  MENU_ITEM_EDIT(float5, msg_a_retract(), &retract_acceleration, 100, 99000);
  MENU_ITEM_EDIT(float5, msg_a_travel(), &travel_acceleration, 100, 99000);
  MENU_ITEM_EDIT(float52, msg_xsteps(), &axis_steps_per_unit[X_AXIS], 5, 9999);
  MENU_ITEM_EDIT(float52, msg_ysteps(), &axis_steps_per_unit[Y_AXIS], 5, 9999);
  MENU_ITEM_EDIT(float51, msg_zsteps(), &axis_steps_per_unit[Z_AXIS], 5, 9999);
  MENU_ITEM_EDIT(float51, msg_esteps(), &axis_steps_per_unit[E_AXIS], 5, 9999);
  #if ENABLED(ABORT_ON_ENDSTOP_HIT_FEATURE_ENABLED)
    MENU_ITEM_EDIT(bool, msg_endstop_abort(), &abort_on_endstop_hit);
  #endif
  #if ENABLED(SCARA)
    MENU_ITEM_EDIT(float74, msg_xscale(), &axis_scaling[X_AXIS], 0.5, 2);
    MENU_ITEM_EDIT(float74, msg_yscale(), &axis_scaling[Y_AXIS], 0.5, 2);
  #endif
  END_MENU();
}

/**
 *
 * "Control" > "Filament" submenu
 *
 */
static void lcd_control_volumetric_menu() {
  START_MENU();
  MENU_ITEM(back, msg_control(), lcd_control_menu);

  MENU_ITEM_EDIT_CALLBACK(bool, msg_volumetric_enabled(), &volumetric_enabled, calculate_volumetric_multipliers);

  if (volumetric_enabled) {
    #if EXTRUDERS == 1
      MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, msg_filament_diam(), &filament_size[0], 1.5, 3.25, calculate_volumetric_multipliers);
    #else //EXTRUDERS > 1
      MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, msg_filament_diam_diam_e1(), &filament_size[0], 1.5, 3.25, calculate_volumetric_multipliers);
      MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, msg_filament_diam_diam_e2(), &filament_size[1], 1.5, 3.25, calculate_volumetric_multipliers);
      #if EXTRUDERS > 2
        MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, msg_filament_diam_diam_e3(), &filament_size[2], 1.5, 3.25, calculate_volumetric_multipliers);
        #if EXTRUDERS > 3
          MENU_MULTIPLIER_ITEM_EDIT_CALLBACK(float43, msg_filament_diam_diam_e4(), &filament_size[3], 1.5, 3.25, calculate_volumetric_multipliers);
        #endif //EXTRUDERS > 3
      #endif //EXTRUDERS > 2
    #endif //EXTRUDERS > 1
  }

  END_MENU();
}

/**
 *
 * "Control" > "Contrast" submenu
 *
 */
#if ENABLED(HAS_LCD_CONTRAST)
  static void lcd_set_contrast() {
    if (encoderPosition != 0) {
      #if ENABLED(U8GLIB_LM6059_AF)
        lcd_contrast += encoderPosition;
        lcd_contrast &= 0xFF;
      #else
        lcd_contrast -= encoderPosition;
        lcd_contrast &= 0x3F;
      #endif
      encoderPosition = 0;
      lcdDrawUpdate = 1;
      u8g.setContrast(lcd_contrast);
    }
    if (lcdDrawUpdate) {
      #if ENABLED(U8GLIB_LM6059_AF)
        lcd_implementation_drawedit(msg_contrast(), itostr3(lcd_contrast));
      #else
        lcd_implementation_drawedit(msg_contrast(), itostr2(lcd_contrast));
      #endif
    }
    if (LCD_CLICKED) lcd_goto_menu(lcd_control_menu);
  }
#endif // HAS_LCD_CONTRAST

/**
 *
 * "Control" > "Retract" submenu
 *
 */
#if ENABLED(FWRETRACT)
  static void lcd_control_retract_menu() {
    START_MENU();
    MENU_ITEM(back, msg_control(), lcd_control_menu);
    MENU_ITEM_EDIT(bool, msg_autoretract(), &autoretract_enabled);
    MENU_ITEM_EDIT(float52, msg_control_retract(), &retract_length, 0, 100);
    #if EXTRUDERS > 1
      MENU_ITEM_EDIT(float52, msg_control_retract_swap(), &retract_length_swap, 0, 100);
    #endif
    MENU_ITEM_EDIT(float3, msg_control_retractf(), &retract_feedrate, 1, 999);
    MENU_ITEM_EDIT(float52, msg_control_retract_zlift(), &retract_zlift, 0, 999);
    MENU_ITEM_EDIT(float52, msg_control_retract_recover(), &retract_recover_length, 0, 100);
    #if EXTRUDERS > 1
      MENU_ITEM_EDIT(float52, msg_control_retract_recover_swap(), &retract_recover_length_swap, 0, 100);
    #endif
    MENU_ITEM_EDIT(float3, msg_control_retract_recoverf(), &retract_recover_feedrate, 1, 999);
    END_MENU();
  }
#endif // FWRETRACT

#if ENABLED(SDSUPPORT)

  #if !PIN_EXISTS(SD_DETECT)
    static void lcd_sd_refresh() {
      card.initsd();
      currentMenuViewOffset = 0;
    }
  #endif

  static void lcd_sd_updir() {
    card.updir();
    currentMenuViewOffset = 0;
  }

  /**
   *
   * "Print from SD" submenu
   *
   */
  void lcd_sdcard_menu() {
    if (lcdDrawUpdate == 0 && LCD_CLICKED == 0) return; // nothing to do (so don't thrash the SD card)
    uint16_t fileCnt = card.getnrfilenames();
    START_MENU();
    MENU_ITEM(back, msg_main(), lcd_main_menu);
    card.getWorkDirName();
    if (card.filename[0] == '/') {
      #if !PIN_EXISTS(SD_DETECT)
        MENU_ITEM(function,PSTR(LCD_STR_REFRESH MSG_REFRESH), lcd_sd_refresh);
      #endif
    }
    else {
      MENU_ITEM(function,PSTR(LCD_STR_FOLDER ".."), lcd_sd_updir);
    }

    for (uint16_t i = 0; i < fileCnt; i++) {
      if (_menuItemNr == _lineNr) {
        card.getfilename(
          #if ENABLED(SDCARD_RATHERRECENTFIRST)
            fileCnt-1 -
          #endif
          i
        );
        if (card.filenameIsDir)
          MENU_ITEM(sddirectory, msg_card_menu(), card.filename, card.longFilename);
        else
          MENU_ITEM(sdfile, msg_card_menu(), card.filename, card.longFilename);
      }
      else {
        MENU_ITEM_DUMMY();
      }
    }
    END_MENU();
  }

#endif //SDSUPPORT

/**
 *
 * Functions for editing single values
 *
 */
#define menu_edit_type(_type, _name, _strFunc, scale) \
  bool _menu_edit_ ## _name () { \
    bool isClicked = LCD_CLICKED; \
    if ((int32_t)encoderPosition < 0) encoderPosition = 0; \
    if ((int32_t)encoderPosition > maxEditValue) encoderPosition = maxEditValue; \
    if (lcdDrawUpdate) \
      lcd_implementation_drawedit(editLabel, _strFunc(((_type)((int32_t)encoderPosition + minEditValue)) / scale)); \
    if (isClicked) { \
      *((_type*)editValue) = ((_type)((int32_t)encoderPosition + minEditValue)) / scale; \
      lcd_goto_menu(prevMenu, prevEncoderPosition); \
    } \
    return isClicked; \
  } \
  void menu_edit_ ## _name () { _menu_edit_ ## _name(); } \
  void menu_edit_callback_ ## _name () { if (_menu_edit_ ## _name ()) (*callbackFunc)(); } \
  static void _menu_action_setting_edit_ ## _name (const char* pstr, _type* ptr, _type minValue, _type maxValue) { \
    prevMenu = currentMenu; \
    prevEncoderPosition = encoderPosition; \
    \
    lcdDrawUpdate = 2; \
    currentMenu = menu_edit_ ## _name; \
    \
    editLabel = pstr; \
    editValue = ptr; \
    minEditValue = minValue * scale; \
    maxEditValue = maxValue * scale - minEditValue; \
    encoderPosition = (*ptr) * scale - minEditValue; \
  } \
  static void menu_action_setting_edit_ ## _name (const char* pstr, _type* ptr, _type minValue, _type maxValue) { \
    _menu_action_setting_edit_ ## _name(pstr, ptr, minValue, maxValue); \
    currentMenu = menu_edit_ ## _name; \
  }\
  static void menu_action_setting_edit_callback_ ## _name (const char* pstr, _type* ptr, _type minValue, _type maxValue, menuFunc_t callback) { \
    _menu_action_setting_edit_ ## _name(pstr, ptr, minValue, maxValue); \
    currentMenu = menu_edit_callback_ ## _name; \
    callbackFunc = callback; \
  }
menu_edit_type(int, int3, itostr3, 1)
menu_edit_type(float, float3, ftostr3, 1)
menu_edit_type(float, float32, ftostr32, 100)
menu_edit_type(float, float43, ftostr43, 1000)
menu_edit_type(float, float5, ftostr5, 0.01)
menu_edit_type(float, float51, ftostr51, 10)
menu_edit_type(float, float52, ftostr52, 100)
menu_edit_type(unsigned long, long5, ftostr5, 0.01)

/**
 *
 * Handlers for RepRap World Keypad input
 *
 */
#if ENABLED(REPRAPWORLD_KEYPAD)
  static void reprapworld_keypad_move_z_up() {
    encoderPosition = 1;
    move_menu_scale = REPRAPWORLD_KEYPAD_MOVE_STEP;
    lcd_move_z();
  }
  static void reprapworld_keypad_move_z_down() {
    encoderPosition = -1;
    move_menu_scale = REPRAPWORLD_KEYPAD_MOVE_STEP;
    lcd_move_z();
  }
  static void reprapworld_keypad_move_x_left() {
    encoderPosition = -1;
    move_menu_scale = REPRAPWORLD_KEYPAD_MOVE_STEP;
    lcd_move_x();
  }
  static void reprapworld_keypad_move_x_right() {
    encoderPosition = 1;
    move_menu_scale = REPRAPWORLD_KEYPAD_MOVE_STEP;
    lcd_move_x();
  }
  static void reprapworld_keypad_move_y_down() {
    encoderPosition = 1;
    move_menu_scale = REPRAPWORLD_KEYPAD_MOVE_STEP;
    lcd_move_y();
  }
  static void reprapworld_keypad_move_y_up() {
    encoderPosition = -1;
    move_menu_scale = REPRAPWORLD_KEYPAD_MOVE_STEP;
    lcd_move_y();
  }
  static void reprapworld_keypad_move_home() {
    enqueuecommands_P((PSTR("G28"))); // move all axis home
  }
#endif // REPRAPWORLD_KEYPAD


/**
 *
 * Audio feedback for controller clicks
 *
 */

#if ENABLED(LCD_USE_I2C_BUZZER)
  void lcd_buzz(long duration, uint16_t freq) { // called from buzz() in Marlin_main.cpp where lcd is unknown
    lcd.buzz(duration, freq);
  }
#endif

void lcd_quick_feedback() {
  lcdDrawUpdate = 2;
  next_button_update_ms = millis() + 500;

  #if ENABLED(LCD_USE_I2C_BUZZER)
    #ifndef LCD_FEEDBACK_FREQUENCY_HZ
      #define LCD_FEEDBACK_FREQUENCY_HZ 100
    #endif
    #ifndef LCD_FEEDBACK_FREQUENCY_DURATION_MS
      #define LCD_FEEDBACK_FREQUENCY_DURATION_MS (1000/6)
    #endif
    lcd.buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
  #elif PIN_EXISTS(BEEPER)
    #ifndef LCD_FEEDBACK_FREQUENCY_HZ
      #define LCD_FEEDBACK_FREQUENCY_HZ 5000
    #endif
    #ifndef LCD_FEEDBACK_FREQUENCY_DURATION_MS
      #define LCD_FEEDBACK_FREQUENCY_DURATION_MS 2
    #endif
    buzz(LCD_FEEDBACK_FREQUENCY_DURATION_MS, LCD_FEEDBACK_FREQUENCY_HZ);
  #else
    #ifndef LCD_FEEDBACK_FREQUENCY_DURATION_MS
      #define LCD_FEEDBACK_FREQUENCY_DURATION_MS 2
    #endif
    delay(LCD_FEEDBACK_FREQUENCY_DURATION_MS);
  #endif
}

/**
 *
 * Menu actions
 *
 */
static void menu_action_back(menuFunc_t func) { lcd_goto_menu(func); }
static void menu_action_submenu(menuFunc_t func) { lcd_goto_menu(func); }
static void menu_action_gcode(const char* pgcode) { enqueuecommands_P(pgcode); }
static void menu_action_function(menuFunc_t func) { (*func)(); }

#if ENABLED(SDSUPPORT)

  static void menu_action_sdfile(const char* filename, char* longFilename) {
    char cmd[30];
    char* c;
    sprintf_P(cmd, PSTR("M23 %s"), filename);
    for (c = &cmd[4]; *c; c++) *c = tolower(*c);
    enqueuecommand(cmd);
    enqueuecommands_P(PSTR("M24"));
    lcd_return_to_status();
  }

  static void menu_action_sddirectory(const char* filename, char* longFilename) {
    card.chdir(filename);
    encoderPosition = 0;
  }

#endif //SDSUPPORT

static void menu_action_setting_edit_bool(const char* pstr, bool* ptr) { *ptr = !(*ptr); }
static void menu_action_setting_edit_callback_bool(const char* pstr, bool* ptr, menuFunc_t callback) {
  menu_action_setting_edit_bool(pstr, ptr);
  (*callback)();
}

#endif //ULTIPANEL

/** LCD API **/
void lcd_init() {

  lcd_implementation_init();

  #if ENABLED(NEWPANEL)

    SET_INPUT(BTN_EN1);
    SET_INPUT(BTN_EN2);
    WRITE(BTN_EN1, HIGH);
    WRITE(BTN_EN2, HIGH);

    #if BTN_ENC > 0
      SET_INPUT(BTN_ENC);
      WRITE(BTN_ENC, HIGH);
    #endif

    #if ENABLED(REPRAPWORLD_KEYPAD)
      pinMode(SHIFT_CLK, OUTPUT);
      pinMode(SHIFT_LD, OUTPUT);
      pinMode(SHIFT_OUT, INPUT);
      WRITE(SHIFT_OUT, HIGH);
      WRITE(SHIFT_LD, HIGH);
    #endif

  #else  // Not NEWPANEL

    #if ENABLED(SR_LCD_2W_NL) // Non latching 2 wire shift register
      pinMode(SR_DATA_PIN, OUTPUT);
      pinMode(SR_CLK_PIN, OUTPUT);
    #elif defined(SHIFT_CLK)
      pinMode(SHIFT_CLK, OUTPUT);
      pinMode(SHIFT_LD, OUTPUT);
      pinMode(SHIFT_EN, OUTPUT);
      pinMode(SHIFT_OUT, INPUT);
      WRITE(SHIFT_OUT, HIGH);
      WRITE(SHIFT_LD, HIGH);
      WRITE(SHIFT_EN, LOW);
    #endif // SR_LCD_2W_NL

  #endif//!NEWPANEL

  #if ENABLED(SDSUPPORT) && PIN_EXISTS(SD_DETECT)
    pinMode(SD_DETECT_PIN, INPUT);
    WRITE(SD_DETECT_PIN, LOW);
    lcd_sd_status = 2; // UNKNOWN
  #endif

  #if ENABLED(LCD_HAS_SLOW_BUTTONS)
    slow_buttons = 0;
  #endif

  lcd_buttons_update();

  #if ENABLED(ULTIPANEL)
    encoderDiff = 0;
  #endif
}

int lcd_strlen(char* s) {
  int i = 0, j = 0;
  while (s[i]) {
    if ((s[i] & 0xc0) != 0x80) j++;
    i++;
  }
  return j;
}

int lcd_strlen_P(const char* s) {
  int j = 0;
  while (pgm_read_byte(s)) {
    if ((pgm_read_byte(s) & 0xc0) != 0x80) j++;
    s++;
  }
  return j;
}

/**
 * Update the LCD, read encoder buttons, etc.
 *   - Read button states
 *   - Check the SD Card slot state
 *   - Act on RepRap World keypad input
 *   - Update the encoder position
 *   - Apply acceleration to the encoder position
 *   - Reset the Info Screen timeout if there's any input
 *   - Update status indicators, if any
 *   - Clear the LCD if lcdDrawUpdate == 2
 *
 * Warning: This function is called from interrupt context!
 */
bool poweroff;
//������
uint8_t checknum=0;
void lcd_update() {

//	if(millis()-oldtime>500)
//	{
	//����Դ�����Ƿ���
	//	poweroff=READ(IN_PUT);
	//	if(0==poweroff)
		//WRITE(OUT_PUT,HIGH);
//	}
	//�������Ƿ���
	if(true==card.sdprinting )
	{
		if(1==READ(CHECK_MATWEIAL))
		{
			checknum++;
			if(checknum>10)
			{
			//������ͷ���ȴ�����
			lcd_sdcard_pause();
		    LCD_MESSAGEPGM("Err: Change Filament");
			//���¼���
			checknum=0;
			}
		}
	}
  #if ENABLED(ULTIPANEL)
    static millis_t return_to_status_ms = 0;
  #endif
    static millis_t led_auto_off = 0;
  lcd_buttons_update();

  #if ENABLED(SDSUPPORT) && ENABLED(POWEROFF_SAVE_SD_FILE)
    if ((power_off_type_yes == 0) && (power_off_commands_count > 0)) {
      currentMenu = lcd_resume_ok_menu;
    }
  #endif

  #if ENABLED(SDSUPPORT) && PIN_EXISTS(SD_DETECT)

    bool sd_status = IS_SD_INSERTED;
    if (sd_status != lcd_sd_status && lcd_detected()) {
      lcdDrawUpdate = 2;
      lcd_implementation_init( // to maybe revive the LCD if static electricity killed it.
        #if ENABLED(LCD_PROGRESS_BAR)
          currentMenu == lcd_status_screen
        #endif
      );

      if (sd_status) {
        card.initsd();
        if (lcd_sd_status != 2) lcd_setstatuspgm(msg_sd_inserted());
        
        #if ENABLED(SDSUPPORT) && ENABLED(POWEROFF_SAVE_SD_FILE)
        init_power_off_info();
        #endif
      }
      else {
        card.release();
        if (lcd_sd_status != 2) lcd_setstatuspgm(msg_sd_removed());;
      }

      lcd_sd_status = sd_status;
    }

  #endif //SDSUPPORT && SD_DETECT_PIN

  millis_t ms = millis();
  if (ms > next_lcd_update_ms) {

    #if ENABLED(LCD_HAS_SLOW_BUTTONS)
      slow_buttons = lcd_implementation_read_slow_buttons(); // buttons which take too long to read in interrupt context
    #endif

    #if ENABLED(ULTIPANEL)

      #if ENABLED(REPRAPWORLD_KEYPAD)
        if (REPRAPWORLD_KEYPAD_MOVE_Z_UP)     reprapworld_keypad_move_z_up();
        if (REPRAPWORLD_KEYPAD_MOVE_Z_DOWN)   reprapworld_keypad_move_z_down();
        if (REPRAPWORLD_KEYPAD_MOVE_X_LEFT)   reprapworld_keypad_move_x_left();
        if (REPRAPWORLD_KEYPAD_MOVE_X_RIGHT)  reprapworld_keypad_move_x_right();
        if (REPRAPWORLD_KEYPAD_MOVE_Y_DOWN)   reprapworld_keypad_move_y_down();
        if (REPRAPWORLD_KEYPAD_MOVE_Y_UP)     reprapworld_keypad_move_y_up();
        if (REPRAPWORLD_KEYPAD_MOVE_HOME)     reprapworld_keypad_move_home();
      #endif

      bool encoderPastThreshold = (abs(encoderDiff) >= ENCODER_PULSES_PER_STEP);
      if (encoderPastThreshold || LCD_CLICKED) {
        if (encoderPastThreshold) {
          int32_t encoderMultiplier = 1;

          #if ENABLED(ENCODER_RATE_MULTIPLIER)

            if (encoderRateMultiplierEnabled) {
              int32_t encoderMovementSteps = abs(encoderDiff) / ENCODER_PULSES_PER_STEP;

              if (lastEncoderMovementMillis != 0) {
                // Note that the rate is always calculated between to passes through the 
                // loop and that the abs of the encoderDiff value is tracked.
                float encoderStepRate = (float)(encoderMovementSteps) / ((float)(ms - lastEncoderMovementMillis)) * 1000.0;

                if (encoderStepRate >= ENCODER_100X_STEPS_PER_SEC)     encoderMultiplier = 100;
                else if (encoderStepRate >= ENCODER_10X_STEPS_PER_SEC) encoderMultiplier = 10;

                #if ENABLED(ENCODER_RATE_MULTIPLIER_DEBUG)
                  SERIAL_ECHO_START;
                  SERIAL_ECHO("Enc Step Rate: ");
                  SERIAL_ECHO(encoderStepRate);
                  SERIAL_ECHO("  Multiplier: ");
                  SERIAL_ECHO(encoderMultiplier);
                  SERIAL_ECHO("  ENCODER_10X_STEPS_PER_SEC: ");
                  SERIAL_ECHO(ENCODER_10X_STEPS_PER_SEC);
                  SERIAL_ECHO("  ENCODER_100X_STEPS_PER_SEC: ");
                  SERIAL_ECHOLN(ENCODER_100X_STEPS_PER_SEC);
                #endif //ENCODER_RATE_MULTIPLIER_DEBUG
              }

              lastEncoderMovementMillis = ms;
            } // encoderRateMultiplierEnabled
          #endif //ENCODER_RATE_MULTIPLIER

          encoderPosition += (encoderDiff * encoderMultiplier) / ENCODER_PULSES_PER_STEP;
          encoderDiff = 0;
        }
        return_to_status_ms = ms + LCD_TIMEOUT_TO_STATUS;
        led_auto_off = ms + led_off_time ;
        lcdDrawUpdate = 1;
      }
    #endif //ULTIPANEL

    if (currentMenu == lcd_status_screen) {
      if (!lcd_status_update_delay) {
        lcdDrawUpdate = 1;
        lcd_status_update_delay = 10;   /* redraw the main screen every second. This is easier then trying keep track of all things that change on the screen */
      }
      else {
        lcd_status_update_delay--;
      }
    }
    #if ENABLED(DOGLCD)  // Changes due to different driver architecture of the DOGM display
        if (lcdDrawUpdate) {
          blink++;     // Variable for fan animation and alive dot
          u8g.firstPage();
          do {
            lcd_setFont(FONT_MENU);
            u8g.setPrintPos(125, 0);
            if (blink % 2) u8g.setColorIndex(1); else u8g.setColorIndex(0); // Set color for the alive dot
            u8g.drawPixel(127, 63); // draw alive dot
            u8g.setColorIndex(1); // black on white
            (*currentMenu)();
          } while (u8g.nextPage());
        }
    #else
      if (lcdDrawUpdate)
        (*currentMenu)();
    #endif

    #if ENABLED(LCD_HAS_STATUS_INDICATORS)
      lcd_implementation_update_indicators();
    #endif

    #if ENABLED(ULTIPANEL)

      // Return to Status Screen after a timeout
      if (currentMenu != lcd_status_screen &&
        #if ENABLED(MANUAL_BED_LEVELING)
          currentMenu != _lcd_level_bed &&
          currentMenu != _lcd_level_bed_homing &&
        #endif
        millis() > return_to_status_ms
      ) {
        lcd_return_to_status();
        lcdDrawUpdate = 2;
      }

    #endif // ULTIPANEL

        if ( millis() > led_auto_off)
         {
              //lcd_return_to_status();
             // lcdDrawUpdate = 2;
              digitalWrite (LED,LOW);
              
         }

      
    if (lcdDrawUpdate == 2) lcd_implementation_clear();
    if (lcdDrawUpdate) lcdDrawUpdate--;
    next_lcd_update_ms = ms + LCD_UPDATE_INTERVAL;
  }
}

void lcd_ignore_click(bool b) {
  ignore_click = b;
  wait_for_unclick = false;
}

void lcd_finishstatus(bool persist=false) {
  #if ENABLED(LCD_PROGRESS_BAR)
    progress_bar_ms = millis();
    #if PROGRESS_MSG_EXPIRE > 0
      expire_status_ms = persist ? 0 : progress_bar_ms + PROGRESS_MSG_EXPIRE;
    #endif
  #endif
  lcdDrawUpdate = 2;

  #if ENABLED(FILAMENT_LCD_DISPLAY)
    previous_lcd_status_ms = millis();  //get status message to show up for a while
  #endif
}

#if ENABLED(LCD_PROGRESS_BAR) && PROGRESS_MSG_EXPIRE > 0
  void dontExpireStatus() { expire_status_ms = 0; }
#endif

void set_utf_strlen(char* s, uint8_t n) {
  uint8_t i = 0, j = 0;
  while (s[i] && (j < n)) {
    if ((s[i] & 0xc0u) != 0x80u) j++;
    i++;
  }
  while (j++ < n) s[i++] = ' ';
  s[i] = 0;
}

bool lcd_hasstatus() { return (lcd_status_message[0] != '\0'); }

void lcd_setstatus(const char* message, bool persist) {
  if (lcd_status_message_level > 0) return;
  strncpy(lcd_status_message, message, 3 * LCD_WIDTH);
  set_utf_strlen(lcd_status_message, LCD_WIDTH);
  lcd_finishstatus(persist);
}

void lcd_setstatuspgm(const char* message, uint8_t level) {
  if (level >= lcd_status_message_level) {
    strncpy_P(lcd_status_message, message, 3 * LCD_WIDTH);
    set_utf_strlen(lcd_status_message, LCD_WIDTH);
    lcd_status_message_level = level;
    lcd_finishstatus(level > 0);
  }
}

void lcd_setalertstatuspgm(const char* message) {
  lcd_setstatuspgm(message, 1);
  #if ENABLED(ULTIPANEL)
    lcd_return_to_status();
  #endif
}

void lcd_reset_alert_level() { lcd_status_message_level = 0; }

#if ENABLED(HAS_LCD_CONTRAST)
  void lcd_setcontrast(uint8_t value) {
    lcd_contrast = value & 0x3F;
    u8g.setContrast(lcd_contrast);
  }
#endif

#if ENABLED(ULTIPANEL)

  /**
   * Setup Rotary Encoder Bit Values (for two pin encoders to indicate movement)
   * These values are independent of which pins are used for EN_A and EN_B indications
   * The rotary encoder part is also independent to the chipset used for the LCD
   */
  #if defined(EN_A) && defined(EN_B)
    #define encrot0 0
    #define encrot1 2
    #define encrot2 3
    #define encrot3 1
  #endif

  /**
   * Read encoder buttons from the hardware registers
   * Warning: This function is called from interrupt context!
   */
  void lcd_buttons_update() {
    #if ENABLED(NEWPANEL)
      uint8_t newbutton = 0;
      if (READ(BTN_EN1) == 0) newbutton |= EN_A;
      if (READ(BTN_EN2) == 0) newbutton |= EN_B;
      #if BTN_ENC > 0
        if (millis() > next_button_update_ms && READ(BTN_ENC) == 0) newbutton |= EN_C;
      #endif
      buttons = newbutton;
      #if ENABLED(LCD_HAS_SLOW_BUTTONS)
        buttons |= slow_buttons;
      #endif
      #if ENABLED(REPRAPWORLD_KEYPAD)
        // for the reprapworld_keypad
        uint8_t newbutton_reprapworld_keypad = 0;
        WRITE(SHIFT_LD, LOW);
        WRITE(SHIFT_LD, HIGH);
        for (int8_t i = 0; i < 8; i++) {
          newbutton_reprapworld_keypad >>= 1;
          if (READ(SHIFT_OUT)) newbutton_reprapworld_keypad |= BIT(7);
          WRITE(SHIFT_CLK, HIGH);
          WRITE(SHIFT_CLK, LOW);
        }
        buttons_reprapworld_keypad = ~newbutton_reprapworld_keypad; //invert it, because a pressed switch produces a logical 0
      #endif
    #else   //read it from the shift register
      uint8_t newbutton = 0;
      WRITE(SHIFT_LD, LOW);
      WRITE(SHIFT_LD, HIGH);
      unsigned char tmp_buttons = 0;
      for (int8_t i = 0; i < 8; i++) {
        newbutton >>= 1;
        if (READ(SHIFT_OUT)) newbutton |= BIT(7);
        WRITE(SHIFT_CLK, HIGH);
        WRITE(SHIFT_CLK, LOW);
      }
      buttons = ~newbutton; //invert it, because a pressed switch produces a logical 0
    #endif //!NEWPANEL

    //manage encoder rotation
    uint8_t enc = 0;
    if (buttons & EN_A) enc |= B01;
    if (buttons & EN_B) enc |= B10;
    if (enc != lastEncoderBits) {
      switch (enc) {
        case encrot0:
          if (lastEncoderBits == encrot3) encoderDiff++;
          else if (lastEncoderBits == encrot1) encoderDiff--;
          break;
        case encrot1:
          if (lastEncoderBits == encrot0) encoderDiff++;
          else if (lastEncoderBits == encrot2) encoderDiff--;
          break;
        case encrot2:
          if (lastEncoderBits == encrot1) encoderDiff++;
          else if (lastEncoderBits == encrot3) encoderDiff--;
          break;
        case encrot3:
          if (lastEncoderBits == encrot2) encoderDiff++;
          else if (lastEncoderBits == encrot0) encoderDiff--;
          break;
      }
    }
    lastEncoderBits = enc;
  }

  bool lcd_detected(void) {
    #if (ENABLED(LCD_I2C_TYPE_MCP23017) || ENABLED(LCD_I2C_TYPE_MCP23008)) && ENABLED(DETECT_DEVICE)
      return lcd.LcdDetected() == 1;
    #else
      return true;
    #endif
  }

  bool lcd_clicked() { return LCD_CLICKED; }

#endif // ULTIPANEL

/*********************************/
/** Number to string conversion **/
/*********************************/

char conv[8];

// Convert float to rj string with 123 or -12 format
char *ftostr3(const float& x) { return itostr3((int)x); }

// Convert float to rj string with _123, -123, _-12, or __-1 format
char *ftostr4sign(const float& x) { return itostr4sign((int)x); }

// Convert int to string with 12 format
char* itostr2(const uint8_t& x) {
  //sprintf(conv,"%5.1f",x);
  int xx = x;
  conv[0] = (xx / 10) % 10 + '0';
  conv[1] = xx % 10 + '0';
  conv[2] = 0;
  return conv;
}

// Convert float to string with +123.4 format
char* ftostr31(const float& x) {
  int xx = abs(x * 10);
  conv[0] = (x >= 0) ? '+' : '-';
  conv[1] = (xx / 1000) % 10 + '0';
  conv[2] = (xx / 100) % 10 + '0';
  conv[3] = (xx / 10) % 10 + '0';
  conv[4] = '.';
  conv[5] = xx % 10 + '0';
  conv[6] = 0;
  return conv;
}

// Convert float to string with 123.4 format, dropping sign
char* ftostr31ns(const float& x) {
  int xx = abs(x * 10);
  conv[0] = (xx / 1000) % 10 + '0';
  conv[1] = (xx / 100) % 10 + '0';
  conv[2] = (xx / 10) % 10 + '0';
  conv[3] = '.';
  conv[4] = xx % 10 + '0';
  conv[5] = 0;
  return conv;
}

// Convert float to string with 123.45 format
char *ftostr32(const float& x) {
  long xx = abs(x * 100);
  conv[0] = x >= 0 ? (xx / 10000) % 10 + '0' : '-';
  conv[1] = (xx / 1000) % 10 + '0';
  conv[2] = (xx / 100) % 10 + '0';
  conv[3] = '.';
  conv[4] = (xx / 10) % 10 + '0';
  conv[5] = xx % 10 + '0';
  conv[6] = 0;
  return conv;
}

// Convert float to string with 1.234 format
char* ftostr43(const float& x) {
  long xx = x * 1000;
  if (xx >= 0)
    conv[0] = (xx / 1000) % 10 + '0';
  else
    conv[0] = '-';
  xx = abs(xx);
  conv[1] = '.';
  conv[2] = (xx / 100) % 10 + '0';
  conv[3] = (xx / 10) % 10 + '0';
  conv[4] = (xx) % 10 + '0';
  conv[5] = 0;
  return conv;
}

// Convert float to string with 1.23 format
char* ftostr12ns(const float& x) {
  long xx = x * 100;
  xx = abs(xx);
  conv[0] = (xx / 100) % 10 + '0';
  conv[1] = '.';
  conv[2] = (xx / 10) % 10 + '0';
  conv[3] = (xx) % 10 + '0';
  conv[4] = 0;
  return conv;
}

// Convert float to space-padded string with -_23.4_ format
char* ftostr32sp(const float& x) {
  long xx = abs(x * 100);
  uint8_t dig;
  if (x < 0) { // negative val = -_0
    conv[0] = '-';
    dig = (xx / 1000) % 10;
    conv[1] = dig ? '0' + dig : ' ';
  }
  else { // positive val = __0
    dig = (xx / 10000) % 10;
    if (dig) {
      conv[0] = '0' + dig;
      conv[1] = '0' + (xx / 1000) % 10;
    }
    else {
      conv[0] = ' ';
      dig = (xx / 1000) % 10;
      conv[1] = dig ? '0' + dig : ' ';
    }
  }

  conv[2] = '0' + (xx / 100) % 10; // lsd always

  dig = xx % 10;
  if (dig) { // 2 decimal places
    conv[5] = '0' + dig;
    conv[4] = '0' + (xx / 10) % 10;
    conv[3] = '.';
  }
  else { // 1 or 0 decimal place
    dig = (xx / 10) % 10;
    if (dig) {
      conv[4] = '0' + dig;
      conv[3] = '.';
    }
    else {
      conv[3] = conv[4] = ' ';
    }
    conv[5] = ' ';
  }
  conv[6] = '\0';
  return conv;
}

// Convert int to lj string with +123.0 format
char* itostr31(const int& x) {
  conv[0] = x >= 0 ? '+' : '-';
  int xx = abs(x);
  conv[1] = (xx / 100) % 10 + '0';
  conv[2] = (xx / 10) % 10 + '0';
  conv[3] = xx % 10 + '0';
  conv[4] = '.';
  conv[5] = '0';
  conv[6] = 0;
  return conv;
}

// Convert int to rj string with 123 or -12 format
char* itostr3(const int& x) {
  int xx = x;
  if (xx < 0) {
    conv[0] = '-';
    xx = -xx;
  }
  else
    conv[0] = xx >= 100 ? (xx / 100) % 10 + '0' : ' ';

  conv[1] = xx >= 10 ? (xx / 10) % 10 + '0' : ' ';
  conv[2] = xx % 10 + '0';
  conv[3] = 0;
  return conv;
}

// Convert int to lj string with 123 format
char* itostr3left(const int& xx) {
  if (xx >= 100) {
    conv[0] = (xx / 100) % 10 + '0';
    conv[1] = (xx / 10) % 10 + '0';
    conv[2] = xx % 10 + '0';
    conv[3] = 0;
  }
  else if (xx >= 10) {
    conv[0] = (xx / 10) % 10 + '0';
    conv[1] = xx % 10 + '0';
    conv[2] = 0;
  }
  else {
    conv[0] = xx % 10 + '0';
    conv[1] = 0;
  }
  return conv;
}

// Convert int to rj string with 1234 format
char* itostr4(const int& xx) {
  conv[0] = xx >= 1000 ? (xx / 1000) % 10 + '0' : ' ';
  conv[1] = xx >= 100 ? (xx / 100) % 10 + '0' : ' ';
  conv[2] = xx >= 10 ? (xx / 10) % 10 + '0' : ' ';
  conv[3] = xx % 10 + '0';
  conv[4] = 0;
  return conv;
}

// Convert int to rj string with _123, -123, _-12, or __-1 format
char *itostr4sign(const int& x) {
  int xx = abs(x);
  int sign = 0;
  if (xx >= 100) {
    conv[1] = (xx / 100) % 10 + '0';
    conv[2] = (xx / 10) % 10 + '0';
  }
  else if (xx >= 10) {
    conv[0] = ' ';
    sign = 1;
    conv[2] = (xx / 10) % 10 + '0';
  }
  else {
    conv[0] = ' ';
    conv[1] = ' ';
    sign = 2;
  }
  conv[sign] = x < 0 ? '-' : ' ';
  conv[3] = xx % 10 + '0';
  conv[4] = 0;
  return conv;
}

// Convert float to rj string with 12345 format
char* ftostr5(const float& x) {
  long xx = abs(x);
  conv[0] = xx >= 10000 ? (xx / 10000) % 10 + '0' : ' ';
  conv[1] = xx >= 1000 ? (xx / 1000) % 10 + '0' : ' ';
  conv[2] = xx >= 100 ? (xx / 100) % 10 + '0' : ' ';
  conv[3] = xx >= 10 ? (xx / 10) % 10 + '0' : ' ';
  conv[4] = xx % 10 + '0';
  conv[5] = 0;
  return conv;
}

// Convert float to string with +1234.5 format
char* ftostr51(const float& x) {
  long xx = abs(x * 10);
  conv[0] = (x >= 0) ? '+' : '-';
  conv[1] = (xx / 10000) % 10 + '0';
  conv[2] = (xx / 1000) % 10 + '0';
  conv[3] = (xx / 100) % 10 + '0';
  conv[4] = (xx / 10) % 10 + '0';
  conv[5] = '.';
  conv[6] = xx % 10 + '0';
  conv[7] = 0;
  return conv;
}

// Convert float to string with +123.45 format
char* ftostr52(const float& x) {
  conv[0] = (x >= 0) ? '+' : '-';
  long xx = abs(x * 100);
  conv[1] = (xx / 10000) % 10 + '0';
  conv[2] = (xx / 1000) % 10 + '0';
  conv[3] = (xx / 100) % 10 + '0';
  conv[4] = '.';
  conv[5] = (xx / 10) % 10 + '0';
  conv[6] = xx % 10 + '0';
  conv[7] = 0;
  return conv;
}

#if ENABLED(MANUAL_BED_LEVELING)

  static int _lcd_level_bed_position;

  /**
   * MBL Wait for controller movement and clicks:
   *   - Movement adjusts the Z axis
   *   - Click saves the Z and goes to the next mesh point
   */
  static void _lcd_level_bed() {
    if (encoderPosition != 0) {
      refresh_cmd_timeout();
      current_position[Z_AXIS] += float((int)encoderPosition) * MBL_Z_STEP;
      if (min_software_endstops && current_position[Z_AXIS] < Z_MIN_POS) current_position[Z_AXIS] = Z_MIN_POS;
      if (max_software_endstops && current_position[Z_AXIS] > Z_MAX_POS) current_position[Z_AXIS] = Z_MAX_POS;
      encoderPosition = 0;
      line_to_current(Z_AXIS);
      lcdDrawUpdate = 2;
    }
    if (lcdDrawUpdate) lcd_implementation_drawedit(PSTR("Z"), ftostr43(current_position[Z_AXIS]));
    static bool debounce_click = false;
    if (LCD_CLICKED) {
      if (!debounce_click) {
        debounce_click = true;
        int ix = _lcd_level_bed_position % MESH_NUM_X_POINTS,
            iy = _lcd_level_bed_position / MESH_NUM_X_POINTS;
        if (iy & 1) ix = (MESH_NUM_X_POINTS - 1) - ix; // Zig zag
        mbl.set_z(ix, iy, current_position[Z_AXIS]);
        _lcd_level_bed_position++;
        if (_lcd_level_bed_position == MESH_NUM_X_POINTS * MESH_NUM_Y_POINTS) {
          current_position[Z_AXIS] = MESH_HOME_SEARCH_Z;
          line_to_current(Z_AXIS);
          mbl.active = 1;
          enqueuecommands_P(PSTR("G28"));
          lcd_return_to_status();
        }
        else {
          current_position[Z_AXIS] = MESH_HOME_SEARCH_Z;
          line_to_current(Z_AXIS);
          ix = _lcd_level_bed_position % MESH_NUM_X_POINTS;
          iy = _lcd_level_bed_position / MESH_NUM_X_POINTS;
          if (iy & 1) ix = (MESH_NUM_X_POINTS - 1) - ix; // Zig zag
          current_position[X_AXIS] = mbl.get_x(ix);
          current_position[Y_AXIS] = mbl.get_y(iy);
          line_to_current(manual_feedrate[X_AXIS] <= manual_feedrate[Y_AXIS] ? X_AXIS : Y_AXIS);
          lcdDrawUpdate = 2;
        }
      }
    }
    else {
      debounce_click = false;
    }
  }

  /**
   * MBL Move to mesh starting point
   */
  static void _lcd_level_bed_homing() {
    if (lcdDrawUpdate) lcd_implementation_drawedit(PSTR("XYZ"), "Homing");
    if (axis_known_position[X_AXIS] && axis_known_position[Y_AXIS] && axis_known_position[Z_AXIS]) {
      current_position[Z_AXIS] = MESH_HOME_SEARCH_Z;
      plan_set_position(current_position[X_AXIS], current_position[Y_AXIS], current_position[Z_AXIS], current_position[E_AXIS]);
      current_position[X_AXIS] = MESH_MIN_X;
      current_position[Y_AXIS] = MESH_MIN_Y;
      line_to_current(manual_feedrate[X_AXIS] <= manual_feedrate[Y_AXIS] ? X_AXIS : Y_AXIS);
      _lcd_level_bed_position = 0;
      lcd_goto_menu(_lcd_level_bed);
    }
    lcdDrawUpdate = 2;
  }

  /**
   * MBL entry-point
   */
  static void lcd_level_bed() {
    axis_known_position[X_AXIS] = axis_known_position[Y_AXIS] = axis_known_position[Z_AXIS] = false;
    mbl.reset();
    enqueuecommands_P(PSTR("G28"));
    lcdDrawUpdate = 2;
    lcd_goto_menu(_lcd_level_bed_homing);
  }

#endif  // MANUAL_BED_LEVELING

#endif // ULTRA_LCD
