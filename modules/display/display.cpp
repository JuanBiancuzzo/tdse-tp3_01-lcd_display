//=====[Libraries]=============================================================

#include "mbed.h"
#include "arm_book_lib.h"
#include "display.h"

#include "stm32f1xx_hal_gpio.h"

//=====[Declaration of private defines]========================================

#define DISPLAY_IR_CLEAR_DISPLAY   0b00000001
#define DISPLAY_IR_ENTRY_MODE_SET  0b00000100
#define DISPLAY_IR_DISPLAY_CONTROL 0b00001000
#define DISPLAY_IR_FUNCTION_SET    0b00100000
#define DISPLAY_IR_SET_DDRAM_ADDR  0b10000000

#define DISPLAY_IR_ENTRY_MODE_SET_INCREMENT 0b00000010
#define DISPLAY_IR_ENTRY_MODE_SET_DECREMENT 0b00000000
#define DISPLAY_IR_ENTRY_MODE_SET_SHIFT     0b00000001
#define DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT  0b00000000

#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON  0b00000100
#define DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_OFF 0b00000000
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_ON   0b00000010
#define DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF  0b00000000
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_ON    0b00000001
#define DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF   0b00000000

#define DISPLAY_IR_FUNCTION_SET_8BITS    0b00010000
#define DISPLAY_IR_FUNCTION_SET_4BITS    0b00000000
#define DISPLAY_IR_FUNCTION_SET_2LINES   0b00001000
#define DISPLAY_IR_FUNCTION_SET_1LINE    0b00000000
#define DISPLAY_IR_FUNCTION_SET_5x10DOTS 0b00000100
#define DISPLAY_IR_FUNCTION_SET_5x8DOTS  0b00000000

#define DISPLAY_20x4_LINE1_FIRST_CHARACTER_ADDRESS 0
#define DISPLAY_20x4_LINE2_FIRST_CHARACTER_ADDRESS 64
#define DISPLAY_20x4_LINE3_FIRST_CHARACTER_ADDRESS 20
#define DISPLAY_20x4_LINE4_FIRST_CHARACTER_ADDRESS 84

#define DISPLAY_RS_INSTRUCTION GPIO_PIN_RESET  
#define DISPLAY_RS_DATA        GPIO_PIN_SET

#define DISPLAY_RW_WRITE 0
#define DISPLAY_RW_READ  1

#define DISPLAY_PIN_RS  4
#define DISPLAY_PIN_RW  5
#define DISPLAY_PIN_EN  6
#define DISPLAY_PIN_D0  7  
#define DISPLAY_PIN_D1  8  
#define DISPLAY_PIN_D2  9  
#define DISPLAY_PIN_D3 10
#define DISPLAY_PIN_D4 11
#define DISPLAY_PIN_D5 12 
#define DISPLAY_PIN_D6 13 
#define DISPLAY_PIN_D7 14 

#define DISPLAY_PIN_A_PCF8574 3

#define I2C1_SDA PB_9
#define I2C1_SCL PB_8

#define PCF8574_I2C_BUS_8BIT_WRITE_ADDRESS 78

//=====[Declaration of private data types]=====================================

typedef struct{
    int address;
    char data;
    bool displayPinRs;
    bool displayPinRw; 
    bool displayPinEn;
    bool displayPinA;
    bool displayPinD4;
    bool displayPinD5;
    bool displayPinD6;
    bool displayPinD7;
} pcf8574_t;

//=====[Declaration and initialization of public global objects]===============

//DigitalOut displayD0( D2 );
#define D0_GPIO_Pin GPIO_PIN_10
#define D0_GPIO_Port GPIOA
//DigitalOut displayD1( D4 );
#define D1_GPIO_Pin GPIO_PIN_5
#define D1_GPIO_Port GPIOB
//DigitalOut displayD2( D5 );
#define D2_GPIO_Pin GPIO_PIN_4
#define D2_GPIO_Port GPIOB
//DigitalOut displayD3( D6 );
#define D3_GPIO_Pin GPIO_PIN_10
#define D3_GPIO_Port GPIOB
//DigitalOut displayD4( D7 );
#define D4_GPIO_Pin GPIO_PIN_8
#define D4_GPIO_Port GPIOA
//DigitalOut displayD5( D8 );
#define D5_GPIO_Pin GPIO_PIN_9
#define D5_GPIO_Port GPIOA
//DigitalOut displayD6( D9 );
#define D6_GPIO_Pin GPIO_PIN_7
#define D6_GPIO_Port GPIOC
//DigitalOut displayD7( D10 );
#define D7_GPIO_Pin GPIO_PIN_6
#define D7_GPIO_Port GPIOB
//DigitalOut displayRs( D11 );
#define Rs_GPIO_Pin GPIO_PIN_7
#define Rs_GPIO_Port GPIOA
//DigitalOut displayEn( D12 );
#define En_GPIO_Pin GPIO_PIN_6
#define En_GPIO_Port GPIOA

I2C_HandleTypeDef hi2c1;

//=====[Declaration of external public global variables]=======================

//=====[Declaration and initialization of public global variables]=============

//=====[Declaration and initialization of private global variables]============

static display_t display;
static pcf8574_t pcf8574;
static bool initial8BitCommunicationIsCompleted;

//=====[Declarations (prototypes) of private functions]========================

static void displayPinWrite( uint8_t pinName, int value );
static void displayDataBusWrite( uint8_t dataByte );
static void displayCodeWrite( bool type, uint8_t dataBus );

//=====[Implementations of public functions]===================================

void displayInit( displayConnection_t connection )
{
    hi2c1.Instance = I2C1;

    display.connection = connection;
    
    if( display.connection == DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER) {
        pcf8574.address = PCF8574_I2C_BUS_8BIT_WRITE_ADDRESS;
        pcf8574.data = 0b00000000;
        hi2c1.Init.ClockSpeed = 100000;
        displayPinWrite( DISPLAY_PIN_A_PCF8574,  ON );
    } 
    
    initial8BitCommunicationIsCompleted = false;    

    delay( 50 );
    
    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_FUNCTION_SET | 
                      DISPLAY_IR_FUNCTION_SET_8BITS );
    delay( 5 );
            
    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_FUNCTION_SET | 
                      DISPLAY_IR_FUNCTION_SET_8BITS );
    delay( 1 ); 

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_FUNCTION_SET | 
                      DISPLAY_IR_FUNCTION_SET_8BITS );
    delay( 1 );  

    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_FUNCTION_SET | 
                              DISPLAY_IR_FUNCTION_SET_8BITS | 
                              DISPLAY_IR_FUNCTION_SET_2LINES |
                              DISPLAY_IR_FUNCTION_SET_5x8DOTS );
            delay( 1 );         
        break;
        
        case DISPLAY_CONNECTION_GPIO_4BITS:
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_FUNCTION_SET | 
                              DISPLAY_IR_FUNCTION_SET_4BITS );
            delay( 1 );  

            initial8BitCommunicationIsCompleted = true;  

            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_FUNCTION_SET | 
                              DISPLAY_IR_FUNCTION_SET_4BITS | 
                              DISPLAY_IR_FUNCTION_SET_2LINES |
                              DISPLAY_IR_FUNCTION_SET_5x8DOTS );
            delay( 1 );                                      
        break;
    }

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_DISPLAY_CONTROL |
                      DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_OFF |      
                      DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF |       
                      DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF );       
    delay( 1 );          

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_CLEAR_DISPLAY );       
    delay( 1 ); 

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_ENTRY_MODE_SET |
                      DISPLAY_IR_ENTRY_MODE_SET_INCREMENT |       
                      DISPLAY_IR_ENTRY_MODE_SET_NO_SHIFT );                  
    delay( 1 );           

    displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                      DISPLAY_IR_DISPLAY_CONTROL |
                      DISPLAY_IR_DISPLAY_CONTROL_DISPLAY_ON |      
                      DISPLAY_IR_DISPLAY_CONTROL_CURSOR_OFF |    
                      DISPLAY_IR_DISPLAY_CONTROL_BLINK_OFF );    
    delay( 1 );  
}

void displayCharPositionWrite( uint8_t charPositionX, uint8_t charPositionY )
{    
    switch( charPositionY ) {
        case 0:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_SET_DDRAM_ADDR |
                              ( DISPLAY_20x4_LINE1_FIRST_CHARACTER_ADDRESS +
                                charPositionX ) );
            delay( 1 );         
        break;
       
        case 1:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_SET_DDRAM_ADDR |
                              ( DISPLAY_20x4_LINE2_FIRST_CHARACTER_ADDRESS +
                                charPositionX ) );
            delay( 1 );         
        break;
       
        case 2:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_SET_DDRAM_ADDR |
                              ( DISPLAY_20x4_LINE3_FIRST_CHARACTER_ADDRESS +
                                charPositionX ) );
            delay( 1 );         
        break;

        case 3:
            displayCodeWrite( DISPLAY_RS_INSTRUCTION, 
                              DISPLAY_IR_SET_DDRAM_ADDR |
                              ( DISPLAY_20x4_LINE4_FIRST_CHARACTER_ADDRESS +
                                charPositionX ) );
            delay( 1 );         
        break;
    }
}

void displayStringWrite( const char * str )
{
    while (*str) {
        displayCodeWrite(DISPLAY_RS_DATA, *str++);
    }
}

//=====[Implementations of private functions]==================================

static void displayCodeWrite( bool type, uint8_t dataBus )
{
    if ( type == DISPLAY_RS_INSTRUCTION )
        displayPinWrite( DISPLAY_PIN_RS, DISPLAY_RS_INSTRUCTION);
    else
        displayPinWrite( DISPLAY_PIN_RS, DISPLAY_RS_DATA);
    displayPinWrite( DISPLAY_PIN_RW, DISPLAY_RW_WRITE );
    displayDataBusWrite( dataBus );
}

static void displayPinWrite( uint8_t pinName, int value )
{
    GPIO_PinState pin_value = (GPIO_PinState) value;

    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
            switch( pinName ) {
                case DISPLAY_PIN_D0: HAL_GPIO_WritePin(D0_GPIO_Port, D0_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_D1: HAL_GPIO_WritePin(D1_GPIO_Port, D1_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_D2: HAL_GPIO_WritePin(D2_GPIO_Port, D2_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_D3: HAL_GPIO_WritePin(D3_GPIO_Port, D3_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_D4: HAL_GPIO_WritePin(D4_GPIO_Port, D4_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_D5: HAL_GPIO_WritePin(D5_GPIO_Port, D5_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_D6: HAL_GPIO_WritePin(D6_GPIO_Port, D6_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_D7: HAL_GPIO_WritePin(D7_GPIO_Port, D7_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_RS: HAL_GPIO_WritePin(Rs_GPIO_Port, Rs_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_EN: HAL_GPIO_WritePin(En_GPIO_Port, En_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_RW: break; 
                default: break;
            }
            break;
        case DISPLAY_CONNECTION_GPIO_4BITS:
            switch( pinName ) {
                case DISPLAY_PIN_D4: HAL_GPIO_WritePin(D4_GPIO_Port, D4_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_D5: HAL_GPIO_WritePin(D5_GPIO_Port, D5_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_D6: HAL_GPIO_WritePin(D6_GPIO_Port, D6_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_D7: HAL_GPIO_WritePin(D7_GPIO_Port, D7_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_RS: HAL_GPIO_WritePin(Rs_GPIO_Port, Rs_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_EN: HAL_GPIO_WritePin(En_GPIO_Port, En_GPIO_Pin, pin_value); break;
                case DISPLAY_PIN_RW: break; 
                default: break;
            }
            break;
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            if ( value ) {
                switch( pinName ) {
                    case DISPLAY_PIN_D4: pcf8574.displayPinD4 = ON; break;
                    case DISPLAY_PIN_D5: pcf8574.displayPinD5 = ON; break;
                    case DISPLAY_PIN_D6: pcf8574.displayPinD6 = ON; break;
                    case DISPLAY_PIN_D7: pcf8574.displayPinD7 = ON; break;
                    case DISPLAY_PIN_RS: pcf8574.displayPinRs = ON; break;
                    case DISPLAY_PIN_EN: pcf8574.displayPinEn = ON; break;
                    case DISPLAY_PIN_RW: pcf8574.displayPinRw = ON; break;
                    case DISPLAY_PIN_A_PCF8574: pcf8574.displayPinA = ON; break;
                    default: break;
                }
            }
            else {
                switch( pinName ) {
                    case DISPLAY_PIN_D4: pcf8574.displayPinD4 = OFF; break;
                    case DISPLAY_PIN_D5: pcf8574.displayPinD5 = OFF; break;
                    case DISPLAY_PIN_D6: pcf8574.displayPinD6 = OFF; break;
                    case DISPLAY_PIN_D7: pcf8574.displayPinD7 = OFF; break;
                    case DISPLAY_PIN_RS: pcf8574.displayPinRs = OFF; break;
                    case DISPLAY_PIN_EN: pcf8574.displayPinEn = OFF; break;
                    case DISPLAY_PIN_RW: pcf8574.displayPinRw = OFF; break;
                    case DISPLAY_PIN_A_PCF8574: pcf8574.displayPinA = OFF; break;
                    default: break;
                }
            }     
            pcf8574.data = 0b00000000;
            if ( pcf8574.displayPinRs ) pcf8574.data |= 0b00000001; 
            if ( pcf8574.displayPinRw ) pcf8574.data |= 0b00000010; 
            if ( pcf8574.displayPinEn ) pcf8574.data |= 0b00000100; 
            if ( pcf8574.displayPinA  ) pcf8574.data |= 0b00001000; 
            if ( pcf8574.displayPinD4 ) pcf8574.data |= 0b00010000; 
            if ( pcf8574.displayPinD5 ) pcf8574.data |= 0b00100000; 
            if ( pcf8574.displayPinD6 ) pcf8574.data |= 0b01000000; 
            if ( pcf8574.displayPinD7 ) pcf8574.data |= 0b10000000; 

            HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)pcf8574.address, (uint8_t *)&pcf8574.data, 16, HAL_MAX_DELAY);
            break;    
    }
}

static void displayDataBusWrite( uint8_t dataBus )
{
    displayPinWrite( DISPLAY_PIN_EN, OFF );
    displayPinWrite( DISPLAY_PIN_D7, dataBus & 0b10000000 );
    displayPinWrite( DISPLAY_PIN_D6, dataBus & 0b01000000 );
    displayPinWrite( DISPLAY_PIN_D5, dataBus & 0b00100000 );
    displayPinWrite( DISPLAY_PIN_D4, dataBus & 0b00010000 );
    switch( display.connection ) {
        case DISPLAY_CONNECTION_GPIO_8BITS:
            displayPinWrite( DISPLAY_PIN_D3, dataBus & 0b00001000 );
            displayPinWrite( DISPLAY_PIN_D2, dataBus & 0b00000100 );  
            displayPinWrite( DISPLAY_PIN_D1, dataBus & 0b00000010 );      
            displayPinWrite( DISPLAY_PIN_D0, dataBus & 0b00000001 );
        break; 
              
        case DISPLAY_CONNECTION_GPIO_4BITS:
        case DISPLAY_CONNECTION_I2C_PCF8574_IO_EXPANDER:
            if ( initial8BitCommunicationIsCompleted == true) {
                displayPinWrite( DISPLAY_PIN_EN, ON );         
                delay( 1 );
                displayPinWrite( DISPLAY_PIN_EN, OFF );              
                delay( 1 );        
                displayPinWrite( DISPLAY_PIN_D7, dataBus & 0b00001000 );
                displayPinWrite( DISPLAY_PIN_D6, dataBus & 0b00000100 );  
                displayPinWrite( DISPLAY_PIN_D5, dataBus & 0b00000010 );      
                displayPinWrite( DISPLAY_PIN_D4, dataBus & 0b00000001 );                
            }
        break;
    
    }
    displayPinWrite( DISPLAY_PIN_EN, ON );              
    delay( 1 );
    displayPinWrite( DISPLAY_PIN_EN, OFF );  
    delay( 1 );                   
}