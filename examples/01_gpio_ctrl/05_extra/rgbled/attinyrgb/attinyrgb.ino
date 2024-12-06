/*
 * WS2812B LED controller
 */

/* libraries to be installed:
 * WS2812B Leds -> Adafruit Neopixel: https://github.com/adafruit/Adafruit_NeoPixel
 * I2C Slave -> TinyWiresS: https://github.com/nadavmatalon/TinyWireS
 */

#include <Adafruit_NeoPixel.h>
#include <TinyWireS.h>

/* *************************************************************************** */
/* DEFINES                                                                     */
/* *************************************************************************** */

#define I2C_SLAVE_ADDRESS ( 0x6A )  // Address of the slave
#define RGB_LEDS_PIN         ( 4 )
#define NUM_RGB_LEDS        ( 16 )
#define ID_LEN               ( 2 )
#define CMD_LEN              ( 1 )
#define CMD_EXTRA            ( 6 )
#define MAX_TRANSMISSION  ( 1 + ID_LEN + CMD_LEN + NUM_RGB_LEDS ) // REG+API+CMD+16 RGB leds

/* this is to define the API version for supported commands/effects */
#define API_VER_MJR ( 0 )
#define API_VER_MIN ( 0 )

enum led_pattern_type
{
    LEDS_OFF=0,         // Make sure this item is always *first* in the enum!
    LEDS_ON,
    LEDS_SET_PATTERN,
    LEDS_LAST           // Make sure this item is always *last* in the enum!
};

/* *************************************************************************** */
/* GLOBALS                                                                     */
/* *************************************************************************** */
volatile uint8_t request[ MAX_TRANSMISSION ];   // incoming I2C data.
volatile uint8_t request_len = 0;
volatile uint8_t response[ MAX_TRANSMISSION ];  // outgoing I2C data.
volatile uint8_t response_len = 0;

volatile uint8_t rv = 0;

volatile bool handleCommand = false;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel( NUM_RGB_LEDS, RGB_LEDS_PIN, NEO_GRB + NEO_KHZ800 );


void i2cReceive( uint8_t n )
{
    if ( n > MAX_TRANSMISSION )
    {
        n = MAX_TRANSMISSION;
    }
    for (uint8_t i = 0; i < n; i++)
    {
        if ( TinyWireS.available( ) )
        {
            request[ i ] = TinyWireS.read( );
            request_len = i + 1;
        }
        else
        {
            break;
        }
    }

    while ( TinyWireS.available( ) )
    {
        TinyWireS.read( );
    }

    handleCommand = true;
    rv = request_len;
}


void i2cRequest( )
{
    // just a ping
    TinyWireS.write( rv );
}


/* *************************************************************************** */
/* SETUP & MAIN                                                                */
/* *************************************************************************** */

void setup( )
{
    delay( 200 );
    pinMode( RGB_LEDS_PIN, OUTPUT );
    delay( 100 );
    colorWipe( pixels.Color( 0, 0, 0 ) );

    TinyWireS.begin( I2C_SLAVE_ADDRESS );
    TinyWireS.onRequest( i2cRequest );
    TinyWireS.onReceive( i2cReceive );

    delay( 500 );
    colorWipe( pixels.Color( 128, 0, 0 ),20 );
    delay( 500 );
    colorWipe( pixels.Color( 0, 128, 0 ) ,20);
    delay( 500 );
    colorWipe( pixels.Color( 0, 0, 128 ),20 );
    delay( 500 );
    colorWipe( pixels.Color( 0, 0, 0 ) ,20);
}


/*
 * Request byte syntax
 * ========================
 * byte 0: REGISTER ADDR
 * byte 1: API MJR NUM
 * byte 2: API MIN NUM
 * byte 3: CMD HIGH
 * byte 4: CMD LOW
 */
void loop( )
{
    if ( handleCommand )
    {
        handleCommand = false;

        /* CHECK LEN: REGISTER, API and CMD always needed */
        if ( request_len >= 4 )
        {
            /* CHECK REGISTER */
            if ( request[ 0 ] == 0 )
            {
                /* CHECK API (16bit) */
                if ( ( request[ 1 ] == API_VER_MJR ) && ( request[ 2 ] == API_VER_MIN ) )
                {
                    /* CHECK COMMAND (16bit) */
                    switch ( request[ 3 ] )
                    {
                        case LEDS_OFF:
                            /* all black */
                            colorWipe( pixels.Color( 0, 0, 0 ) );
                            rv = 0; // confirm command accepted
                            break;

                        case LEDS_ON:
                            /* all white, modulate intensity */
                            if ( request_len > 4 )
                            {
                                colorWipe( pixels.Color( request[ 4 ], request[ 4 ], request[ 4 ] ) );
                                rv = 0; // confirm command accepted
                            }
                            else
                            {
                                /* all white,fixed intensity */
                                colorWipe( pixels.Color( 100, 100, 100 ) );
                                rv = 0; // confirm command accepted
                            }
                            break;

                        case LEDS_SET_PATTERN:
                            /* custom color pattern */
                            if ( request_len >= MAX_TRANSMISSION )
                            {
                                for (int i = 0; i < NUM_RGB_LEDS; i++)
                                {
                                    if ( request[ i + 4 ] >= 2 )
                                    {
                                        if ( request_len >= MAX_TRANSMISSION + CMD_EXTRA )
                                        {
                                            pixels.setPixelColor( i, pixels.Color( request[ MAX_TRANSMISSION + 0 ], request[ MAX_TRANSMISSION + 1 ], request[ MAX_TRANSMISSION + 2 ] ) );
                                        }
                                        else
                                        {
                                            pixels.setPixelColor( i, pixels.Color( 30, 0, 0 ) );
                                        }
                                    }
                                    else if ( request[ i + 4 ] >= 1 )
                                    {
                                        if ( request_len >= MAX_TRANSMISSION + CMD_EXTRA )
                                        {
                                            pixels.setPixelColor( i, pixels.Color( request[ MAX_TRANSMISSION + 3 ], request[ MAX_TRANSMISSION + 4 ], request[ MAX_TRANSMISSION + 5 ] ) );
                                        }
                                        else
                                        {
                                            pixels.setPixelColor( i, pixels.Color( 0, 30, 0 ) );
                                        }
                                    }
                                    else
                                    {
                                        pixels.setPixelColor( i, pixels.Color( 0, 0, 0 ) );
                                    }
                                    pixels.show( );
                                }
                                rv = 0; // confirm command accepted
                            }
                    }
                }
            }
        }
    }

    // This needs to be here
    TinyWireS_stop_check( );
}


/* *************************************************************************** */

/*
 * LED PATTERN METHODS
 * Make sure that any methods here do NOT iterate for long periods of time
 * or I2C will not be responsive to user input.
 * Instead, use static variables to control the state of long-changing patterns.
 */
/* *************************************************************************** */

void colorWipe( uint32_t c )
{
    for (uint16_t i = 0; i < pixels.numPixels( ); i++)
    {
        pixels.setPixelColor( i, c );
    }
    pixels.show( );
}


/*
 * Fill the dots one after the other with a color using the specified delay
 */
void colorWipe( uint32_t c, uint8_t wait )
{
    for (uint16_t i = 0; i < pixels.numPixels( ); i++)
    {
        pixels.setPixelColor( i, c );
        pixels.show( );
        delay( wait );
    }
}
