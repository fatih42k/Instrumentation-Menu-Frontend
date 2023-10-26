#include <math.h>
#include <ItemSubMenu.h>
#include <LcdMenu.h>
#include <OneWire.h>
#include <DallasTemperature.h>

#define LCD_ROWS 2
#define LCD_COLS 16
#define ONE_WIRE_BUS 8

// Configure keyboard keys (ASCII)
#define UP 119        // w
#define DOWN 115      // s
#define LEFT 97       // a
#define RIGHT 100     // d
#define ENTER 101     // e
#define BACK 98       // b
#define BACKSPACE 102 // f
#define CLEAR 99      // c

extern MenuItem* mainMenu[];
extern MenuItem* PHSubMenu[];
extern MenuItem* TDSSubMenu[];

// Construct OneWire and DallasTemperature instances
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
// Variable to store current temperature
float currentTemperature = -127.00;
char currentTemperatureChar[8]; // Construct a char array with a length of 8 (7 characters + null-terminator)
char temperatureCharArray[15] = "TEMP: -127.00C";
// Function to read and update temperature
void updateTemperature() {
    sensors.requestTemperatures();
    currentTemperature = sensors.getTempCByIndex(0);
    dtostrf(currentTemperature, 7, 2, currentTemperatureChar);
    for (int i=0; i<7; i++){
      temperatureCharArray[i+6] = currentTemperatureChar[i];
    }
}

// Define subsubmenus
SUB_MENU(PHSettings, PHSubMenu,
    ITEM_BASIC("PH:  4.000"),
    ITEM_BASIC("PH:  7.000"),
    ITEM_BASIC("PH: 10.000")
);

SUB_MENU(TDSSettings, TDSSubMenu,
    ITEM_BASIC("TDS:  100PPM"),
    ITEM_BASIC("TDS:  500PPM"),
    ITEM_BASIC("TDS: 1000PPM")
);

// Define submenus
SUB_MENU(PHSubMenu, mainMenu,
    ITEM_SUBMENU("PH: 7.000", PHSettings)
);

SUB_MENU(TDSSubMenu, mainMenu,
    ITEM_SUBMENU("TDS: 200PPM", TDSSettings)
);

// Define the temperature menu item
MenuItem* temperature = new MenuItem("TEMP: -127.00C");

// Define the TEMPSubMenu
SUB_MENU(TEMPSubMenu, mainMenu,
    temperature
);

// Define the main menu
MAIN_MENU(
    ITEM_SUBMENU("PH", PHSubMenu),
    ITEM_SUBMENU("TDS", TDSSubMenu),
    ITEM_SUBMENU("TEMP", TEMPSubMenu)
);

LcdMenu menu(LCD_ROWS, LCD_COLS);

void setup() {
    Serial.begin(9600);
    menu.setupLcdWithMenu(0x27, mainMenu);
}

void loop() {
    uint8_t cursor = menu.getCursorPosition();
    MenuItem* item = menu.getItemAt(cursor);

    if (item == temperature) {
        updateTemperature();
        item->setText(temperatureCharArray);
        menu.update();
        delay(1000);
    }
    
    if (!Serial.available()) return;
    char command = Serial.read();
    
    if (command == UP)
        menu.up();
    else if (command == DOWN)
        menu.down();
    else if (command == LEFT)
        menu.left();
    else if (command == RIGHT)
        menu.right();
    else if (command == ENTER)
        menu.enter();
    else if (command == BACK)
        menu.back();
}
