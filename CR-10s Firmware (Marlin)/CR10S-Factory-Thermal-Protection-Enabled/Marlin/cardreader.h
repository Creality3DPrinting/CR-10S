#ifndef CARDREADER_H
#define CARDREADER_H

#if ENABLED(SDSUPPORT)

#define MAX_DIR_DEPTH 10          // Maximum folder depth

#include "SdFile.h"
enum LsAction { LS_SerialPrint, LS_Count, LS_GetFilename };

class CardReader {
public:
  CardReader();

  void initsd();
  void write_command(char *buf);
  //files auto[0-9].g on the sd card are performed in a row
  //this is to delay autostart and hence the initialisaiton of the sd card to some seconds after the normal init, so the device is available quick after a reset

  void checkautostart(bool x);
  void openFile(char* name,bool read,bool replace_current=true);
  void openLogFile(char* name);
  #if ENABLED(SDSUPPORT) && ENABLED(POWEROFF_SAVE_SD_FILE)
  void openPowerOffFile(char* name, uint8_t oflag);
  void closePowerOffFile();
  bool existPowerOffFile(char* name);
  int16_t savePowerOffInfo(const void* data, uint16_t size);
  int16_t getPowerOffInfo(void* data, uint16_t size);
  void removePowerOffFile();
  #endif
  void removeFile(char* name);
  void closefile(bool store_location=false);
  void release();
  void startFileprint();
  void pauseSDPrint();
  void getStatus();
  void printingHasFinished();

  #if ENABLED(LONG_FILENAME_HOST_SUPPORT)
    void printLongPath(char *path);
  #endif

  void getfilename(uint16_t nr, const char* const match=NULL);
  uint16_t getnrfilenames();

  void getAbsFilename(char *t);

  void ls();
  void chdir(const char *relpath);
  void updir();
  void setroot();


  FORCE_INLINE bool isFileOpen() { return file.isOpen(); }
  FORCE_INLINE bool eof() { return sdpos >= filesize; }
  FORCE_INLINE int16_t get() { sdpos = file.curPosition(); return (int16_t)file.read(); }
  FORCE_INLINE void setIndex(long index) { sdpos = index; file.seekSet(index); }
  FORCE_INLINE uint32_t getIndex() { return sdpos; }
  //FORCE_INLINE char* getCurrentPrintFileName() { return filenames[file_subcall_ctr]; }
  FORCE_INLINE uint8_t percentDone() { return (isFileOpen() && filesize) ? sdpos / ((filesize + 99) / 100) : 0; }
  FORCE_INLINE char* getWorkDirName() { workDir.getFilename(filename); return filename; }

public:
  bool saving, logging, sdprinting, cardOK, filenameIsDir;
  char filename[FILENAME_LENGTH], longFilename[LONG_FILENAME_LENGTH];
  int autostart_index;
private:
  SdFile root, *curDir, workDir, workDirParents[MAX_DIR_DEPTH];
  uint16_t workDirDepth;
  Sd2Card card;
  SdVolume volume;
  SdFile file;
  #if ENABLED(SDSUPPORT) && ENABLED(POWEROFF_SAVE_SD_FILE)
  SdFile powerOffFile;
  #endif
  #define SD_PROCEDURE_DEPTH 1
  #define MAXPATHNAMELENGTH (FILENAME_LENGTH*MAX_DIR_DEPTH + MAX_DIR_DEPTH + 1)
  uint8_t file_subcall_ctr;
  uint32_t filespos[SD_PROCEDURE_DEPTH];
  char filenames[SD_PROCEDURE_DEPTH][MAXPATHNAMELENGTH];
  uint32_t filesize;
  millis_t next_autostart_ms;
  uint32_t sdpos;

  bool autostart_stilltocheck; //the sd start is delayed, because otherwise the serial cannot answer fast enought to make contact with the hostsoftware.

  LsAction lsAction; //stored for recursion.
  uint16_t nrFiles; //counter for the files in the current directory and recycled as position counter for getting the nrFiles'th name in the directory.
  char* diveDirName;
  void lsDive(const char *prepend, SdFile parent, const char * const match=NULL);
};

extern CardReader card;
#if ENABLED(SDSUPPORT) && ENABLED(POWEROFF_SAVE_SD_FILE)
struct power_off_info_t {
  /* header (1B + 7B = 8B) */
  uint8_t valid_head;
  //uint8_t reserved1[8-1];
  /* Gcode related information. (44B + 20B = 64B) */
  float current_position[NUM_AXIS];
  float feedrate;
  float saved_z;
  int target_temperature[4];
  int target_temperature_bed;
  //uint8_t reserved2[64-44];
  /* print queue related information. (396B + 116B = 512B) */
  int cmd_queue_index_r;
  int cmd_queue_index_w;
  int commands_in_queue;
  char command_queue[BUFSIZE][MAX_CMD_SIZE];
  //uint8_t reserved3[512-396];
  /* SD card related information. (165B + 91B = 256B)*/
  uint32_t sdpos;
  millis_t print_job_start_ms;
  char sd_filename[MAXPATHNAMELENGTH];
  char power_off_filename[16];
  //uint8_t reserved4[256-166];
  uint8_t valid_foot;
};

extern struct power_off_info_t power_off_info;
extern int power_off_commands_count;
extern int power_off_type_yes;
#endif

#define IS_SD_PRINTING (card.sdprinting)

#if PIN_EXISTS(SD_DETECT)
  #if ENABLED(SD_DETECT_INVERTED)
    #define IS_SD_INSERTED (READ(SD_DETECT_PIN) != 0)
  #else
    #define IS_SD_INSERTED (READ(SD_DETECT_PIN) == 0)
  #endif
#else
  //No card detect line? Assume the card is inserted.
  #define IS_SD_INSERTED true
#endif

#else

#define IS_SD_PRINTING (false)

#endif //SDSUPPORT

#endif //__CARDREADER_H
