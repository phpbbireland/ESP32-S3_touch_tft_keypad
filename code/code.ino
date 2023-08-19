/* 
 *  Code last tested: 2023/08/19 ... Working! 
 *  
 *  Project Name: 3x5_touch_tft_keypad - https://github.com/phpbbireland/3x5_touch_tft_keypad
 *  
 *  An LCD/TFT (Makerfabs-ESP32-S3-SPI-TFT-with-Touch), an Image (320x240px) including 16 icons (buttons) and a Text file with 16 lines of text...
 *  
 *  Basically we simply use a predesigned 320x240 image containging 16 icons (one for each key) and map the their position to touch actions...
 *  
 *  We load the image and read the text file (menu actions) from SD card and that's it...
 *  
 *
 *  The text file consists of 15 lines, each containg the actual characters we need to send to the computer...
 *  Special keys: [ALT], [SHIFT], [CTRL], [SUPER], [SPACE], [TAB], [F1 - [F24]...
 *  
 *  Each line can contain:  
 *    A program name (can include the path) to launch, for example: ~/{HOME}/this_path/this_program
 *    Special key combinations: [ALT]{3} or [CTRL][F10] or [SHIFT][CTRL][ALT]{g}
 *    Any combination of keys up to 128 characters...
 *        
 *
 * Code relating to loading config data from file was created by ArduinoGetStarted.com
 *
 * This example code is in the public domain
 *
 * Tutorial page: https://arduinogetstarted.com/tutorials/arduino-read-config-from-sd-card
 */
 
#define LGFX_USE_V1
#include <LovyanGFX.hpp>
#include <vector>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include "USB.h"
#include "USBHIDKeyboard.h"
#include "FT6236.h"
#include "Button.h"
#include "SPI_9488.h"

#define LCD_MOSI 13
#define LCD_MISO 12
#define LCD_SCK 14
#define LCD_CS 15
#define LCD_RST -1 // 26
#define LCD_DC 21  // 33
#define LCD_BL -1
#define SD_MOSI 2
#define SD_MISO 41
#define SD_SCK 42
#define SD_CS 1
#define I2C_SCL 39
#define I2C_SDA 38

#define COLOR_BACKGROUND TFT_BLACK
#define COLOR_BUTTON TFT_BLACK
#define COLOR_BUTTON_P 0x4BAF
#define COLOR_TEXT TFT_WHITE
#define COLOR_LINE TFT_RED
#define BUTTON_POS_X 2         // button x margin
#define BUTTON_POS_Y 1         // button y margin
#define BUTTON_DELAY 150
#define BUTTONS_PER_PAGE 15

#define KEY_MAX_LENGTH    20
#define VALUE_MAX_LENGTH  128
#define FILE_NAME "/menu0"



/*
class LGFX : public lgfx::LGFX_Device
{
    // lgfx::Panel_ILI9341 _panel_instance;
    lgfx::Panel_ILI9488 _panel_instance;
    lgfx::Bus_SPI _bus_instance;

public:
    // コンストラクタを作成し、ここで各種設定を行います。
    // クラス名を変更した場合はコンストラクタも同じ名前を指定してください。
    LGFX(void)
    {
        {                                      // バス制御の設定を行います。
            auto cfg = _bus_instance.config(); // バス設定用の構造体を取得します。

            // SPIバスの設定
            cfg.spi_host = SPI3_HOST;  // 使用するSPIを選択  (VSPI_HOST or HSPI_HOST)
            cfg.spi_mode = 0;          // SPI通信モードを設定 (0 ~ 3)
            cfg.freq_write = 40000000; // 送信時のSPIクロック (最大80MHz, 80MHzを整数で割った値に丸められます)
            cfg.freq_read = 16000000;  // 受信時のSPIクロック
            cfg.spi_3wire = true;      // 受信をMOSIピンで行う場合はtrueを設定
            cfg.use_lock = true;       // トランザクションロックを使用する場合はtrueを設定
            cfg.dma_channel = 1;       // Set the DMA channel (1 or 2. 0=disable)   使用するDMAチャンネルを設定 (0=DMA不使用)
            cfg.pin_sclk = LCD_SCK;    // SPIのSCLKピン番号を設定
            cfg.pin_mosi = LCD_MOSI;   // SPIのMOSIピン番号を設定
            cfg.pin_miso = LCD_MISO;   // SPIのMISOピン番号を設定 (-1 = disable)
            cfg.pin_dc = LCD_DC;       // SPIのD/Cピン番号を設定  (-1 = disable)
                                       // SDカードと共通のSPIバスを使う場合、MISOは省略せず必ず設定してください。

            _bus_instance.config(cfg);              // 設定値をバスに反映します。
            _panel_instance.setBus(&_bus_instance); // バスをパネルにセットします。
        }

        {                                        // 表示パネル制御の設定を行います。
            auto cfg = _panel_instance.config(); // 表示パネル設定用の構造体を取得します。

            cfg.pin_cs = LCD_CS;   // CSが接続されているピン番号   (-1 = disable)
            cfg.pin_rst = LCD_RST; // RSTが接続されているピン番号  (-1 = disable)
            cfg.pin_busy = -1;     // BUSYが接続されているピン番号 (-1 = disable)

            // ※ 以下の設定値はパネル毎に一般的な初期値が設定されていますので、不明な項目はコメントアウトして試してみてください。

            cfg.memory_width = 320;   // ドライバICがサポートしている最大の幅
            cfg.memory_height = 480;  // ドライバICがサポートしている最大の高さ
            cfg.panel_width = 320;    // 実際に表示可能な幅
            cfg.panel_height = 480;   // 実際に表示可能な高さ
            cfg.offset_x = 0;         // パネルのX方向オフセット量
            cfg.offset_y = 0;         // パネルのY方向オフセット量
            cfg.offset_rotation = 0;  // 回転方向の値のオフセット 0~7 (4~7は上下反転)
            cfg.dummy_read_pixel = 8; // ピクセル読出し前のダミーリードのビット数
            cfg.dummy_read_bits = 1;  // ピクセル以外のデータ読出し前のダミーリードのビット数
            cfg.readable = true;      // データ読出しが可能な場合 trueに設定
            cfg.invert = false;       // パネルの明暗が反転してしまう場合 trueに設定
            cfg.rgb_order = false;    // パネルの赤と青が入れ替わってしまう場合 trueに設定
            cfg.dlen_16bit = false;   // データ長を16bit単位で送信するパネルの場合 trueに設定
            cfg.bus_shared = true;    // SDカードとバスを共有している場合 trueに設定(drawJpgFile等でバス制御を行います)

            _panel_instance.config(cfg);
        }

        setPanel(&_panel_instance); // 使用するパネルをセットします。
    }
};
*/


LGFX lcd;
USBHIDKeyboard Keyboard;  
SPIClass SD_SPI;          

int mx,my, mix = 0;
int pos[2] = {0, 0};

String menuitem01, menuitem02,menuitem03, menuitem04, menuitem05, menuitem06, menuitem07;
String menuitem08, menuitem09,menuitem10, menuitem11,menuitem12, menuitem13, menuitem14, menuitem15;

void setup(void)
{
    Serial.begin(115200);              
    Serial.println("Keyboard begin");  
    
    lcd_init();         
    lcd.setRotation(5); // rotate so USB is on top to suit my case...

    sd_init();        

    /* Load main menu text from SD card (currently named menu0) and fill variables... */
    
    menuitem01 = SD_findString(F("menuitem01"));
    menuitem02 = SD_findString(F("menuitem02"));
    menuitem03 = SD_findString(F("menuitem03"));
    menuitem04 = SD_findString(F("menuitem04"));
    menuitem05 = SD_findString(F("menuitem05"));
    menuitem06 = SD_findString(F("menuitem06"));
    menuitem07 = SD_findString(F("menuitem07"));
    menuitem08 = SD_findString(F("menuitem08"));
    menuitem09 = SD_findString(F("menuitem09"));
    menuitem10 = SD_findString(F("menuitem10"));
    menuitem11 = SD_findString(F("menuitem11"));
    menuitem12 = SD_findString(F("menuitem12"));
    menuitem13 = SD_findString(F("menuitem13"));
    menuitem14 = SD_findString(F("menuitem14"));
    menuitem15 = SD_findString(F("menuitem15"));

    Keyboard.begin();  
    USB.begin();       

    print_img(SD, "/menu0.bmp", 480, 320);  // Load and display the Background Image 
    delay(100);
    mainMenu();
}

void loop(void)
{
   
}

/* ArduinoGetStarted.com example code Starts */

String SD_findString(const __FlashStringHelper * key)
{
  char value_string[VALUE_MAX_LENGTH];
  int value_length = SD_findKey(key, value_string);
  return HELPER_ascii2String(value_string, value_length);
}

int SD_findKey(const __FlashStringHelper * key, char * value)
{
  File configFile = SD.open(FILE_NAME);

  if (!configFile)
  {
    Serial.print(F("SD Card: error on opening file "));
    Serial.println(FILE_NAME);
    return 0;
  }

  char key_string[KEY_MAX_LENGTH];
  char SD_buffer[KEY_MAX_LENGTH + VALUE_MAX_LENGTH + 1]; // 1 is = character
  int key_length = 0;
  int value_length = 0;

  // Flash string to string
  PGM_P keyPoiter;
  keyPoiter = reinterpret_cast<PGM_P>(key);
  byte ch;
  do {
    ch = pgm_read_byte(keyPoiter++);
    if (ch != 0)
      key_string[key_length++] = ch;
  } while (ch != 0);

  // check line by line
  while (configFile.available())
  {
    int buffer_length = configFile.readBytesUntil('\n', SD_buffer, VALUE_MAX_LENGTH+1);
    if (SD_buffer[buffer_length - 1] == '\r')
      buffer_length--; // trim the \r

    if (buffer_length > (key_length + 1)) { // 1 is = character
      if (memcmp(SD_buffer, key_string, key_length) == 0) { // equal
        if (SD_buffer[key_length] == '=') {
          value_length = buffer_length - key_length - 1;
          memcpy(value, SD_buffer + key_length + 1, value_length);
          break;
        }
      }
    }
  }

  configFile.close();  // close the file
  return value_length;
}

String HELPER_ascii2String(char *ascii, int length)
{
  String str;
  str.reserve(length);
  str = "";

  for (int i = 0; i < length; i++)
  {
    char c = *(ascii + i);
    str += String(c);
  }
  return str;
}

/* ArduinoGetStarted.com example code Ends */

void  mainMenu()
{
    char str[80];

    lcd.setRotation(0);
    
    Button b[BUTTONS_PER_PAGE];

    static String b_list[BUTTONS_PER_PAGE] = {
      menuitem01,
      menuitem02,
      menuitem03,
      menuitem04,
      menuitem05,
      menuitem06,
      menuitem07,
      menuitem08,
      menuitem09,
      menuitem10,
      menuitem11,
      menuitem12,
      menuitem13,
      menuitem14,
      menuitem15
    };

    // Build Buttons
    for (int i = 0; i < BUTTONS_PER_PAGE; i++)
    {
        Serial.print("[");
        Serial.print(b_list[i]);
        Serial.print("] [");
        Serial.print(BUTTON_POS_X + i % 3 * 105);
        Serial.print("][");
        Serial.print(BUTTON_POS_Y + i / 3 * 108);
        Serial.print("] [");

        b[i].set(BUTTON_POS_X + i % 3 * 105, BUTTON_POS_Y + i / 3 * 82 + mix, 103, 95, "NULL", ENABLE);
        b[i].setText(b_list[i]);
        b[i].setValue(i);

        Serial.print(i);
        Serial.print("]\n");

        if(i == 2 || i == 5 || i == 8 || i == 11) mix=mix+14;
        drawButton(b[i]);
    }

    while (1)
    {
        ft6236_pos(pos);

        for (int i = 0; i < BUTTONS_PER_PAGE; i++)
        {
            int button_value = UNABLE;
            if ((button_value = b[i].checkTouch(pos[0], pos[1])) != UNABLE)
            {
                /*
                Serial.printf("Pos is :%d,%d\n", pos[0], pos[1]);
                Serial.printf("Value is :%d\n", button_value);
                Serial.printf("Text is :");
                Serial.println(b[i].getText());
                */
                Serial.print("{"); Serial.print(b_list[i]); Serial.print("}");
                report();

                drawButton_p(b[i]);
                delay(BUTTON_DELAY);
                drawButton(b[i]);

                key_input_process(button_value, b_list[i]);
                //page_switch(button_value);
                delay(200);
                //lcd.enableSleep(true);
            }
        }
    }
}

void sd_init()
{
    SD_SPI.begin(SD_SCK, SD_MISO, SD_MOSI);
    if (!SD.begin(SD_CS, SD_SPI, 40000000))
    {
        Serial.println("Card Mount Failed");
        lcd.setCursor(10, 10);
        lcd.println("SD Card Failed");
        while (1)
            delay(1000);
    }
/*    
    else
    {
        Serial.println("Card Mount Successed");
    }

    Serial.println("SD init over.\n\n");
*/
}

// Display image from file
int print_img(fs::FS &fs, String filename, int x, int y)
{

    File f = fs.open(filename, "r");
    if (!f)
    {
        Serial.println("Failed to open file for reading");
        f.close();
        return 0;
    }

    f.seek(54);
    int X = x;
    int Y = y;
    uint8_t RGB[3 * X];
    for (int row = 0; row < Y; row++)
    {
        f.seek(54 + 3 * X * row);
        f.read(RGB, 3 * X);

        lcd.pushImage(0, row, X, 1, (lgfx::rgb888_t *)RGB);
    }

    f.close();
    return 0;
}

void drawButton(Button b)
{
    int b_x;
    int b_y;
    int b_w;
    int b_h;
    int shadow_len = 3;
    String text;
    int textSize;

    b.getFoDraw(&b_x, &b_y, &b_w, &b_h, &text, &textSize);

    lcd.drawRect(b_x, b_y, b_w, b_h, COLOR_LINE);
    lcd.setCursor(b_x + 20, b_y + 20);
    lcd.setCursor(b_x + 2, b_y + b_w/2+2);
    lcd.setTextColor(COLOR_TEXT);
    lcd.setTextSize(textSize);
}

void drawButton_p(Button b)
{
    int b_x;
    int b_y;
    int b_w;
    int b_h;
    int shadow_len = 3;
    String text;
    int textSize;

    b.getFoDraw(&b_x, &b_y, &b_w, &b_h, &text, &textSize);

    lcd.drawRect(b_x, b_y, b_w, b_h, TFT_WHITE);
    
    lcd.setCursor(b_x + 20, b_y + 20);
    lcd.setTextColor(COLOR_TEXT);
    lcd.setTextSize(textSize);
}

void clean_button()
{
    lcd.fillRect(BUTTON_POS_X, BUTTON_POS_Y, 319 - BUTTON_POS_X, 479 - BUTTON_POS_Y, COLOR_BACKGROUND);
}

void clean_screen()
{
    lcd.fillRect(0, 0, 319, 479, COLOR_BACKGROUND);
}

// May add more menus later...

void page_switch(int page)
{
    switch (page)
    {
    case 0:
        //menu0();
        break;
    case 1:
        //menu1();
        break;
    case 2:
        //menu2();
        break;

    defualt:
        break;
    }
    delay(100);
}

// needs to be more robust... later

void key_input_process(int value, String str)
{
    if(str.startsWith("[LALT]", 0))
    {
        Keyboard.write(KEY_LEFT_ALT);
        Keyboard.print("3");
        delay(100);
        Keyboard.releaseAll();
        //remove special keys
        //Keyboard.write(keydata);
    }
    else
    if(str.startsWith("[RALT]", 0))
    {
        Keyboard.write(KEY_RIGHT_ALT);
    }
    else
    if(str.startsWith("[LCTRL]", 0))
    {
        Keyboard.write(KEY_LEFT_CTRL);
    }
    else
    if(str.startsWith("[RCTRL]", 0))
    {
        Keyboard.write(KEY_RIGHT_CTRL);
    }
    else
    if(str.startsWith("[LSHIFT]", 0))
    {
        Keyboard.write(KEY_LEFT_SHIFT);
    }
    else
    if(str.startsWith("[RSHIFT]", 0))
    {
        Keyboard.write(KEY_RIGHT_SHIFT);
    }
    else
    {
      Keyboard.print(str);
      Keyboard.write(KEY_RETURN);
    }

    delay(100);
    Keyboard.releaseAll();
    //lcd.sleepDisplay(true);
}


void report()
{
    mx = getTouchPointX();
    my = getTouchPointY();
    Serial.print("[");
    Serial.print(mx);
    Serial.print("][");
    Serial.print(my);
    Serial.print("]\n");
}

// Hardware init
void lcd_init()
{
    lcd.init();
    lcd.fillScreen(COLOR_BACKGROUND);

    // I2C init
    Wire.begin(I2C_SDA, I2C_SCL);
    byte error, address;

    Wire.beginTransmission(TOUCH_I2C_ADD);
    error = Wire.endTransmission();

    if (error == 0)
    {
        /*
        Serial.print("I2C device found at address 0x");
        Serial.print(TOUCH_I2C_ADD, HEX);
        Serial.println("  !");
        */
    }
    else
    {
        Serial.print("Unknown error at address 0x");
        Serial.println(TOUCH_I2C_ADD, HEX);
    }
}