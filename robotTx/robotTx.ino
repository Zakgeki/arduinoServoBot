#include<Servo.h>
#include<RF24.h>
#include<SPI.h>
#include<printf.h>

#define RADIO_CHANNEL 0xE8E8F0F0E1LL

#define TX_LED 13

#define LEFT_Y  A0
#define LEFT_X  A1
#define RIGHT_Y A2
#define RIGHT_X A3
#define POSITIVE_DEADZONE 94
#define NEGATIVE_DEADZONE 86
#define CHIP_ENABLE 2
#define CHIP_SELECT 3

RF24 radio( CHIP_ENABLE, CHIP_SELECT );

int analogRaw;
int analogPos[ 4 ];
const int bufferSize = sizeof( analogPos );
const int nullPos[ 4 ] = { 90, 90, 90, 90 };

unsigned long crntMillis = 0;
unsigned long lastMillis = 0;

bool transmitOk;

void loop       ( void );
void getPos     ( void );
void printPos   ( void );
void setup      ( void );
void transmitPos( void );


void getPos( void )
{
    analogRaw = analogRead( LEFT_X );
    analogPos[ 0 ] = map( analogRaw, 0, 1023, 0, 180 );

    analogRaw = analogRead( LEFT_Y );
    analogPos[ 1 ] = map( analogRaw, 0, 1023, 180, 0 );

    analogRaw = analogRead( RIGHT_X );
    analogPos[ 2 ] = map( analogRaw, 0, 1023, 0, 180 );

    analogRaw = analogRead( RIGHT_Y );
    analogPos[ 3 ] = map( analogRaw, 0, 1023, 180, 0 );
}

void loop( void )
{
    getPos( );
    transmitPos( );
    printPos( );

}

void printPos( void )
{
    crntMillis = millis( );

    if ( crntMillis - lastMillis > 1000 )
    {
        lastMillis = crntMillis;

        printf( "\n\tLX: %d, LY: %d\r\n\tRX: %d, RY: %d\r\n\n",
        analogPos[ 0 ], analogPos[ 1 ], analogPos[ 2 ], analogPos[ 3 ] );
    }
}

void setup( void )
{
    Serial.begin( 115200 );
    printf_begin( );

    radio.begin( );
    radio.openWritingPipe( RADIO_CHANNEL );
    radio.printDetails( );
}

void transmitPos( void )
{
    if ( analogPos[ 0 ] > POSITIVE_DEADZONE || analogPos[ 0 ] < NEGATIVE_DEADZONE ||
        analogPos[ 1 ] > POSITIVE_DEADZONE || analogPos[ 1 ] < NEGATIVE_DEADZONE ||
        analogPos[ 2 ] > POSITIVE_DEADZONE || analogPos[ 2 ] < NEGATIVE_DEADZONE ||
        analogPos[ 3 ] > POSITIVE_DEADZONE || analogPos[ 3 ] < NEGATIVE_DEADZONE )
    {
        transmitOk = radio.write( analogPos, bufferSize );

        if ( transmitOk )
        {
            printf( "TX SUCCESS\r\n" );
        }
        else
        {
            printf( "TX FAILED\r\n" );
        }
    }
    else
    {
        transmitOk = radio.write( nullPos, bufferSize );
    }
}
