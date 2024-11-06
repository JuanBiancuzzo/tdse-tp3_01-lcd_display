//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"

#include "user_interface.h"

#include "smart_home_system.h"
#include "display.h"

//=====[Declaration of private defines]========================================

#define DISPLAY_REFRESH_TIME_MS 1000

#define DISPLAY_CONECCION_I2C 2
#define DISPLAY_CONECCION_4_BIT 4
#define DISPLAY_CONECCION_8_BIT 8

#define DISPLAY_CONECCION DISPLAY_CONECCION_I2C

//=====[Declaration of private data types]=====================================

//=====[Declaration and initialization of public global objects]===============

DigitalOut incorrectCodeLed(LED3);
DigitalOut systemBlockedLed(LED2);

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static bool incorrectCodeState = OFF;
static bool systemBlockedState = OFF;

static bool codeComplete = false;
static int numberOfCodeChars = 0;

//=====[Declarations (prototypes) of private functions]========================

static void userInterfaceDisplayInit();
static void userInterfaceDisplayUpdate();

//=====[Implementations of public functions]===================================

void userInterfaceInit()
{
    userInterfaceDisplayInit();
}

void userInterfaceUpdate()
{
    userInterfaceDisplayUpdate();
}

//=====[Implementations of private functions]==================================

static void userInterfaceDisplayInit()
{

    #if DISPLAY_CONECCION == DISPLAY_CONECCION_I2C
        displayInit(DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER);

    #elif DISPLAY_CONECCION == DISPLAY_CONECCION_4_BIT
        displayInit(DISPLAY_CONNECTION_GPIO_4BITS);

    #elif DISPLAY_CONECCION == DISPLAY_CONECCION_8_BIT
        displayInit(DISPLAY_CONNECTION_GPIO_8BITS);
        
    #endif
     
    displayCharPositionWrite ( 0,0 );
    displayStringWrite( "Temperature:" );
}

static void userInterfaceDisplayUpdate()
{
    static int accumulatedDisplayTime = 0;
    char temperatureString[3] = "";
    
    if( accumulatedDisplayTime >=
        DISPLAY_REFRESH_TIME_MS ) {

        accumulatedDisplayTime = 0;

        //sprintf(temperatureString, "%.0f", temperatureSensorReadCelsius());
        sprintf(temperatureString, "%.0f", 30.0f);
        displayCharPositionWrite(12, 0);
        displayStringWrite(temperatureString);
        displayCharPositionWrite(14, 0);
        displayStringWrite("'C");

        displayCharPositionWrite(4, 1);

    } else {
        accumulatedDisplayTime =
            accumulatedDisplayTime + SYSTEM_TIME_INCREMENT_MS;        
    }
}

static void incorrectCodeIndicatorUpdate()
{
    incorrectCodeLed = incorrectCodeStateRead();
}

static void systemBlockedIndicatorUpdate()
{
    systemBlockedLed = systemBlockedState;
}