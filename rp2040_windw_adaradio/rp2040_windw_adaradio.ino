/**************************************************************************
  Check out the links for tutorials and wiring diagrams.

  Adafruit invests time and resources providing this open source code,
  please support Adafruit and open-source hardware by purchasing
  products from Adafruit!

  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/
//#define ESP32_S3_TFT // for ESP32-S3 TFT
//#define HX8357 // for the Adafruit 3.5" TFT (HX8357) FeatherWing
#define USE_MP3
// Note: If you have an error while compiling with USE_MP3:
//  In file included from C:\Users\User\AppData\Local\Arduino15\packages\rp2040\hardware\rp2040\2.7.1\libraries\SD\src/SD.h:24,
//                   from C:\Users\User\AppData\Local\Arduino15\packages\rp2040\hardware\rp2040\2.7.1\libraries\SD\examples\Files\Files.ino:21:
//  C:\Users\User\AppData\Local\Arduino15\packages\rp2040\hardware\rp2040\2.7.1\libraries\SDFS\src/SDFS.h: In member function 'virtual int sdfs::SDFSFileImpl::availableForWrite()':
//  C:\Users\User\AppData\Local\Arduino15\packages\rp2040\hardware\rp2040\2.7.1\libraries\SDFS\src/SDFS.h:279:31: error: 'using element_type = class File32' {aka 'class File32'} has no member named 'availableSpaceForWrite'; did you mean 'availableForWrite'?
//    279 |         return _opened ? _fd->availableSpaceForWrite() : 0;
//        |                               ^~~~~~~~~~~~~~~~~~~~~~
//        |                               availableForWrite
// check for:
//  Multiple libraries were found for "SdFat.h"
//   Used: C:\Users\User\Documents\Arduino\libraries\SdFat_-_Adafruit_Fork <<-- REMOVE this library. IT IS OUT-OF-DATE. Then a different library will automatically be used instead.
//   Not used: C:\Users\User\AppData\Local\Arduino15\packages\rp2040\hardware\rp2040\2.7.1\libraries\ESP8266SdFat

#include <Adafruit_GFX.h>    // Core graphics library
#ifdef ESP32_S3_TFT
  // This is a library for several Adafruit displays based on ST77* drivers.
  // Works with the Adafruit TFT Gizmo
  // ----> http://www.adafruit.com/products/4367
  #include <Adafruit_ST7789.h> // Hardware-specific library for ST7789 // on ESP32-S3 TFT
#endif
#ifdef HX8357
  // This is our library for the Adafruit 3.5" TFT (HX8357) FeatherWing
  // ----> http://www.adafruit.com/products/3651
  #include "Adafruit_HX8357.h" // on 3.5" TFT (HX8357) FeatherWing

  // If using the rev 1 with STMPE resistive touch screen controller uncomment this line:
  //#include <Adafruit_STMPE610.h>
  // If using the rev 2 with TSC2007, uncomment this line:
  #include <Adafruit_TSC2007.h>
#endif

#include <SPI.h>
#include <Wire.h> // for i2c keyboard, DRV2605 haptic feedback, FM RDS transmitter

#include <Adafruit_Si4713.h> // for FM RDS transmitter
#define _BV(n) (1 << n)

#include "Adafruit_seesaw.h" // mini gamepad (on STEMMA QT)

#ifdef USE_MP3
  #include <Adafruit_VS1053.h> // for mp3 playing from microSD card; wants same pins as (HX8357) 3.5" sTFT? (5, 6, 9, 10)
#endif

#include "Adafruit_DRV2605.h" // haptic

Adafruit_DRV2605 drv; // haptic

//#define DEBUG_BUTTONS
// ------------------------- FM RDS transmitter ----------------------
#define FM_RESETPIN 13 // wired to a different pin than intended (wanted 12)
//#define FM_RESETPIN 12
//#define FM_RESETPIN 5

Adafruit_Si4713 radio = Adafruit_Si4713(FM_RESETPIN);
uint16_t fm_station_freq = 10230; // 102.30 MHz
//#define DEBUG_FM

// ------------------------- TFT setup -------------------------

#ifdef ESP32_S3_TFT
  // Use dedicated hardware SPI pins
  Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

  // on ESP32-S3 TFT
  #define PIXELS_X 240
  #define PIXELS_Y 135
  
#elif defined HX8357
  #ifdef ESP8266
    #define STMPE_CS 16
    #define TFT_CS   0
    #define TFT_DC   15
    #define SD_CS    2
  #elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3)
    #define STMPE_CS 32
    #define TFT_CS   15
    #define TFT_DC   33
    #define SD_CS    14
  #elif defined(TEENSYDUINO)
    #define TFT_DC   10
    #define TFT_CS   4
    #define STMPE_CS 3
    #define SD_CS    8
  #elif defined(ARDUINO_STM32_FEATHER)
    #define TFT_DC   PB4
    #define TFT_CS   PA15
    #define STMPE_CS PC7
    #define SD_CS    PC5
  #elif defined(ARDUINO_NRF52832_FEATHER)  /* BSP 0.6.5 and higher! */
    #define TFT_DC   11
    #define TFT_CS   31
    #define STMPE_CS 30
    #define SD_CS    27
  #elif defined(ARDUINO_MAX32620FTHR) || defined(ARDUINO_MAX32630FTHR)
    #define TFT_DC   P5_4
    #define TFT_CS   P5_3
    #define STMPE_CS P3_3
    #define SD_CS    P3_2
  #else
      // Anything else, defaults!
    #define STMPE_CS 6
    #define TFT_CS   9
    #define TFT_DC   10
    #define SD_CS    5
  #endif

  #define TFT_RST -1

  // Use hardware SPI and the above for CS/DC
  Adafruit_HX8357 tft = Adafruit_HX8357(TFT_CS, TFT_DC, TFT_RST);

// --------------------- touchscreen setup ---------------------

  #if defined(_ADAFRUIT_STMPE610H_)
    Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);
  #elif defined(_ADAFRUIT_TSC2007_H)
    // If you're using the TSC2007 there is no CS pin needed, so instead its an IRQ!
    #define TSC_IRQ STMPE_CS
    Adafruit_TSC2007 ts = Adafruit_TSC2007();             // newer rev 2 touch contoller
  #else
    #error("You must have either STMPE or TSC2007 headers included!")
  #endif

  // This is calibration data for the raw touch data to the screen coordinates
  // For STMPE811/STMPE610
  #define STMPE_TS_MINX 3800
  #define STMPE_TS_MAXX 100
  #define STMPE_TS_MINY 100
  #define STMPE_TS_MAXY 3750
  // For TSC2007
  #define TSC_TS_MINX 300
  #define TSC_TS_MAXX 3800
  #define TSC_TS_MINY 185
  #define TSC_TS_MAXY 3700
  // set during setup(); use during loop()
  int16_t touch_minx, touch_maxx;
  int16_t touch_miny, touch_maxy;
#else
  GFXcanvas16 tft = GFXcanvas16(240, 135);
#endif
//#define DEBUG_TOUCH


// --------------------- input ------------------------

// on-board digital input button
#define PIN_BOOT_BUTTON  0 // on ESP32 S3 TFT

// mini gamepad (on STEMMA QT)
Adafruit_seesaw ss;
#define BUTTON_X         6
#define BUTTON_Y         2
#define BUTTON_A         5
#define BUTTON_B         1
#define BUTTON_SELECT    0
#define BUTTON_START    16
uint32_t button_mask = (1UL << BUTTON_X) | (1UL << BUTTON_Y) | (1UL << BUTTON_START) |
                       (1UL << BUTTON_A) | (1UL << BUTTON_B) | (1UL << BUTTON_SELECT);
//#define IRQ_PIN   5

// ----------------------- mp3 sound output ----------------------------
#ifdef USE_MP3
  // sound module: Adafruit Music Maker FeatherWing
  // (3357 headphone version, or 1788 3W stereo amp version)
  // These are the pins used
  #define VS1053_RESET   -1     // VS1053 reset pin (not used!)

  // Feather ESP8266
  #if defined(ESP8266)
    #define VS1053_CS      16     // VS1053 chip select pin (output)
    #define VS1053_DCS     15     // VS1053 Data/command select pin (output)
    #define CARDCS          2     // Card chip select pin
    #define VS1053_DREQ     0     // VS1053 Data request, ideally an Interrupt pin

  // Feather ESP32
  #elif defined(ESP32) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S2) && !defined(ARDUINO_ADAFRUIT_FEATHER_ESP32S3_TFT)
    #define VS1053_CS      32     // VS1053 chip select pin (output)
    #define VS1053_DCS     33     // VS1053 Data/command select pin (output)
    #define CARDCS         14     // Card chip select pin
    #define VS1053_DREQ    15     // VS1053 Data request, ideally an Interrupt pin

  // Feather Teensy3
  #elif defined(TEENSYDUINO)
    #define VS1053_CS       3     // VS1053 chip select pin (output)
    #define VS1053_DCS     10     // VS1053 Data/command select pin (output)
    #define CARDCS          8     // Card chip select pin
    #define VS1053_DREQ     4     // VS1053 Data request, ideally an Interrupt pin

  // WICED feather
  #elif defined(ARDUINO_STM32_FEATHER)
    #define VS1053_CS       PC7     // VS1053 chip select pin (output)
    #define VS1053_DCS      PB4     // VS1053 Data/command select pin (output)
    #define CARDCS          PC5     // Card chip select pin
    #define VS1053_DREQ     PA15    // VS1053 Data request, ideally an Interrupt pin

  #elif defined(ARDUINO_NRF52832_FEATHER )
    #define VS1053_CS       30     // VS1053 chip select pin (output)
    #define VS1053_DCS      11     // VS1053 Data/command select pin (output)
    #define CARDCS          27     // Card chip select pin
    #define VS1053_DREQ     31     // VS1053 Data request, ideally an Interrupt pin

  // Feather RP2040
  #elif defined(ARDUINO_ADAFRUIT_FEATHER_RP2040)
    #define VS1053_CS       8     // VS1053 chip select pin (output)
    #define VS1053_DCS     10     // VS1053 Data/command select pin (output)
    #define CARDCS          7     // Card chip select pin
    #define VS1053_DREQ     9     // VS1053 Data request, ideally an Interrupt pin

  // Feather M4, M0, 328, ESP32S2, ESP32S3 TFT, nRF52840 or 32u4
  #else
    #define VS1053_CS       6     // VS1053 chip select pin (output)
    #define VS1053_DCS     10     // VS1053 Data/command select pin (output)
    #define CARDCS          5     // Card chip select pin
    // DREQ should be an Int pin *if possible* (not possible on 32u4)
    #define VS1053_DREQ     9     // VS1053 Data request, ideally an Interrupt pin

  #endif

  Adafruit_VS1053_FilePlayer musicPlayer =
    Adafruit_VS1053_FilePlayer(VS1053_RESET, VS1053_CS, VS1053_DCS, VS1053_DREQ, CARDCS);

#endif

bool touch_found;
bool gamepad_found;
bool haptic_found;
bool fm_tx_found;
bool mp3_found;

// ------------------ setup ------------------
void setup(void) {
  //Serial.begin(9600);
  Serial.begin(115200);
  delay(2000);
  Serial.println(F("Hello! Welcome to windw serial port!"));

  // ------------------------ Start mp3 sound output ------------------------------------
#ifdef USE_MP3
  Serial.println(F("Checking MP3 sound output module (VS1053)..."));
  mp3_found = musicPlayer.begin();
  if (!mp3_found) { // initialise the music player
    Serial.println(F("Couldn't find MP3 sound output module (VS1053) - no sound output will be used. Check the right pins are defined?"));
    Serial.print("Reset: pin "); Serial.println(VS1053_RESET);
    Serial.print("MP3 chip select: pin "); Serial.println(VS1053_CS);
    Serial.print("Data select:     pin "); Serial.println(VS1053_DCS);
    Serial.print("Data request:    pin "); Serial.println(VS1053_DREQ);
    Serial.print("SD card chip select: pin "); Serial.println(CARDCS);
  } else {
    Serial.println(F("MP3 sound output module (VS1053) found OK."));

    musicPlayer.sineTest(0x44, 500);    // Make a tone to indicate VS1053 is working
    mp3_found = SD.begin(CARDCS);
    if (!mp3_found) {
      Serial.println(F("MP3 sound SD card failed / not present - no sound output will be used."));
    }
  }
  if (mp3_found) {
    Serial.println(F("MP3 sound SD card is OK!"));

    // list files
    printDirectory(SD.open("/"), 0);

    // Set volume for left, right channels. lower numbers == louder volume!
    musicPlayer.setVolume(10,10);

  #if defined(__AVR_ATmega32U4__)
    // Timer interrupts are not suggested, better to use DREQ interrupt!
    // but we don't have them on the 32u4 feather...
    musicPlayer.useInterrupt(VS1053_FILEPLAYER_TIMER0_INT); // timer int
  #else
    // If DREQ is on an interrupt pin we can do background
    // audio playing
    musicPlayer.useInterrupt(VS1053_FILEPLAYER_PIN_INT);  // DREQ int
  #endif
  }
#endif

  // ------------------------ Start FM RDS Radio Tx ------------------------------------
  Serial.println(F("Checking FM RDS transmitter..."));
  delay(2000);  
  fm_tx_found = radio.begin(); // begin with address 0x63 (CS high default)
  if (!fm_tx_found) {
    delay(2000);
    Serial.println("Couldn't find FM RDS transmitter?");
    set_fm_station_label ("NO FM");
  } else {
    uint8_t noise_lvl;
    uint8_t lowest_noise_lvl = 255;
    uint16_t lowest_noise_freq = 0;

    delay(2000);
    Serial.println("Found FM RDS transmitter OK. Measuring...");
    
    // scan power of entire range from 87.5 to 108.0 MHz
    for (uint16_t f  = 8750; f<10800; f+=20) {
      radio.readTuneMeasure(f);
      if (f % 50 == 0) {
        Serial.print(f/100); 
        Serial.print('.'); 
        Serial.println(f % 100);
      }
#ifdef DEBUG_FM
      Serial.print("Measuring "); Serial.print(f); Serial.print("...");
#endif
      radio.readTuneStatus();
      noise_lvl = radio.currNoiseLevel;
#ifdef DEBUG_FM
      Serial.println(noise_lvl);
#endif
      if (lowest_noise_lvl > noise_lvl) {
        lowest_noise_lvl = noise_lvl;
        lowest_noise_freq = f;
      }
    }
    Serial.print("Lowest noise freq "); Serial.print(lowest_noise_freq); Serial.print(" at ");
    Serial.println(lowest_noise_lvl);
    fm_station_freq = lowest_noise_freq;

    Serial.print("\nSet TX power");
    radio.setTXpower(115);  // dBuV, 88-115 max

    Serial.print("\nTuning into "); 
    Serial.print(fm_station_freq/100); 
    Serial.print('.'); 
    Serial.println(fm_station_freq % 100);
    radio.tuneFM(fm_station_freq); // e.g. 102.3 mhz
    //snprintf (fm_station_str, LABEL_MAX_TEXT, "%d.%d", fm_station_freq/100, (fm_station_freq % 100)/10);
    set_fm_station_label ("%d.%d", fm_station_freq/100, (fm_station_freq % 100)/10);

    // This will tell you the status in case you want to read it from the chip
    radio.readTuneStatus();
    Serial.print("\tCurr freq: "); 
    Serial.println(radio.currFreq);
    Serial.print("\tCurr freqdBuV:"); 
    Serial.println(radio.currdBuV);
    Serial.print("\tCurr ANTcap:"); 
    Serial.println(radio.currAntCap);

    // begin the RDS/RDBS transmission
    radio.beginRDS();
    radio.setRDSstation("AdaRadio");
    radio.setRDSbuffer( "Adafruit g0th Radio!");

    Serial.println("RDS on!");  
  }

  // Start TFT display
#ifdef ESP32_S3_TFT
  // turn on backlite
  pinMode(TFT_BACKLITE, OUTPUT);
  digitalWrite(TFT_BACKLITE, HIGH);

  //pinMode(TFT_BACKLIGHT, OUTPUT);
  //digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight on; for CircuitPlayground Express TFT Gizmo

  // turn on the TFT / I2C power supply
  pinMode(TFT_I2C_POWER, OUTPUT);
  digitalWrite(TFT_I2C_POWER, HIGH);
  delay(10);

  tft.init(PIXELS_Y, PIXELS_X);                // Init ST7789 ST7789 240x135 on ESP32-S3 TFT
#endif

  // ----------------------- start touchscreen input ----------------------------
#ifdef HX8357
  #if defined(_ADAFRUIT_STMPE610H_)
    touch_found = ts.begin();
    if (!touch_found) {
      Serial.println("Couldn't start STMPE touchscreen controller");
    } else {
      touch_minx = STMPE_TS_MINX; touch_maxx = STMPE_TS_MAXX;
      touch_miny = STMPE_TS_MINY; touch_maxy = STMPE_TS_MAXY;
      Serial.println("Touchscreen STMPE started");
    }
  #else
    touch_found = ts.begin(0x48, &Wire);
    if (!touch_found) {
      Serial.println("Couldn't start TSC2007 touchscreen controller");
    } else {
      touch_minx = TSC_TS_MINX; touch_maxx = TSC_TS_MAXX;
      touch_miny = TSC_TS_MINY; touch_maxy = TSC_TS_MAXY;
      pinMode(TSC_IRQ, INPUT);
      Serial.println("Touchscreen TSC2007 started");
    }
  #endif

  tft.begin();

  // read TFT display diagnostics (optional but can help debug problems)
  uint8_t x = tft.readcommand8(HX8357_RDPOWMODE);
  Serial.print("Display Power Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDMADCTL);
  Serial.print("MADCTL Mode: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDCOLMOD);
  Serial.print("Pixel Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDDIM);
  Serial.print("Image Format: 0x"); Serial.println(x, HEX);
  x = tft.readcommand8(HX8357_RDDSDR);
  Serial.print("Self Diagnostic: 0x"); Serial.println(x, HEX); 
#endif
  tft.setRotation(3);//tft.setRotation(2);
  tft.fillScreen(get_color_backgr());

  Serial.println(F("Initialized"));

  uint16_t time = millis();
  tft.fillScreen(get_color_backgr());
  time = millis() - time;

  Wire.begin();        // join i2c bus (address optional for master)

  // ------------------------ Start mini gamepad input ---------------------------
  gamepad_found = ss.begin(0x50);
  if (!gamepad_found) {
    Serial.println("seesaw not found - no gamepad will be used.");
  } else { // gamepad_found
    Serial.println("gamepad started");

    uint32_t version = ((ss.getVersion() >> 16) & 0xFFFF);
    if (version != 5743) {
      Serial.print("Wrong firmware loaded? ");
      Serial.println(version);
      while(1) delay(10);
    }
    Serial.println("Found Product 5743");
    
    ss.pinModeBulk(button_mask, INPUT_PULLUP);
    ss.setGPIOInterrupts(button_mask, 1);

  #if defined(IRQ_PIN)
    pinMode(IRQ_PIN, INPUT);
  #endif
  }
  
  Serial.println(time, DEC);
  tft.fillScreen(get_color_backgr());

  // --------------------------------- Start haptic output ----------------------------
  haptic_found = drv.begin();
  if (!haptic_found) {
    Serial.println("DRV2605 not found - no haptic feedback will be used.");
  } else { // haptic_found
    drv.selectLibrary(1);
  
    // I2C trigger by sending 'go' command 
    // default, internal trigger when sending GO command
    drv.setMode(DRV2605_MODE_INTTRIG); 
  }

  // ------------------------  Set up on-board digital input button -------------------------
  pinMode     (PIN_BOOT_BUTTON, INPUT_PULLUP);
  //digitalWrite(PIN_BOOT_BUTTON, HIGH); // enable pull-up

  // ---------------------------- Set up screen controls -----------------------------------
  setup_screen();
}

// ---------------------- MP3 sound output: list SD card files ------------------
#ifdef USE_MP3
/// File listing helper
// print files on SD card, which should include sound files for this game
void printDirectory(File dir, int numTabs) {
   while(true) {

     File entry =  dir.openNextFile();
     if (! entry) {
       // no more files
       //Serial.println("**nomorefiles**");
       break;
     }
     for (uint8_t i=0; i<numTabs; i++) {
       Serial.print('\t');
     }
     Serial.print(entry.name());
     if (entry.isDirectory()) {
       Serial.println("/");
       printDirectory(entry, numTabs+1);
     } else {
       // files have sizes, directories do not
       Serial.print("\t\t");
       Serial.println(entry.size(), DEC);
     }
     entry.close();
   }
}
#endif

// ---------------------- CardKB mini keyboard constants ------------------
#define KB_ADDR 0x5f
#define MIN_PRINTABLE ' '
#define MAX_PRINTABLE '~'

// When you press Fn+A thru Fn+Z on cardkb,
//  the cardkb will return these values.
#define FUNC_KEY_COUNT ('Z'-'A'+1)
unsigned func_keys[FUNC_KEY_COUNT] =  // Fn+
{	0x9a, 0xaa, 0xa8, 0x9c, 0x8f, 0x9d, // abc def
	0x9e, 0x9f, 0x94, 0xa0, 0xa1, 0xa2, // ghi jkl
	0xac, 0xab, 0x95, 0x96, 0x8d, 0x90, // mno pqr
	0x9b, 0x91, 0x93, 0xa9, 0x8e, 0xa7, // stu vwx
	0x92, 0xa6 }; // yz
#define OKALT      '\n'
#define CANCELALT  0x1b
#define KEY_BACKSPACE 0x8
#define KEY_LEFT  0xb4 // <--
#define KEY_UP    0xb5 // ^
#define KEY_DOWN  0xb6 // v
#define KEY_RIGHT 0xb7 //  -->

// -------------------------------- loop --------------------------------

bool  do_click  = false;
bool was_click  = false;
int last_x = 0, last_y = 0; // old joystick position
void loop() {
  int x_move;
  int y_move;
  static int  y_old = 0;
  char        kb_in = 0; // no key pressed
  bool        do_unclick = false;

  // do_click can be set by touch, kb_in Enter press, or digital input button.
  do_click = false;

  if (fm_tx_found) {
#ifdef DEBUG_FM
    radio.readASQ();
    Serial.print("\tCurr ASQ: 0x"); 
    Serial.println(radio.currASQ, HEX);
    Serial.print("\tCurr InLevel:"); 
    Serial.println(radio.currInLevel);
#endif
  }

  // --------------------- Check touchscreen input -------------------------
#if defined(TSC_IRQ)
  if (!digitalRead(TSC_IRQ)) {
    // IRQ pin is low, we can read from touchscreen!

    TS_Point p_raw = ts.getPoint();
    int touch_x, touch_y;

#ifdef DEBUG_TOUCH
    Serial.print("X = "); Serial.print(p_raw.x);
    Serial.print("\tY = "); Serial.print(p_raw.y);
    Serial.print("\tPressure = "); Serial.print(p_raw.z);
#endif
    //if (((p_raw.x == 0) && (p_raw.y == 0)) || (p_raw.z < 10)) return; // no pressure, no touch
    if (((p_raw.x != 0) || (p_raw.y != 0)) && (p_raw.z >= 10)) {
      // Scale from ~0->4000 to tft.width using the calibration #'s
      //OK with some TFT rotations:
      //p.x = map(p.x, touch_minx, touch_maxx, 0, tft.width());
      //p.y = map(p.y, touch_miny, touch_maxy, 0, tft.height());
      //Serial.print(" -> "); Serial.print(p.x); Serial.print(", "); Serial.println(p.y);
      //our TFT rotation:
      touch_x = map(p_raw.y, touch_miny, touch_maxy, tft.width(), 0); // screen x: raw y scaled, and left-right mirrored
      touch_y = map(p_raw.x, touch_minx, touch_maxx, 0, tft.height());  // screen y: raw x scaled
  #ifdef DEBUG_TOUCH
      Serial.print(" -> "); Serial.print(touch_x); Serial.print(", "); Serial.println(touch_y);
  #endif

      ScreenTouch(touch_x, touch_y);
    } // end if (touch && pressure)
  }
#endif

   // ----------------------  Check CardKB mini keyboard input --------------------------
  Wire.requestFrom(KB_ADDR, 1);    // request 1 byte from peripheral device KB_ADDR

  x_move = 0;
  y_move = 0;

  if (Wire.available()) { // peripheral may send less than requested
    kb_in = Wire.read(); // receive a byte as character
    switch (kb_in) {
    case 0x00: // nothing pressed. do nothing.
      break;
    case 0xd: // Enter
      kb_in = '\n';
      do_click = true;
      //was_click = true;
      break;
    case 0x1b: // Escape
      break;
    case KEY_BACKSPACE: // Backspace
      break;
    case 0x9: // Tab
      break;
    case KEY_LEFT: // <- Left
      x_move = -1;
      break;
    case KEY_UP: // ^ Up
      y_move = -1;
      break;
    case KEY_DOWN: // v Down
      y_move = 1;
      break;
    case KEY_RIGHT: // Right ->
      x_move = 1;
      break;
    default:
      if (kb_in >= MIN_PRINTABLE && kb_in <= MAX_PRINTABLE) {
        // A-Z, 1-9, punctuation, space
      } else {
        // Someone probably pressed "Function" + key, got 0x80 thru 0xAF
        // See https://docs.m5stack.com/en/unit/cardkb_1.1
      }
    }
    //Serial.print(kb_in);         // print the character
  }

#ifdef DEBUG_BUTTONS
  Serial.print("Left Button: ");
  if (left_btn) {
    Serial.print("DOWN");
  } else {
    Serial.print("  UP");
  }
  Serial.print("   Right Button: ");
  if (right_btn) {
    Serial.print("DOWN");
  } else {
    Serial.print("  UP");    
  }
  Serial.println();
#endif

  // ------------------------ Check mini gamepad input ---------------------------
  if (gamepad_found) {
    // Reverse x/y values to match joystick orientation
    int joy_x = 1023 - ss.analogRead(14);
    int joy_y = 1023 - ss.analogRead(15);
    
    if ( (abs(joy_x - last_x) > 3)  ||  (abs(joy_y - last_y) > 3)) {
      Serial.print("joy_x: "); Serial.print(joy_x); Serial.print(", "); Serial.print("joy_y: "); Serial.println(joy_y);
      last_x = joy_x;
      last_y = joy_y;
    }
    // X joystick axis: be more sensitive, as it is harder to push
    if (joy_x < 400) {
      x_move = -1;
    }
    if (joy_x > 600) {
      x_move = 1;
    }
    // Y joystick axis: be less sensitive, as it is so easy to push (don't override X motions)
    if (joy_y < 128) {
      y_move = 1;
    }
    if (joy_y > 900) {
      y_move = -1;
    }
#if defined(IRQ_PIN)
    if(!digitalRead(IRQ_PIN)) {
      return;
    }
#endif

    uint32_t buttons = ss.digitalReadBulk(button_mask);
    if (! (buttons & (1UL << BUTTON_X))) {
      y_move = -1;
    }
    if (! (buttons & (1UL << BUTTON_B))) {
      y_move = 1;
    }
    if (! (buttons & (1UL << BUTTON_Y))) {
      x_move = -1;
    }
    if (! (buttons & (1UL << BUTTON_A))) {
      x_move = 1;
    }
  } // end if(gamepad_found)

  // move the direction that was tilted the fastest,
  //  or if none tilted fast, the way we are most tilted.
  if (y_move > 0 && y_old == 0) {
    ScreenTabNext();
  }
  if (y_move < 0 && y_old == 0) {
    ScreenTabPrev();
  }
  if (x_move > 0) {
    do_click = true;
    //was_click = true;
  }
  if (was_click && !do_click) {
    do_unclick = true;
  }

  // ------------------------ Check onboard digital input button ---------------------------
  int btn_in = digitalRead(PIN_BOOT_BUTTON);
  static int old_btn_in = 1;

  if (!btn_in && old_btn_in) { // b/c pull-up
    do_click = true;
  }
  if (btn_in && !old_btn_in) { // un-click
    do_unclick = true;
  }

  // ------------------------- Send input to control that has focus --------------------------
  loop_screen (kb_in, do_unclick);


  old_btn_in = btn_in;
  y_old = y_move;

  delay(100);
}

// ---------------- make a haptic rumble "click" ------------
void haptic_click(void) {
  if (!haptic_found) {
    return;
  }
  int effect = 1; // Strong Click - 100%

  // set the effect to play
  drv.setWaveform(0, effect);  // play effect 
  drv.setWaveform(1, 0);       // end waveform

  // play the effect!
  drv.go();
}

