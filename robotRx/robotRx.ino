#include<Servo.h>
#include<RF24.h>
#include<SPI.h>
#include<printf.h>

#define SERVO_PINR 3
#define SERVO_PINL 4
#define RADIO_CHANNEL 0xE8E8F0F0E1LL

Servo servoL;
Servo servoR;

RF24 radio( 9, 10 );

bool bufferHasData;

int rxBuffer[ 4 ];
const int bufferSize = sizeof( rxBuffer );

unsigned long crntMillis = 0;
unsigned long lastMillis = 0;

void loop           ( void );
void printBuffer    ( void );
void readBuffer     ( void );
void setup          ( void );
void writeServoSpeed( void );

void readBuffer( void )
{
    if ( radio.available )
    {
        bufferHasData = true;

        while ( bufferHasData )
        {
            bufferHasData = radio.read( rxBuffer, sizeof( rxBuffer ) );
        }
    }
}

void setup( void )
{
    Serial.begin( 115200 );
    printf_begin( );

    servoL.attach( SERVO_PINL );
    servoR.attach( SERVO_PINR );

    servoL.write( 90 );
    servoR.write( 90 );

    radio.begin( );
    radio.openReadingPipe( 1, RADIO_CHANNEL );
    radio.startListening( );
    radio.printDetails( );
}

void loop( void )
{
    readBuffer( );
}

void printBuffer( void )
{
    crntMillis = millis( );

    if ( crntMillis - lastMillis > 1000 )
    {
        lastMillis = crntMillis;

        printf( "\n\tLX: %d, LY: %d\r\n\tRX: %d, RY: %d\r\n\n",
        rxBuffer[ 0 ], rxBuffer[ 1 ], rxBuffer[ 2 ], rxBuffer[ 3 ] );
    }
}

void writeServoSpeed( void )
{
    servoL.write( rxBuffer[ 1 ] );
    servoR.write( rxBuffer[ 2 ] );
}
